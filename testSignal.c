#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include <stddef.h>

void custom_handler()
{
  printf(1, "Custom Handler Called!\n");
  return;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf(1, "Usage: testSignal signum\n");
    exit();
  }
  int pid, ppid, signum;
  signum = atoi(argv[1]);
  pid = fork();
  if (pid == 0)
  {
    sleep(200);
    ppid = getppid();
    printf(1, "parents pid = %d\n", ppid);
    kill(ppid, signum);
    printf(1, "Signal Sent\n");
    // sleep(1000);
    kill(ppid, 19);
    printf(1, "child working its next cmd\n");
  }
  else
  {
    // struct sigaction act;
    // act.sa_handler = &custom_handler;
    // act.sigmask = 0;
    // sigaction(signum, &act, NULL);
    printf(1, "Parent\n");
    wait();
    printf(1, "child done.\n");
    printf(1, "now parent exiting...\n");
  }

  exit();
}
