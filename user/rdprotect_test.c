#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  // Obtenemos la dirección actual del heap
  char *addr = sbrk(0);

  // Reservamos exactamente una página (4096 bytes)
  if(sbrk(4096) == (void*)-1){
    printf("Error al pedir memoria con sbrk()\n");
    exit(1);
  }

  // Escribimos algo antes de proteger la página
  addr[0] = 'Z';
  printf("Antes de proteger -> addr[0] = %c\n", addr[0]);

  // Activamos la protección contra lectura
  if(mrdprotect(addr, 1) < 0){
    printf("mrdprotect no funcionó\n");
    exit(1);
  }
  printf("Protección activada en la página que parte en %p\n", addr);

  // Ahora hacemos un intento de lectura: debería provocar un page fault
  // y se debería matar el proceso

  printf("A punto de leer addr[0] (debería crashear ahora)...\n");

  char c = addr[0];  // load → page fault
  printf("Valor leido: %c (Este valor no debería verse NUNCA)\n", c);

  // Si por algo llegamos aqui, revertimos la protección
  if(munrdprotect(addr, 1) < 0){
    printf("munrdprotect falló\n");
    exit(1);
  }

  printf("Proteccion revertida correctamente. (No deberíamos ver este print)\n");
  exit(0);
}

