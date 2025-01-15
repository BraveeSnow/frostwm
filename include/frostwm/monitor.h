/**
 * @file monitor.h
 * @author David Long (david.long@snows.world)
 * @brief Monitor output handling.
 * @version 0.1.0
 * @date 2025-01-03
 *
 * @copyright Copyright (c) 2025
 */

#ifndef _FROST_MONITOR_H
#define _FROST_MONITOR_H

#include <wayland-server-core.h>
#include <wlr/interfaces/wlr_output.h>
#include "frostwm/session.h"

/**
 * @struct frost_monitor
 * @brief Keeps track of the state of a monitor.
 */
struct frost_monitor
{
  struct wlr_output *output;

  struct wl_listener on_monitor_destroy;
  struct wl_listener on_monitor_request_state;

  struct wl_list link;
};

/**
 * @brief Notify function called when a monitor has been connected.
 *
 * @param listener The @ref wl_listener associated with the event.
 * @param data A @ref wlr_output pointer referencing the connected monitor.
 */
void on_monitor_connected_notify (struct wl_listener *listener, void *data);

#endif
