Implementación de mrdprotect y munrdprotect en xv6

### 1. Implementación de las syscalls de protección de lectura ###

Primero, creamos la nueva rama:

    git checkout -b Tarea_3-Dgomez-Tpoblete

Luego agregamos las nuevas syscalls:

    SYS_mrdprotect  = 25
    SYS_munrdprotect = 26

Esto se hizo en kernel/syscall.h

Después conectamos las syscalls en el kernel:
    
    I) En kernel/syscall.c agregamos:

            extern uint64 sys_mrdprotect(void);

            extern uint64 sys_munrdprotect(void);

    Y las entradas en el arreglo syscalls[]

    II) En user/usys.pl agregamos:

        entry("mrdprotect");
        entry("munrdprotect");

    Esto genera automáticamente los stubs en usys.S cuando se compila

    III) En user/user.h declaramos:

        int mrdprotect(void *addr, int len);
        int munrdprotect(void *addr, int len);

### 2. Implementación en sysproc.c ###

Creamos dos funciones del lado kernel:

    sys_mrdprotect()

    sys_munrdprotect()

Ambas reciben addr y len desde user space usando argaddr() y argint()

Luego implementamos la función interna:

    do_rdprotect(pagetable_t pagetable, uint64 addr, int len, int enable_read)

que hace el trabajo real:

    I) Alinea las direcciones a página

    II) Recorre todas las páginas del rango

    III) Para cada PTE:

        - revisa PTE_V

        - revisa PTE_U

        - aplica:

            PTE_R = 0 si estamos protegiendo

            PTE_R = 1 si estamos desprotegiendo

    IV) Llama a sfence_vma() para invalidar la TLB

    Finalmente, ambas syscalls retornan:

        0 si todo salió bien,

        -1 si hubo cualquier error (dirección no alineada, len inválido, PTE no válido, etc.)

### 3. Programa de prueba (rdprotect_test.c) ###

Creamos user/rdprotect_test.c para verificar el funcionamiento

El test:

    I) Reserva memoria con sbrk(PGSIZE)

    II) Escribe un valor ('Z') en esa página

    III) Llama a mrdprotect

    IV) Intenta leer nuevamente el valor

En este punto se produce un page fault:

    usertrap(): unexpected scause 13 (Load Page Fault)

Esto confirma que la syscall sí removió PTE_R

Luego, si se llama munrdprotect, la página vuelve a ser legible

### 4. Problemas y soluciones ###

1) Errores de compilación por argaddr y argint

       Problema: Apareció el error:
        void value not ignored as it ought to be
        al usar:

            if (argaddr(0, &addr) < 0)
            if (argint(1, &len) < 0)

        Causa: En esta versión de xv6, argaddr y argint son funciones void (no retornan int), por lo que no se pueden comparar con < 0

        Solución:
        Cambiamos a:

            argaddr(0, &addr);
            argint(1, &len);

2) Error de compilación en sysproc.c “unused function”

        Problema: El compilador marcó:
        do_rdprotect defined but not used

        Causa: La función helper do_rdprotect() estaba escrita pero las syscalls sys_mrdprotect y sys_munrdprotect todavía no la llamaban correctamente dentro de sysproc.c

        Solución:
        Movimos do_rdprotect a la parte superior del archivo (después de los includes)
   
        Con esto desapareció el warning y se centralizó la lógica en una sola función


3) Interferencia visual con el Lottery Scheduler

        Problema: Los mensajes del lottery scheduler (por ejemplo, PID X saliendo: tickets=) aparecían mezclados con los mensajes de rdprotect_test y con los usertrap(), lo que hacía un poco confusa la lectura del output

        Causa: La Tarea 2 ya había reemplazado el scheduler por lottery, con prints al terminar los procesos. La T3 se probó sobre la misma base, así que ambos outputs se mezclan en la consola

        Solución:
        Quitamos los printf en syscall, proc, etc

        Con esto pudimos hacer que el usertrap() apareciera sin ser interrumpido

### 5. Estado final de la tarea ###

        Ambas syscalls implementadas completamente

        Se recorren los PTEs y se modifican los bits correctamente

        La validación de direcciones y permisos está implementada

        Se usa sfence_vma() para actualizar TLB

        El test en user space demuestra el funcionamiento correcto

### 6. Output esperado ###

Durante el test, el kernel debe mostrar un page fault al intentar leer una página protegida:

    usertrap(): unexpected scause 0xd
    stval = <direccion>

Y el test debe imprimir la secuencia:

    Escritura inicial ok: addr[0] = Z
    mrdprotect aplicado
    A punto de leer addr[0]
    <page fault aquí>

