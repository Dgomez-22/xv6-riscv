#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("getancestor(0)=%d\n", getancestor(0));
  printf("getancestor(1)=%d\n", getancestor(1));
  printf("getancestor(2)=%d\n", getancestor(2));
  exit(0);
}
