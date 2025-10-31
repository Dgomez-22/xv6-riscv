### 1. La implementación del Lottery Scheduler ###

Primero, abrimos Ubuntu y creamos la nueva rama:

    git checkout -b Tarea_2-Dgomez-Tpoblete

Luego, entramos a proc.h y agregamos dos nuevos campos dentro de struct proc:

    int tickets;      // cantidad de tickets asignados al proceso
    int cpu_slices;   // cuántas veces el proceso fue elegido por la CPU

Después fuimos a proc.c y modificamos lo siguiente:

1- En allocproc() inicializamos los nuevos valores:

    tickets = 100 (el valor base por defecto)
    cpu_slices = 0

2- En scheduler() reemplazamos el scheduler original por nuestro lottery scheduler:

    1- Sumamos los tickets de todos los procesos RUNNABLE

    2- Si el total es 0, pasamos al siguiente ciclo

    3- Generamos un número aleatorio dentro del rango 1 hasta el total de tickets

    4- Recorremos los procesos hasta que acumulando tickets encontremos al ganador

    5- Ponemos ese proceso en RUNNING y sumamos cpu_slices


Luego registramos la nueva syscall settickets():

    1- En syscall.h le asignamos un número identificador (SYS_settickets)

    2- En syscall.c conectamos ese número con la función sys_settickets

    3- En sysproc.c implementamos sys_settickets(int n) para modificar los tickets del proceso actual

    4- En user.h declaramos int settickets(int);

    5- En lugar de modificar usys.S manualmente, agregamos entry("settickets") en usys.pl, 
    para que el stub se genere automáticamente al compilar

Con el sistema ya funcionando, creamos demo.c en la carpeta user/.

Este archivo crea varios procesos hijos con distintos tickets, ejecutan carga de CPU, y al terminar imprimen: PID, tickets asignados y cpu_slices.

En las pruebas se confirmó que los procesos con más tickets suelen terminar antes (porque tienen más probabilidad de ganar la lotería), pero al ser probabilístico, no es 100% determinista.

### 2. Problemas y soluciones ###

1- Problema con la generación del stub de la syscall (settickets)

    Problema: En la tarea anterior intentamos modificar usys.S directamente, pero make clean lo regeneraba y borraba los cambios.

    Solución: Esta vez agregamos la entrada en usys.pl, lo que hace que usys.S se reconstruya con la syscall incluida automáticamente.

2- Choque de impresiones en demo.c

    Problema: La salida de varios hijos se mezclaba en consola al terminar casi al mismo tiempo.

    Solución: Ajustamos una pequeña pausa, con delay y sleep

    Problema 2: Las implementaciones con delay y sleep estaban mal hechas

    Solución 2: Quitamos delay y sleep, y aumentamos la carga de la cpu, para que se demore más y así se no se sobrelapen los mensajes

### 3. Problemas del Lottery Scheduler ###

    1- No garantiza justicia determinista → procesos con pocos tickets pueden ganar varias veces seguidas por azar.

    2- No sirve para tiempo real → no asegura tiempos máximos de respuesta, solo probabilidades.

    3- Puede provocar starvation indirecto → si un proceso tiene muy pocos tickets y hay procesos con muchos, 
    puede tardar demasiado en ejecutarse.

    4- No controla prioridades exactas → solo probabilísticas, no “este proceso debe tener 40% de CPU fijo”.

    5- Escala mal con muchos procesos → el scheduler debe sumar tickets y recorrer procesos en cada tick.

    6- No considera otros factores importantes del sistema (I/O, memoria, CPU usage acumulado, afinidad, etc.).

### 4. Estado final de la tarea ###

    1- El Scheduler reemplazado correctamente
    
    2- El Syscall quedó implementado y funcional
    
    3- La Demo ha sido probada con distintos tickets
    
    4- Confirmamos el comportamiento probabilístico esperado
    
    5- Verificamos consistencias y redundancias básicas para evitar errores
