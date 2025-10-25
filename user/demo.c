#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Esta demo crea 10 procesos hijos
// Cada proceso hijo se asigna una cantidad distinta de tickets usando el settickets()
// Luego hace un loop grande para consumir cpu
// Al final imprime su PID y cuántos tickets le dimos
// El padre espera a que todos terminen

int
main(void)
{
  int i;

  for (i = 0; i < 10; i++) {
    int pid = fork();
    if (pid < 0) {
      // Por si hay error al forkear
      printf("fork fallo\n");
      exit(1);
    }

    if (pid == 0) {
      // ----- PROCESO HIJO -----
      int mytickets = 50 * (i + 1);  // 50, 100, 150, ..., 500
      settickets(mytickets);

      // trabajo artificial para gastar CPU
      volatile unsigned long x = 0;
      for (unsigned long k = 0; k < 100000000UL; k++) {
        x = x + k;
      }

      // Mensaje final del proceso hijo
      printf("[Hijo PID %d] Terminé con %d tickets\n", getpid(), mytickets);

      exit(0); // Proceso hijo sale
    }

    // ----- PROCESO PADRE -----
    // El proceso padre sigue el for y crea el siguiente proceso hijo
  }

  // Proceso Padre: esperar a que terminen todos los hijos
  for (i = 0; i < 10; i++) {
    wait(0);
  }

  printf("Todos los hijos terminaron.\n");
  exit(0);
}
