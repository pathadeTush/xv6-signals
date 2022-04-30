#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include <stddef.h>

#define SIGKILL 9
#define SLEEP_TIME 100

typedef struct testcase
{
  int signum;
  char *name;
  char kill_call_required;
} testcase;

void custom_handler()
{
  printf(1, "Custom Handler Called!\n");
  return;
}

int test_testcase(testcase t)
{
  printf(1, "\n====== testing %s ======\n", t.name);
  int pid;
  pid = fork();
  if (pid == 0)
  {
    pid = getpid();
    printf(1, "childs pid = %d\n", pid);
    while (1);
  }
  else
  {
    printf(1, "parent\n");
    sleep(SLEEP_TIME);
    printf(1, "pid: %d\n", pid);
    kill(pid, t.signum); // stop the child process
    if (t.kill_call_required)
    {
      sleep(SLEEP_TIME);
      kill(pid, SIGKILL); // kill the child process
      printf(1, "sent kill signal to child\n");
    }
    wait();
    printf(1, "child exited.\n");
    printf(1, "now parent exiting...\n");
  }
  printf(1, "====== %s passed :) ======\n", t.name);
  return 1;
}

int test_sigprocmask(){
  printf(1, "\n====== testing sigprocmask ======\n");
  int pid, signum_to_be_masked = 19;
  pid = fork();
  if (pid == 0)
  {
    pid = getpid();
    printf(1, "masking %d signum\n", signum_to_be_masked);
    sigprocmask((1 << signum_to_be_masked));
    printf(1, "childs pid = %d\n", pid);
    while (1);
  }
  else
  {
    printf(1, "parent\n");
    sleep(SLEEP_TIME);
    printf(1, "pid: %d\n", pid);
    printf(1, "calling masked signal %d\n", signum_to_be_masked);
    kill(pid, signum_to_be_masked); // stop the child process
    sleep(100);
    kill(pid, SIGKILL);
    wait();
    printf(1, "child exited.\n");
    printf(1, "now parent exiting...\n");
  }
  printf(1, "====== sigprocmask passed :) ======\n");
  return 1;
}

int main(int argc, char *argv[])
{
  testcase kill_test, sigstop_test, sigcont_test;

  // kill test
  kill_test.kill_call_required = 0;
  kill_test.name = "kill";
  kill_test.signum = SIGKILL;
  test_testcase(kill_test);

  // sigstop test
  sigstop_test.kill_call_required = 1;
  sigstop_test.name = "sigstop";
  sigstop_test.signum = SIGKILL;
  test_testcase(sigstop_test);

  // sigcont test
  sigcont_test.kill_call_required = 1;
  sigcont_test.name = "sigcont";
  sigcont_test.signum = SIGKILL;
  test_testcase(sigcont_test);

  test_sigprocmask();

  exit();
}
