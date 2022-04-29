#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include <stddef.h>
#define SIGTEST 18

void custom_handler()
{
  printf(1, "Custom Handler Called!\n");
  return;
}

int main()
{
  int pid, ppid;
  pid = fork();
  if (pid == 0)
  {
    sleep(200);
    ppid = getppid();
    printf(1, "parents pid = %d\n", ppid);
    kill(ppid, 18);
    printf(1, "Signal Sent\n");
    sleep(200); // sleep child so that parent will keep waiting for wait() to return
    printf(1, "child working its next cmd\n");
  }
  else
  {
    struct sigaction act;
    act.sa_handler = &custom_handler;
    act.sigmask = 0;
    sigaction(18, &act, NULL);
    printf(1, "Parent\n");
    wait();
    printf(1, "child done.\n");
    printf(1, "now parent exiting...\n");
  }
  exit();
}
