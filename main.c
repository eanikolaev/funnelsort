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

    size_t nmemb=atoi(argv[1]), size=atoi(argv[2]);
    int *arr = (int*)malloc(nmemb*size);
    size_t i, n=nmemb*size/sizeof(int);

    if (argc == 4) {
        FILE *fp = fopen(argv[3], "r");
        for (i=0; i<n; i++) {
            fscanf(fp, "%d", &arr[i]);
        }
        fclose(fp);
    }
    else {
        for (i=0; i<n; i++)
            arr[i] = rand() % nmemb;
    }

    printf("BEFORE:\n");
    print_array(arr, n);
    printf("\n");

    sort((void *)arr, nmemb, size, cmp_int);

    printf("AFTER:\n");
    print_array(arr, n);
    printf("\n");

    return 0;
}
