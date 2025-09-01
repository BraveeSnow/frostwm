#ifndef FROST_MONITOR_H_
#define FROST_MONITOR_H_

#include <experimental/memory>
#include <vector>

extern "C"
{
#include <wayland-server.h>
#include <wlr/backend.h>
}

struct wlr_renderer_destroyer
{
  void operator() (wlr_renderer *render) const;
};

struct wlr_allocator_destroyer
{
  void operator() (wlr_allocator *alloc) const;
};

class frost_monitor
{
public:
  frost_monitor (std::experimental::observer_ptr<wlr_output> monitor);
  ~frost_monitor ();

private:
  std::experimental::observer_ptr<wlr_output> _monitor;

  // event listeners and callbacks
  wl_listener _on_output_destroyed;
  friend void _on_output_destroyed_callback (struct wl_listener *listener,
                                             void *data);
};

class frost_monitor_controller
{
public:
  frost_monitor_controller () = default;
  frost_monitor_controller (
      std::experimental::observer_ptr<wlr_backend> backend);
  ~frost_monitor_controller ();

  // TODO: refactor this class to always require the backend passed to the
  // constructor when possible (skip the default constructor)
  void set_backend (std::experimental::observer_ptr<wlr_backend> backend);

private:
  std::vector<std::unique_ptr<frost_monitor>> _monitors;

  // wayland/wlroots fields
  std::unique_ptr<wlr_allocator, wlr_allocator_destroyer> _alloc;
  std::unique_ptr<wlr_renderer, wlr_renderer_destroyer> _render;

  // event listeners and callbacks
  wl_listener _on_output_connected;
  friend void _on_output_connected_callback (wl_listener *listener,
                                             void *data);
};

#endif
