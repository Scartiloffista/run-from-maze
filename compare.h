#ifndef lasd_compare_h
#define lasd_compare_h

#include <string.h>
#include <stdio.h>

typedef int(*COMPAREFN)(void*,void*);
typedef void(*PRINTFN)(void*);
int int_cmp(void* a, void* b);
int dbl_cmp(void* a, void* b);
int str_cmp(void* a, void* b);
void dbl_print(void*);
void int_print(void*);
int int_cmp_rev(void* a, void* b);
void str_print(void*);


#endif
