#include "frostwm/input.h"

#include <stdlib.h>
#include <wlr/types/wlr_input_device.h>
#include "frostwm/log.h"

static void
on_keyboard_pressed_notify (struct wl_listener *listener, void *data)
{
  // the function name is misleading, it listens to both press and release
  // events
  struct wlr_keyboard_key_event *event;
  struct frost_keyboard         *kbd;
  xkb_keysym_t                  *keysym;
  uint32_t                       modifiers;

  event     = data;
  kbd       = wl_container_of (listener, kbd, on_keyboard_pressed);
  modifiers = wlr_keyboard_get_modifiers (kbd->keyboard);

  frost_log (LOG_DEBUG, "%u", event->keycode);
}

static void
on_keyboard_destroy_notify (struct wl_listener *listener, void *data)
{
  struct frost_keyboard *kbd
      = wl_container_of (listener, kbd, on_keyboard_destroy);

  // remove from lists
  wl_list_remove (&kbd->on_keyboard_destroy.link);
  wl_list_remove (&kbd->on_keyboard_pressed.link);

  // free struct memory
  free (kbd);
}

void
on_keyboard_connected_notify (struct wl_listener *listener, void *data)
{
  struct wlr_input_device *dev;
  struct wlr_keyboard     *kb;
  struct frost_keyboard   *fkbd;

  dev = data;
  if (dev->type != WLR_INPUT_DEVICE_KEYBOARD)
    {
      return;
    }

  // register keyboard
  frost_log (LOG_DEBUG, "Registering keyboard input source...");
  kb             = wlr_keyboard_from_input_device (dev);
  fkbd           = malloc (sizeof (struct frost_keyboard));
  fkbd->keyboard = kb;

  // register events
  fkbd->on_keyboard_destroy.notify = &on_keyboard_destroy_notify;
  wl_signal_add (&dev->events.destroy, &fkbd->on_keyboard_destroy);
  fkbd->on_keyboard_pressed.notify = &on_keyboard_pressed_notify;
  wl_signal_add (&kb->events.key, &fkbd->on_keyboard_pressed);

  frost_log (LOG_DEBUG, "Keyboard %s registered", dev->name);
}
