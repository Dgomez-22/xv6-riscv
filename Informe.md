------ Creación de yosoytupadre -------

Primero reinstalé todo xv6 debido a un error que me estaba dando

Nuevamente me dio el error 0x80071772, pero lo solucioné con:

cipher /d /s:"$env:LOCALAPPDATA\Microsoft\WindowsApps"
cipher /d /s:"$env:LOCALAPPDATA\Packages"

Creamos una nueva rama llamada "Tarea_1-Dgomez-Tpoblete" para esta tarea

definimos el SYS_getppid 22 en syscall.h

luego, declaramos en syscall.c el "extern uint64 sys_getppid(void);"

y registramos en la tabla syscalls "[SYS_getppid]     sys_getppid,"

en sysproc.c agregamos getppid(), que retorna el pid del padre

en user.h declaramos int (getppid(void); y en usys.S pusimos SYSCALL(getppid)

Con esto logramos tener que nos devuelve el id del proceso que se usa para llamar al propio "yosoytupadre" junto con su padre, es decir, su ppid

Luego de eso dejamos en standby un tiempo la tarea, y al volver nos daba un error, no se había commiteado algo correctamente

Intentamos muchas cosas, vimos línea por línea y resulta que al hacer make clean se había "reseteado" el usys.S
Al final declaramos lo del getppid en usys.pl, que no se cambiaba al hacer make clean, y con eso se solucionó el problema.

Cree un nuevo token de github y le hice commit al fix.

Luego, creamos el getancestor que seguía una lógica similar, pero con unos pasos extra.
En esencia era lo mismo que el yosoytupadre pero con unas lógicas demás

Al correrlo, nos daba que solo cambiaba el el pid, no el padre ni el ancestor, y nos quedamos bastante tiempo viendo que podría estar mal, pero nos dimos cuenta que no estaba mal
Como tal, el proceso ancestor invocaba al padre, y el padre invocaba el pid original, como hemos visto en clases, por lo que al pensarlo un tiempo tenía sentido que el pid original fuera
el único que cambiara, debido a que el proceso padre no mutaba; solo llamaba a distintos procesos hijos (por eso cambiaban), por lo mismo, como el padre llamaba a nuevos pids y nada más,
nos daba que el ancestor y el ppid no mutaban.

Cada parte de la tarea se corre de manera independiente; al hacer el make qemu, vamos a poder usar tanto yosoytupadre (para la parte 1) como getancestor (para la parte 2).
En caso de no correr intentar usar make clean y luego make qemu, no hacer make clean luego de cada "edición" nos causaba problemas, pero bastante simple de resolver (con make clean)
