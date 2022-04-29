#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

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
    printf(1, "child working its next cmd\n");
  }
  else
  {
    printf(1, "Parent\n");
    wait();
    printf(1, "child done.\n");
    sleep(2000);
    printf(1, "now parent exiting...\n");
  }
  // exit();
}