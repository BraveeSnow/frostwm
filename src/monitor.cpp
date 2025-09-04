#include <frostwm/monitor.h>

#include <memory>
#include <utility>

extern "C"
{
#include <wlr/render/allocator.h>
#include <wlr/util/log.h>
}

// type destroyers

void
wlr_renderer_destroyer::operator() (wlr_renderer *render) const
{
  wlr_renderer_destroy (render);
}

void
wlr_allocator_destroyer::operator() (wlr_allocator *alloc) const
{
  wlr_allocator_destroy (alloc);
}

// --- friend functions ---

void
_on_output_connected_callback (wl_listener *listener, void *data)
{
  wlr_output *monitor = reinterpret_cast<wlr_output *> (data);
  wlr_output_mode *preferred = wlr_output_preferred_mode (monitor);
  frost_monitor_controller *controller
      = wl_container_of (listener, controller, _on_output_connected);
  wlr_output_state state;
  std::unique_ptr<frost_monitor> container;

  wlr_output_state_init (&state);
  wlr_output_init_render (monitor, controller->_alloc.get (),
                          controller->_render.get ());

  wlr_log (WLR_INFO, "Monitor \"%s\" connected", monitor->name);
  if (preferred != NULL)
    {
      wlr_log (WLR_INFO, "Monitor \"%s\" defaulting to mode %dx%d",
               monitor->name, preferred->width, preferred->height);
    }

  wlr_output_state_set_enabled (&state, true);
  wlr_output_state_set_mode (&state, preferred);

  if (!wlr_output_commit_state (monitor, &state))
    {
      wlr_log (WLR_ERROR, "Failed to commit state to \"%s\"", monitor->name);
    }

  wlr_output_state_finish (&state);

  container = std::make_unique<frost_monitor> (frost_monitor (monitor));
  controller->_monitors.push_back (std::move (container));

  wlr_log (WLR_DEBUG, "Monitor count at %ld", controller->_monitors.size ());
}

void
_on_output_destroyed_callback (struct wl_listener *listener, void *data)
{
  frost_monitor *mon = wl_container_of (listener, mon, _on_output_destroyed);

  // unhook monitor event listeners
  wl_list_remove (&mon->_on_output_destroyed.link);

  wlr_log (WLR_INFO, "Monitor \"%s\" disconnected", mon->_monitor->name);
}

// --- class frost_monitor ---

frost_monitor::frost_monitor (wlr_output *monitor) : _monitor (monitor)
{
  _on_output_destroyed.notify = &_on_output_destroyed_callback;
  wl_signal_add (&_monitor->events.destroy, &_on_output_destroyed);
}

frost_monitor::~frost_monitor ()
{
  wl_list_remove (&_on_output_destroyed.link);
}

// --- class frost_monitor_controller ---

frost_monitor_controller::frost_monitor_controller (wlr_backend *backend)
{
  set_backend (backend);
}

frost_monitor_controller::~frost_monitor_controller ()
{
  // unhook all listeners
  wl_list_remove (&_on_output_connected.link);
}

void
frost_monitor_controller::set_backend (wlr_backend *backend)
{
  // create additional wlroots objects
  _render = std::unique_ptr<wlr_renderer, wlr_renderer_destroyer> (
      wlr_renderer_autocreate (backend));
  _alloc = std::unique_ptr<wlr_allocator, wlr_allocator_destroyer> (
      wlr_allocator_autocreate (backend, _render.get ()));

  // configure listeners
  _on_output_connected.notify = &_on_output_connected_callback;
  wl_signal_add (&backend->events.new_output, &_on_output_connected);
}
