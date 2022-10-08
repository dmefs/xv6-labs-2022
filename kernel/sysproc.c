#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
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
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
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
  backtrace();
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
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

uint64
sys_sigalarm(void)
{
  int ticks;
  uint64 handler;

  // Get ticks & handler
  argint(0, &ticks);
  argaddr(1, &handler);


  struct proc *proc = myproc();

  if (ticks == 0 && handler == 0) {
    proc->alarmhandler = 0;
    proc->alarmticks = 0;
    proc->alarmtickspassed = 0;
    return 0;
  }
  // store data to proc
  proc->alarmticks = ticks;
  proc->alarmhandler = handler;
  char *p = kalloc();
  if (!p)
    panic("sigalarm");
  proc->alarmframe = (struct trapframe *)(uint64)p;
  // save previous proc state
  *proc->alarmframe = *proc->trapframe;

  return 0;
}

uint64
sys_sigreturn(void)
{
  struct proc *proc = myproc();
  *proc->trapframe = *proc->alarmframe;
  proc->alarmtickspassed = 0;

  if (proc->alarmticks == 0) {
    if (proc->alarmframe)
      kfree(proc->alarmframe);
    proc->alarmframe = 0;
  }
  return proc->trapframe->a0;
}