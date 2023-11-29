#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

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

/*设置进程的跟踪掩码。它从用户空间获取一个整数参数（掩码），将该掩码存储在当前进程的mask字段中，
  用于指定要跟踪的系统调用。函数返回0表示成功执行。*/
uint64
sys_trace(void)
{ 
  int mask;
  argint(0, &mask);
  myproc()->mask = mask;
  return 0;
}

// 参考：copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len)
uint64
sys_sysinfo(void)
{
  struct sysinfo info;
  struct proc *p = myproc();
  uint64 vir_addr;

  //从第一个参数（a0），把目标虚拟地址读取出来存储在addr中
  argaddr(0, &vir_addr);

  info.freemem = free_mem();
  info.nproc = runpro();

  if (copyout(p->pagetable, vir_addr, (char *)&info, sizeof(info)) < 0)
    return -1;
  return 0;
}