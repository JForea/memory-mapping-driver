#ifndef MEMORY_HELPERS_H
#define MEMORY_HELPERS_H

int mem_patch(unsigned long addr);
int mem_allocate(unsigned long addr, unsigned long len);
int mem_free(unsigned long addr, unsigned long len);

#endif