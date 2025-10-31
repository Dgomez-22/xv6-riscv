Primero, abrimos ubuntu y creamos la nueva rama "Tarea_2-Dgomez-Tpoblete" con:

  > git checkout -b Tarea_2-Dgomez-Tpoblete

Luego nos adentramos en las carpetas hasta llegar a proc.h, donde le agregamos esto:

  > int tickets;       (la cantidad de tickets asignados al proceso para el lottery scheduling)
  > int cpu_slices;    (el contador de cuantas veces el proceso fue elegido por la cpu)

Guardamos y salimos, luego nos metimos a proc.c y modificamos lo siguiente:

  1) En alloproc() asignamos el valor inicial para los tickets (100) y el numero inicial de slices (0)
  2) En scheduler() borramos el que había antes y agregamos el lottery scheduler, que funciona de la siguiente manera:

  > Calculamos el número total de tickets (sumamos los tickets de los runnables)
  > Si el total de tickets es 0, saltamos al ciclo que viene
  > Generamos un número (pseudo) aleatorio ganador , entre 1 ticket y el total
  > Buscamos qué proceso ganó y lo ejecutamos
  > Le sumamos cpu_slices al proceso que ganó (antes de correr dicho proceso)

Luego nos fuimos a syscall.h donde le asignamos el número "identificador" por así decir
Después nos fuimos a syscall.c y "linkeamos" el num identificador con la función sys_settickets()
Con esto ya listo, fuimos a user.h para que los programas en el modo usuario reconozcan que settickets() es válida y llamable
Para poder hacer que se "cree" automáticamente, en usys.pl agregamos el settickets como un "entry", de manera en que al crearse usys.S el settickets también aparece dentro

Esto mismo del usys ".S" y ".pl" era lo que nos complicó la tarea pasada, por lo que esta vez no tuvimos problemas
Ya sabíamos que al hacer el make clean el ".S" se borraba y era re-creado por el ".pl"

Por último creamos el demo.c, en la carpeta user, para que creara varios procesos hijos con cantidades distintas de tickets usando el settickets()
Cada proceso hijo ejecuta su "carga" que le pusimos como parámetro y muestra como resultado:

  > Su "prioridad" (se imprimen primero los que "ganaron" antes)
  > El número de tickets que tuvo al inicio
  > El número de cpu_slices

En general, nos dio que aquel proceso que tenía más tickets era el que salía antes (recordar, es probabilístico, por lo que hay casos en que aquellos con menos tickets se imprimen primero)
El demo.c está mejor explicado dentro del propio código, e intentamos hacer que se pudiesen cambiar los parámetros fácilmente, algo más tipo modular por así decir
