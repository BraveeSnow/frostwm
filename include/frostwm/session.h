/**
 * @file session.h
 * @author David Long (david.long@snows.world)
 * @brief Session management for FrostWM.
 * @version 0.1.0
 * @date 2025-01-03
 *
 * @copyright Copyright (c) 2025
 */

#ifndef _FROST_SESSION_H
#define _FROST_SESSION_H

#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_seat.h>

/**
 * @struct frost_session
 * @brief Main session struct to keep track of session state.
 *
 * The session class is to contain practically everything pertaining to the
 * state of the window manager.
 */
struct frost_session
{
  // wayland and wlroots main components
  struct wl_display     *display;
  struct wl_event_loop  *event_loop;
  struct wlr_backend    *backend;
  struct wlr_renderer   *renderer;
  struct wlr_allocator  *allocator;
  struct wlr_compositor *compositor;
  struct wlr_seat       *seat;

  // state
  struct wl_list         monitors; // frost_monitor::link
  struct frost_keyboard *keyboard;

  // event handling
  struct wl_listener on_monitor_connected;
  struct wl_listener on_keyboard_connected;
};

/**
 * @brief Initializes the FrostWM session.
 *
 * The returned pointer should be destroyed when unused via
 * @ref frost_session_destroy.
 *
 * @return struct frost_session * A session struct containing runtime
 * information or NULL when an error occurred.
 */
struct frost_session *frost_session_init (void);

/**
 * @brief Initiates the event loop.
 *
 * First call @ref frost_session_init, then pass the resulting @ref
 * frost_session to this function. Calling this function will start the event
 * loop.
 *
 * Note that this function will block the thread it runs on.
 *
 * @param session The session created by @ref frost_session_init.
 */
void frost_session_start (struct frost_session *session);

/**
 * @brief Starts the shutdown process.
 * 
 * Calling this before @ref frost_session_start will cause the window manager
 * to be unable to start.
 */
void frost_session_shutdown (void);

/**
 * @brief Destroys the FrostWM session struct.
 *
 * This should really only be called when the session has been terminated by
 * the user.
 *
 * @param session A pointer to the session instance.
 */
void frost_session_destroy (struct frost_session *session);

#endif
