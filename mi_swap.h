#ifndef MI_SWAP_H
#define MI_SWAP_H

 FILE *swap_file;

 int init_swap(const char *path);

 int store_to_swap (int vpage, int ppage) ;

 int fetch_from__swap (int vpage, int ppage);

#endif


