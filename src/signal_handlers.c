#include <signal.h>
#include "signal_handlers.h"

void catch_sigint(int signalNo)
{
  // TODO: File this!

        signal(signalNo,SIG_IGN);

}

void catch_sigtstp(int signalNo)
{
  // TODO: File this!

        signal(signalNo,SIG_IGN);
}

