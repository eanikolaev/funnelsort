#include "sort.h"
#include <stdio.h>
#include <stdlib.h>

int cmp_int(const void * x, const void * y)
{
    return (*(int *)x - *(int *)y);
}

void print_array(int *arr, int nmemb)
{
    int i;
    for (i=0; i<nmemb; i++) {
        printf("%d", arr[i]);
        printf("%c", ' ');  
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3 && argc != 4) {
        printf("usage: %s nmemb size [filename]\n", argv[0] );
        return -1;
    }

/*
	TODO:
    if (argc == 4) {
        read file
    }
*/     
    size_t nmemb=atoi(argv[1]), size=atoi(argv[2]);
    int *arr = (int*)malloc(nmemb*size);
    size_t i, n=nmemb*size/sizeof(int);
    for (i=0; i<n; i++)
        arr[i] = rand() % nmemb;

    printf("BEFORE sort:\n");
    print_array(arr, n);
    sort((void *)arr, nmemb, size, cmp_int);

    printf("\nAFTER sort:\n");
    print_array(arr, n);
    return 0;
}
