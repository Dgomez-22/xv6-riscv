Implementación de mrdprotect y munrdprotect en xv6
1. Implementación de las syscalls de protección de lectura

Primero, creamos la nueva rama:

    git checkout -b Tarea_3-Dgomez-Tpoblete

Luego agregamos las nuevas syscalls:

    SYS_mrdprotect  = 25
    SYS_munrdprotect = 26

Esto se hizo en kernel/syscall.h.

Después conectamos las syscalls en el kernel:

En kernel/syscall.c agregamos:

extern uint64 sys_mrdprotect(void);

extern uint64 sys_munrdprotect(void);

Y las entradas en el arreglo syscalls[].

En user/usys.pl agregamos:

    entry("mrdprotect");
    entry("munrdprotect");

Esto genera automáticamente los stubs en usys.S cuando se compila.

En user/user.h declaramos:

    int mrdprotect(void *addr, int len);
    int munrdprotect(void *addr, int len);

2. Implementación en sysproc.c

Creamos dos funciones del lado kernel:

    sys_mrdprotect()

    sys_munrdprotect()

Ambas reciben addr y len desde user space usando argaddr() y argint().

Luego implementamos la función interna:

    do_rdprotect(pagetable_t pagetable, uint64 addr, int len, int enable_read)

que hace el trabajo real:

Alinea las direcciones a página.

Recorre todas las páginas del rango.

Para cada PTE:

revisa PTE_V

revisa PTE_U

aplica:

PTE_R = 0 si estamos protegiendo

PTE_R = 1 si estamos desprotegiendo

Llama a sfence_vma() para invalidar la TLB.

Finalmente, ambas syscalls retornan:

0 si todo salió bien,

-1 si hubo cualquier error (dirección no alineada, len inválido, PTE no válido, etc.).

3. Programa de prueba (rdprotect_test.c)

Creamos user/rdprotect_test.c para verificar el funcionamiento.

El test:

Reserva memoria con sbrk(PGSIZE).

Escribe un valor ('Z') en esa página.

Llama a mrdprotect.

Intenta leer nuevamente el valor.

En este punto se produce un page fault:

    usertrap(): unexpected scause 13 (Load Page Fault)

Esto confirma que la syscall sí removió PTE_R.

Luego, si se llama munrdprotect, la página vuelve a ser legible.

4. Problemas y soluciones

1. Error “unknown syscall 25”
Causa: faltaba conectar las syscalls en syscall.c.
Solución: agregar ambas entradas en el arreglo syscalls[].

2. Error de compilación en sysproc.c (“unused function”)
Causa: definimos do_rdprotect pero no lo llamamos.
Solución: llamar la función desde sys_mrdprotect y sys_munrdprotect.

3. Page fault al leer después de proteger
Esto no es un error: es el comportamiento esperado.
En RISC-V, una página con escritura pero sin lectura es inválida por hardware.

4. Interferencia con el Lottery Scheduler
Los prints del scheduler siguen apareciendo porque están mezclados en la consola,
pero no afectan la syscall ni el test.

6. Output esperado

Durante el test, el kernel debe mostrar un page fault al intentar leer una página protegida:

    usertrap(): unexpected scause 0xd
    stval = <direccion>

Y el test debe imprimir la secuencia:

    Escritura inicial ok: addr[0] = Z
    mrdprotect aplicado...
    A punto de leer addr[0]...
    <page fault aquí>

