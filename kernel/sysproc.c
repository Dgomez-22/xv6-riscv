#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

// Helper para modificar permisos de lectura.
// enable_read = 0 → quitar lectura (mrdprotect)
// enable_read = 1 → permitir lectura (munrdprotect)
static int
do_rdprotect(uint64 addr, int len, int enable_read)
{
  struct proc *p = myproc();
  pagetable_t pt = p->pagetable;

  // Validaciones básicas
  if(len <= 0)
    return -1;

  if(addr % PGSIZE != 0)
    return -1;

  for(int i = 0; i < len; i++){
    uint64 va = addr + (uint64)i * PGSIZE;

    if(va >= MAXVA)
      return -1;

    pte_t *pte = walk(pt, va, 0);
    if(pte == 0)
      return -1;

    if((*pte & PTE_V) == 0)
      return -1;

    if((*pte & PTE_U) == 0)
      return -1;
  }

  // Modificar permisos
  for(int i = 0; i < len; i++){
    uint64 va = addr + (uint64)i * PGSIZE;
    pte_t *pte = walk(pt, va, 0);

    if(enable_read)
      *pte |= PTE_R;
    else
      *pte &= ~PTE_R;
  }

  // Limpiar TLB
  sfence_vma();

  return 0;
}

uint64
sys_mrdprotect(void)
{
  uint64 addr;
  int len;

  // En tu xv6, argaddr y argint son void, no devuelven int
  argaddr(0, &addr);
  argint(1, &len);

  return do_rdprotect(addr, len, 0);  // 0 = quitar lectura
}

uint64
sys_munrdprotect(void)
{
  uint64 addr;
  int len;

  argaddr(0, &addr);
  argint(1, &len);

  return do_rdprotect(addr, len, 1);  // 1 = restaurar lectura
}

extern struct proc* myproc(void); // Es una función que llama de afuera, devuelve el proceso actual

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_getppid(void)
{
  struct proc *p = myproc();
  if(p->parent)
    return p->parent->pid;
  return -1;
}

uint64
sys_getancestor(void)
{
  int n;
  argint(0, &n);
  if (n < 0)
    return -1;

  struct proc *cur = myproc();
  while (n > 0 && cur != 0) {
    cur = cur->parent;
    n--;
  }

  if (cur == 0)
    return -1;

  return cur->pid;
}

uint64
sys_settickets(void)
{
  int n;
  argint(0, &n);

  if(n < 1){
    n = 1; 	// Garantizamos al menos un ticket (evitar errores)
  }

  struct proc *p = myproc(); // Obtenemos el proceso que está ejecutando esta call
  acquire(&p->lock);	// Bloqueamos el proceso para editarlo
  p->tickets = n;	// Asignamos el nuevo núm de tickets
  release(&p->lock);	// Liberamos el lock

  return 0;
}
