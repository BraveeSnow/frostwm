#include "frostwm/session.h"

#include <stdlib.h>
#include "frostwm/input.h"
#include "frostwm/log.h"
#include "frostwm/monitor.h"

#define DEFAULT_SEAT_NAME "seat0"

struct frost_session *
frost_session_init (void)
{
  struct frost_session *session = malloc (sizeof (struct frost_session));

  // TODO: handle any failures and abort early
  frost_log (LOG_INFO, "Initializing session");
  session->display    = wl_display_create ();
  session->event_loop = wl_display_get_event_loop (session->display);
  session->backend    = wlr_backend_autocreate (session->event_loop, NULL);
  session->renderer   = wlr_renderer_autocreate (session->backend);

  wlr_renderer_init_wl_display (session->renderer, session->display);

  session->allocator
      = wlr_allocator_autocreate (session->backend, session->renderer);
  session->compositor = wlr_compositor_create (
      session->display, wl_compositor_interface.version, session->renderer);
  session->seat = wlr_seat_create (session->display, DEFAULT_SEAT_NAME);

  // register event handlers
  wl_list_init (&session->monitors);
  session->on_monitor_connected.notify = &on_monitor_connected_notify;
  wl_signal_add (&session->backend->events.new_output,
                 &session->on_monitor_connected);
  session->on_keyboard_connected.notify = &on_keyboard_connected_notify;
  wl_signal_add (&session->backend->events.new_input,
                 &session->on_keyboard_connected);

  return session;
}

void
frost_session_start (struct frost_session *session)
{
  // prepare for running display
  if (!wlr_backend_start (session->backend))
    {
      frost_log (LOG_ERROR, "Failed to start wlroots backend");
      return;
    }

  // ! display loop - blocks main thread
  wl_display_run (session->display);
}

void
frost_session_destroy (struct frost_session *session)
{
  // unhook events
  wl_list_remove (&session->on_monitor_connected.link);
  wl_list_remove (&session->on_keyboard_connected.link);

  // remove main components
  wlr_seat_destroy (session->seat);
  wlr_allocator_destroy (session->allocator);
  wlr_renderer_destroy (session->renderer);
  wlr_backend_destroy (session->backend);
  wl_display_destroy (session->display);

  // remove malloc'd object
  free (session);
}
