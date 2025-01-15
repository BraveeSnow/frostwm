#include "frostwm/session.h"

int
main (int argc, char *argv[])
{
  struct frost_session *session;

  session = frost_session_init ();
  frost_session_start (session);
  frost_session_destroy (session);
}