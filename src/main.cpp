#include <signal.h>

extern "C"
{
#include <wayland-server.h>
#include <wlr/util/log.h>
}

#include "frostwm/session.h"

void
sigint_callback (int signal)
{
  if (signal == SIGINT)
    {
      frost_session::get_instance ().terminate ();
    }
}

int
main (int argc, char *argv[])
{
  wlr_log_init (WLR_DEBUG, NULL);
  signal (SIGINT, &sigint_callback);

  frost_session &session = frost_session::get_instance ();
  session.start ();
}
