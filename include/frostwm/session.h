#ifndef FROST_SESSION_H_
#define FROST_SESSION_H_

#include <memory>

extern "C"
{
#include <wayland-server.h>
#include <wlr/backend.h>
}

#include <frostwm/monitor.h>

struct wl_display_destroyer
{
  void operator() (wl_display *display) const;
};

struct wlr_backend_destroyer
{
  void operator() (wlr_backend *backend) const;
};

class frost_session
{
public:
  static frost_session &
  get_instance ()
  {
    static frost_session instance;
    return instance;
  }

  void start ();
  void terminate ();

private:
  frost_session ();
  ~frost_session ();

  // wayland members
  std::unique_ptr<wl_display, wl_display_destroyer> _display;
  wl_event_loop *_event_loop;

  // wlroots members
  std::unique_ptr<wlr_backend, wlr_backend_destroyer> _backend;
  wlr_session *_session;

  // placed here because these are destroyed first
  // DO NOT MOVE ABOVE WAYLAND FIELDS
  frost_monitor_controller _monitor_controller;
};

#endif
