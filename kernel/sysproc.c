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

//TODO:
uint64
sys_sigalarm(void)
{
  int ticks;
  uint64 handler;
  argint(0, &ticks);
  argaddr(1, &handler);

  struct proc *p = myproc();
  p->ticks=ticks;
  p->handler = handler;

  p->ticks_count = 0;

  return 0;
}

void restore()
{
  struct proc *p = myproc();
  p->trapframe->ra = p->t_ra;
  p->trapframe->sp = p->t_sp;
  p->trapframe->gp = p->t_gp;
  p->trapframe->tp = p->t_tp;
  p->trapframe->t0 = p->t_t0;
  p->trapframe->t1 = p->t_t1;
  p->trapframe->t2 = p->t_t2;
  p->trapframe->s0 = p->t_s0;
  p->trapframe->s1 = p->t_s1;
  p->trapframe->a0 = p->t_a0;
  p->trapframe->a1 = p->t_a1;
  p->trapframe->a2 = p->t_a2;
  p->trapframe->a3 = p->t_a3;
  p->trapframe->a4 = p->t_a4;
  p->trapframe->a5 = p->t_a5;
  p->trapframe->a6 = p->t_a6;
  p->trapframe->a7 = p->t_a7;
  p->trapframe->s2 = p->t_s2;
  p->trapframe->s3 = p->t_s3;
  p->trapframe->s4 = p->t_s4;
  p->trapframe->s5 = p->t_s5;
  p->trapframe->s6 = p->t_s6;
  p->trapframe->s7 = p->t_s7;
  p->trapframe->s8 = p->t_s8;
  p->trapframe->s9 = p->t_s9;
  p->trapframe->s10 = p->t_s10;
  p->trapframe->s11 = p->t_s11;
  p->trapframe->t3 = p->t_t3;
  p->trapframe->t4 = p->t_t4;
  p->trapframe->t5 = p->t_t5;
  p->trapframe->t6 = p->t_t6;
}

uint64
sys_sigreturn(void)
{
  struct proc *p = myproc();
  p->trapframe->epc = p->res_epc;
  p->handler_exec = 0;
  restore();
  int ret = p->trapframe->a0;
  return ret;
}
