#include "frostwm/monitor.h"

#include <stdlib.h>
#include "frostwm/log.h"

static void
set_monitor_mode (struct frost_monitor *restrict mon)
{
  struct wlr_output_state state;
  struct wlr_output_mode *mode;
\
  // skip this if no mode exists for the output
  if (!wl_list_empty (&mon->output->modes))
    {
      frost_log (LOG_DEBUG, "Skipping modeset: no available modes");
      return;
    }

  // set default monitor mode
  // TODO: read from config file to set specific mode
  wlr_output_state_init (&state);
  mode = wl_container_of (mon->output->modes.prev, mode, link);
  wlr_output_state_set_mode (&state, mode);
  frost_log (LOG_DEBUG, "Setting mode to %dx%d@%d", mode->width, mode->height,
             mode->refresh);

  // commit selected mode
  if (!wlr_output_commit_state (mon->output, &state))
    {
      frost_log (LOG_ERROR, "Output mode commit failed");
    }
  wlr_output_state_finish (&state);
}

static void
on_monitor_destroy_notify (struct wl_listener *listener, void *data)
{
  struct frost_monitor *mon
      = wl_container_of (listener, mon, on_monitor_destroy);

  // remove any list links present for cleanup
  frost_log (LOG_WARN, "Destroying monitor %s", mon->output->name);
  wl_list_remove (&mon->link);
  wl_list_remove (&mon->on_monitor_destroy.link);

  // free memory from struct
  free (mon);
}

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
  set_monitor_mode (mon);

  // configure events
  mon->on_monitor_destroy.notify = &on_monitor_destroy_notify;
  wl_signal_add (&mon->output->events.destroy, &mon->on_monitor_destroy);

  // add monitor to session state list
  wl_list_insert (&session->monitors, &mon->link);
}
