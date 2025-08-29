#include "frostwm/session.h"

#include <memory>
#include <stdexcept>

extern "C"
{
#include <wayland-server.h>
#include <wlr/util/log.h>
}

void
wl_display_destroyer::operator() (wl_display *display) const
{
  wl_display_destroy (display);
}

void
wlr_backend_destroyer::operator() (wlr_backend *backend) const
{
  wlr_backend_destroy (backend);
}

frost_session::frost_session ()
    : _display (wl_display_create ()),
      _event_loop (wl_display_get_event_loop (_display.get ()))
{
  _backend = std::unique_ptr<wlr_backend, wlr_backend_destroyer> (
      wlr_backend_autocreate (_event_loop, &_session));

  if (_backend == NULL)
    {
      throw std::runtime_error ("unable to initialize wlroots backend");
    }
}

frost_session::~frost_session () {}

void
frost_session::start ()
{
  wlr_backend_start (_backend.get ());
  wl_display_run (_display.get ());
}

void
frost_session::terminate ()
{
  wlr_log (WLR_INFO, "terminating frostwm...");
  wl_display_terminate (_display.get ());
}
