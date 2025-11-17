#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  // Dirección actual del heap
  char *addr = sbrk(0);

  // Reservamos exactamente una página (4096 bytes)
  if(sbrk(4096) == (void*)-1){
    printf("sbrk fallo\n");
    exit(1);
  }

  // Escribimos algo antes de proteger
  addr[0] = 'Z';
  printf("Escritura inicial ok: addr[0] = %c\n", addr[0]);

  // Activamos la protección contra lectura
  if(mrdprotect(addr, 1) < 0){
    printf("mrdprotect fallo\n");
    exit(1);
  }
  printf("mrdprotect aplicado sobre una pagina a partir de %p\n", addr);

  // ⚠️ NO volvemos a escribir aquí, porque en RISC-V
  // una pagina con W=1 y R=0 se trata como invalida.

  // Intento de lectura: deberia provocar un page fault y matar el proceso
  printf("A punto de leer addr[0] (deberia crashear ahora)...\n");
  char c = addr[0];  // load → page fault
  printf("Valor leido: %c (ESTO NO DEBERIA IMPRIMIRSE)\n", c);

  // Si por alguna razon llegamos aqui, revertimos la proteccion
  if(munrdprotect(addr, 1) < 0){
    printf("munrdprotect fallo\n");
    exit(1);
  }

  printf("Proteccion revertida correctamente.\n");
  exit(0);
}

