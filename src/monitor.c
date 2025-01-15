#include "frostwm/monitor.h"

#include <stdlib.h>
#include "frostwm/log.h"

/**
 * @brief Automatically sets the given monitor's mode.
 *
 * TODO: this should later be able to set mode according to the user's
 * configuration file.
 *
 * @internal
 */
static void
set_monitor_mode (struct frost_monitor *restrict mon,
                  struct wlr_allocator *alloc, struct wlr_renderer *render)
{
  struct wlr_output_state state;
  struct wlr_output_mode *mode;

  // skip this if no mode exists for the output
  if (!wl_list_empty (&mon->output->modes))
    {
      frost_log (LOG_DEBUG, "Skipping modeset: no available modes");
      return;
    }

  // set default monitor mode
  wlr_output_state_init (&state);
  wlr_output_init_render (mon->output, alloc, render);
  wlr_output_state_set_enabled (&state, true);

  mode = wlr_output_preferred_mode (mon->output);
  wlr_output_state_set_mode (&state, mode);

  if (mode != NULL)
    {
      frost_log (LOG_DEBUG, "Setting mode to %dx%d@%d", mode->width,
                 mode->height, mode->refresh);
    }

  // commit selected mode
  if (!wlr_output_commit_state (mon->output, &state))
    {
      frost_log (LOG_ERROR, "Output mode commit failed");
    }
  wlr_output_state_finish (&state);
}

/**
 * @brief Updates monitor mode upon request.
 *
 * This is usually for running FrostWM under another X11 or Wayland session.
 * This is due to how windows are resizable, and screen dimensions must be
 * updated accordingly.
 *
 * @internal
 */
static void
on_monitor_request_state_notify (struct wl_listener *listener, void *data)
{
  struct frost_monitor *mon
      = wl_container_of (listener, mon, on_monitor_request_state);
  const struct wlr_output_event_request_state *event = data;
  wlr_output_commit_state (mon->output, event->state);
}

/**
 * @brief Triggers when a monitor is destroyed.
 *
 * @internal
 */
static void
on_monitor_destroy_notify (struct wl_listener *listener, void *data)
{
  struct frost_monitor *mon
      = wl_container_of (listener, mon, on_monitor_destroy);

  // remove any list links present for cleanup
  frost_log (LOG_WARN, "Destroying monitor %s", mon->output->name);
  wl_list_remove (&mon->link);
  wl_list_remove (&mon->on_monitor_request_state.link);
  wl_list_remove (&mon->on_monitor_destroy.link);

  // free memory from struct
  free (mon);
}

/**
 * @brief Triggers when a monitor has been connected and sets mode.
 *
 * This callback should only handle setting up events for the related monitor.
 * For setting mode, modify @ref set_monitor_mode instead.
 *
 * @internal
 */
void
on_monitor_connected_notify (struct wl_listener *listener, void *data)
{
  struct frost_session *session;
  struct frost_monitor *mon;

  session     = wl_container_of (listener, session, on_monitor_connected);
  mon         = malloc (sizeof (struct frost_monitor));
  mon->output = data;

  // set monitor mode
  frost_log (LOG_DEBUG, "Monitor \"%s\" connected, setting mode",
             mon->output->name);
  set_monitor_mode (mon, session->allocator, session->renderer);

  // configure events
  mon->on_monitor_request_state.notify = &on_monitor_request_state_notify;
  mon->on_monitor_destroy.notify       = &on_monitor_destroy_notify;

  wl_signal_add (&mon->output->events.request_state,
                 &mon->on_monitor_request_state);
  wl_signal_add (&mon->output->events.destroy, &mon->on_monitor_destroy);

  // add monitor to session state list
  wl_list_insert (&session->monitors, &mon->link);
}
