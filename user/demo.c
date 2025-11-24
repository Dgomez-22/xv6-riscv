#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// =========================
// Settings del demo.c
// =========================

// Cantidad de procesos hijos
#define NCHILD 15

// Cantidad de bloques de trabajo que hará cada hijo (más grande -> más tiempo)
#define OUTER_LOOPS 10

// Carga de cada bloque (mientras más grande, más cpu consume cada bloque)
#define INNER_WORK 50000000UL

// ============================================================================
// Para hacerlo más rápido o más lento, cambiar el OUTER_LOOPS y el INNER_WORK
// ============================================================================

int sleep(int ticks);

int
main(void)
{
  int i;

  for (i = 0; i < NCHILD; i++) {
    int pid = fork();
    if (pid < 0) {
      exit(1);
    }

    if (pid == 0) {
      // =====
      // HIJO
      // =====

      // Asignar tickets distintos, múltiplos de 50
      int mytickets = 50 * (i + 1);
      settickets(mytickets);

      // En cuanto al trabajo: usamos algunos bloques medianos en vez de un bloque gigante, para poder refinar los bloques de mejor manera
      // Esto hace que el timer del kernel interrumpa algunas veces, así el scheduler va eligiendo repetidamente a cada proceso.
      for (int block = 0; block < OUTER_LOOPS; block++) {
        volatile unsigned long acc = 0;
        for (unsigned long k = 0; k < INNER_WORK; k++) {
          acc += k;
        }
      }
      exit(0);
    }

    // ======
    // PADRE
    // ======
    // El proceso padre sigue creando más hijos
  }

  // El proceso padre espera a todos los hijos
  for (i = 0; i < NCHILD; i++) {
    wait(0);
  }

  exit(0);
}
