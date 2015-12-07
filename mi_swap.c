#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mi_syscall.h"
#include "include/hardware.h"
#include "hw_config.h"



 int init_swap(const char *path) {
	swap_file = fopen("./swap_file","w+");
	return (swap_file == NULL) ? -1 : 0;
}

 int store_to_swap (int vpage, int ppage) {
	if ((fseek(swap_file, (vpage<<12)), SEEK_SET) == -1)
		return -1;
	if ((fwrite ((ppage<<12)+ physical_mem, 1, PAGE_SIZE, swap_file)) == -1 ) 
		return -1;
	return 0;
}

 int fetch_from__swap (int vpage, int ppage) {
	if ((fseek(swap_file, (vpage<<12)), SEEK_SET) == -1)
		return -1;
	if ((fread ((ppage<<12)+ physical_mem, 1, PAGE_SIZE, swap_file)) == -1 ) 
		return -1;
	return 0;
}
