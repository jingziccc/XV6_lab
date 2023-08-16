#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
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
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
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

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
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

/*

proc Pre-process state


*/

uint64
sys_trace(void)
{
  int mask;
  argint(0, &mask);//获取第一个参数，赋值给，mask
  myproc()->mask = mask;//将mask赋值给myproc()->mask
  return 0;
}

uint64
sys_sysinfo(void)
{
  // user pointer to struct sysinfo
  uint64 si_addr;
  argaddr(0, &si_addr);
  int nproc;
  int freemem;

  nproc = get_proc();;
  freemem = get_free_mem();

  struct sysinfo {
    uint64 freemem;
    uint64 nproc;
  }s;

  s.freemem = freemem;
  s.nproc = nproc;

  struct proc *p = myproc();
  if (copyout(p->pagetable, si_addr, (char *)&s, sizeof(s)) < 0)
    return -1;

  return 0;
}