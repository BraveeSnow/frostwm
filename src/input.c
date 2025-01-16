#include "frostwm/input.h"

#include <stdlib.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_seat.h>
#include <xkbcommon/xkbcommon.h>
#include "frostwm/log.h"
#include "frostwm/session.h"

static void
on_keyboard_pressed_notify (struct wl_listener *listener, void *data)
{
  // the function name is misleading, it listens to both press and release
  // events
  struct wlr_keyboard_key_event *event;
  struct frost_keyboard         *kbd;
  const xkb_keysym_t            *keysyms;
  uint32_t                       modifiers;
  int                            num_keys;
  char                           utf8char[7] = { 0 };

  event     = data;
  kbd       = wl_container_of (listener, kbd, on_keyboard_pressed);
  modifiers = wlr_keyboard_get_modifiers (kbd->keyboard);

  // only register key down
  if (event->state != WL_KEYBOARD_KEY_STATE_PRESSED)
    {
      return;
    }

  num_keys = xkb_state_key_get_syms (kbd->keyboard->xkb_state,
                                     event->keycode + 8, &keysyms);

  if (num_keys < 1)
    {
      return;
    }

  for (int i = 0; i < num_keys; i++)
    {
      xkb_keysym_to_utf8 (keysyms[i], utf8char, sizeof (utf8char));
      frost_log (LOG_DEBUG, "Pressed: %s", utf8char);
    }

  if (keysyms[0] == XKB_KEY_Escape)
    {
      frost_log (LOG_INFO, "Gracefully exiting...");
      wl_display_terminate (kbd->session->display);
    }
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
  struct frost_session    *session;
  struct wlr_input_device *dev;
  struct wlr_keyboard     *kb;
  struct frost_keyboard   *fkbd;
  struct xkb_context      *xkb_context;
  struct xkb_keymap       *keymap;

  session = wl_container_of (listener, session, on_keyboard_connected);
  dev     = data;
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

  // create xkb keymap
  xkb_context = xkb_context_new (XKB_CONTEXT_NO_FLAGS);
  keymap      = xkb_keymap_new_from_names (xkb_context, NULL,
                                           XKB_KEYMAP_COMPILE_NO_FLAGS);
  wlr_keyboard_set_keymap (kb, keymap);
  xkb_keymap_unref (keymap);
  xkb_context_unref (xkb_context);

  // add keyboard to seat
  wlr_seat_set_keyboard (session->seat, kb);

  // record in session
  fkbd->session = session;

  frost_log (LOG_DEBUG, "Keyboard %s registered", dev->name);
}
