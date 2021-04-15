#include "compare.h"


int int_cmp(void* a, void* b)
{
    int inta = *((int*)(a)), intb = *((int*)(b));
    if (inta > intb) {
        return 1;
    }
    else if (inta < intb) {
        return -1;
    }
    else {
        return 0;
    }
}

int int_cmp_rev(void* a, void* b)
{
    return int_cmp(a, b) * -1;
}

int dbl_cmp(void* a, void* b)
{
    double floata = *((double*)(a)), floatb = *((double*)(b));
    if (floata > floatb) {
        return 1;
    }
    else if (floata < floatb) {
        return -1;
    }
    else {
        return 0;
    }
}


int str_cmp(void* a, void* b)
{
	char *chara = (char*)a, *charb = (char*)b;
    return strcmp(chara, charb);
}

void dbl_print(void* a)
{
	printf("%f", *(double*)(a));
}

void int_print(void* a)
{
	int inta = *(int*)(a);
	printf("%d", inta);
}

void str_print(void* a)
{
	printf("%s", (char*)(a));
}
