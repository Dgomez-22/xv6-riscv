#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("pid=%d, ppid=%d\n", getpid(), getppid());
  exit(0);
}
