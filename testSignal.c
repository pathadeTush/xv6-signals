#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include <stddef.h>

#define SIGKILL 9
#define SIGSTOP 17
#define SIGCONT 19
#define SLEEP_TIME 100

typedef struct testcase
{
  int signum;
  char *name;
  char kill_call_required;
  char override_df_handler;
  struct sigaction *s;
} testcase;

void sigdfl_handler()
{
  printf(1, "--- SIG_DFL Custom Handler Called!\n");
  return;
}

void sigign_handler()
{
  printf(1, "--- SIG_IGN Custom Handler Called!\n");
  return;
}

void sigcont_handler()
{
  printf(1, "--- SIG_CONT Custom Handler Called!\n");
  return;
}

void test_testcase(testcase t)
{
  printf(1, "\n=============== %s test ===============\n", t.name);
  int pid;
  pid = fork();
  if (pid == 0)
  {
    pid = getpid();
    printf(1, "Child PID: %d\n", pid);
    if(t.override_df_handler){
      sigaction(t.signum, t.s, NULL);
    }
    while (1);
  }
  else
  {
    sleep(SLEEP_TIME);
    kill(pid, t.signum); // stop the child process
    printf(1, "Sending %d signal to child\n", t.signum);
    if (t.kill_call_required)
    {
      sleep(SLEEP_TIME);
      printf(1, "Sending SIGKILL to child\n");
      kill(pid, SIGKILL); // kill the child process
    }
    wait();
    printf(1, "Child exited\n");
    printf(1, "Parent exiting\n");
  }
  printf(1, "=============== %s pass ===============\n", t.name);
  return;
}

void test_sigprocmask(){
  printf(1, "\n=============== sigprocmask test ===============\n");
  int pid, signum_to_be_masked = 19;
  pid = fork();
  if (pid == 0)
  {
    pid = getpid();
    printf(1, "Child PID: %d\n", pid);
    printf(1, "Masking %d signum for child\n", signum_to_be_masked);
    sigprocmask((1 << signum_to_be_masked));
    while (1);
  }
  else
  {
    sleep(SLEEP_TIME);
    printf(1, "Sending signal for masked %d signum\n", signum_to_be_masked);
    kill(pid, signum_to_be_masked);
    sleep(100);
    printf(1, "Sending SIGKILL to child\n");
    kill(pid, SIGKILL);
    wait();
    printf(1, "Child exited\n");
    printf(1, "Parent exiting\n");
  }
  printf(1, "=============== sigprocmask pass ===============\n");
  return;
}

void test_pause(){
  printf(1, "\n=============== pause test ===============\n");
  int pid;
  pid = fork();
  if (pid == 0)
  {
    pid = getpid();
    printf(1, "Childs PID: %d\n", pid);
    printf(1, "Called pause() from child process\n");
    pause();
    while (1);
  }
  else
  {
    sleep(SLEEP_TIME);
    printf(1, "Sent SIGCONT signal to child\n");
    kill(pid, SIGCONT);
    sleep(100);
    kill(pid, SIGKILL);
    printf(1, "Sent SIGKILL to child\n");
    wait();
    printf(1, "Child exited\n");
    printf(1, "Parent exiting\n");
  }
  printf(1, "=============== pause pass ===============\n");
  return;
}

testcase setup_testcase(int signum, char *name, char kill_call_required, char override_df_handler, struct sigaction *s){
  testcase t;
  t.signum = signum;
  t.name = name;
  t.kill_call_required = kill_call_required;
  t.override_df_handler = override_df_handler;
  t.s = s;
  return t;
}

struct sigaction get_sigaction_struct(void *addr, int sigmask){
  struct sigaction s;
  s.sa_handler = addr;
  s.sigmask = sigmask;
  return s;
}

int main(int argc, char *argv[])
{
  // kill test
  test_testcase(setup_testcase(SIGKILL, "kill", 0, 0, NULL));

  // sigstop test
  test_testcase(setup_testcase(17, "sigstop", 1, 0, NULL));

  // sigcont test
  test_testcase(setup_testcase(19, "sigcont", 1, 0, NULL));

  struct sigaction s;
  // SIG_DFL custom handler test
  s = get_sigaction_struct(&sigdfl_handler, 0);
  test_testcase(setup_testcase(0, "sigdfl custom handler", 1, 1, &s));

  // SIG_IGN custom handler test
  s = get_sigaction_struct(&sigign_handler, 0);
  test_testcase(setup_testcase(1, "sigign custom handler", 1, 1, &s));

  // SIG_CONT custom handler test
  s = get_sigaction_struct(&sigcont_handler, 0);
  test_testcase(setup_testcase(19, "sigcont custom handler", 1, 1, &s));

  // pause test
  test_pause();

  // sigprocmask test
  test_sigprocmask();

  for(int i = 1; i < argc; i++)
    printf(1, "%s%s", argv[i], i+1 < argc ? " " : "\n");
  exit();
}
