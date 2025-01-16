/**
 * @file input.h
 * @author David Long (david.long@snows.world)
 * @brief Input handling library for FrostWM.
 * @version 0.1.0
 * @date 2025-01-08
 *
 * @copyright Copyright (c) 2025
 */

#ifndef _FROST_INPUT_H
#define _FROST_INPUT_H

#include <wayland-server-core.h>
#include <wlr/types/wlr_keyboard.h>
#include "frostwm/session.h"

struct frost_keyboard
{
  struct wlr_keyboard  *keyboard;
  struct frost_session *session;

  struct wl_listener on_keyboard_destroy;
  struct wl_listener on_keyboard_pressed;
};

void on_keyboard_connected_notify (struct wl_listener *listener, void *data);

#endif
