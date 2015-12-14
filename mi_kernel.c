#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mi_syscall.h"
#include "include/hardware.h"
#include "hw_config.h"
#include "mi_swap.h"

static int current_process;
static unsigned int mapped_vpage;

struct tlb_entry_s {
  unsigned int rfu:8;
  unsigned int vpage:12;
  unsigned int ppage:8;
  unsigned int access_w:1;
  unsigned int access_r:1;
  unsigned int access_x:1;
  unsigned int util:1;
};

union tlb_entry_u {
  struct tlb_entry_s entry;
  int asInt;
};

static int vpage_of_vaddr(unsigned int vaddr) {
  if ((vaddr & 0xFF000000) != (int) virtual_memory) return -1;
  return (vaddr >> 12)& 0xFFF;
}

static int ppage_of_vaddr(int p,unsigned int vaddr) {
  int page = vpage_of_vaddr(vaddr);
  if (page < 0) return -1;
  return (page < N/2) ? p * (N/2) + page + 1 : -1;
}

// static void mmuhandler() {
//   union tlb_entry_u u;
//   struct tlb_entry_s e;
//   unsigned int vaddr = _in(MMU_FAULT_ADDR);
//   int ppage = ppage_of_vaddr(current_process, vaddr);
//   if (ppage < 0) exit(EXIT_FAILURE);
//   e.ppage = ppage;
//   e.vpage = vpage_of_vaddr(vaddr);
//   e.access_r = 1;
//   e.access_w = 1;
//   e.access_x = 1;
//   e.util = 1;
//   u.entry = e;
//   _out(TLB_ADD_ENTRY,u.asInt);
// }

static void mmuhandler() {
  struct tlb_entry_s e;
  union tlb_entry_u u;
 // if (kernel_mode) {
 // fprintf(stderr, "kernel panic"); 
 //   exit(-1);
 // }
  unsigned int vaddr=_in(MMU_FAULT_ADDR);
  if((vaddr < (int) virtual_memory) || ((vaddr) >= (int) virtual_memory + VM_SIZE)) {
    fprintf(stderr, "seg fault");
    exit(-1);
  }
  store_to_swap(mapped_vpage,1);
  e.vpage= mapped_vpage;
  e.ppage = 1;
  u.entry = e;
  _out(TLB_DEL_ENTRY, u.asInt);
  fetch_from_swap(vpage_of_vaddr(vaddr),1);
  e.vpage = mapped_vpage;
  e.access_r= e.access_w = e.access_x = 1;
  u.entry = e;
  _out(TLB_ADD_ENTRY,u.asInt);
}

static int 
sum(void *ptr) 
{
  int i;
  int sum = 0;
  
  for(i = 0; i < PAGE_SIZE * N/2 ; i++)
      sum += ((char*)ptr)[i];
  return sum;
}

static void 
switch_to_process0(void) 
{
  current_process = 0;
  _out(MMU_CMD, MMU_RESET);
}

static void
switch_to_process1(void) 
{
  current_process = 1;
  _out(MMU_CMD, MMU_RESET);
}

static void
emptyIT()
{
    return;
}

int 
main(int argc, char **argv) 
{
  void *ptr;
  int res,i;

  init_hardware(HARDWARE_INI);
  /* Interrupt handlers */
  for(i=0; i<16; i++)
    IRQVECTOR[i] = emptyIT;
  IRQVECTOR[MMU_IRQ] = mmuhandler;
  IRQVECTOR[16] = switch_to_process0;
  IRQVECTOR[17] = switch_to_process1;
  _mask(0x1001);

  ptr = virtual_memory;

  _int(16);
  memset(ptr, 1, PAGE_SIZE * N/2);
  _int(17);
  memset(ptr, 3, PAGE_SIZE * N/2);

  _int(16);
  res = sum(ptr);
  printf("Resultat du processus 0 : %d\n",res);
  _int(17);
  res = sum(ptr);
  printf("Resultat processus 1 : %d\n",res);
}
