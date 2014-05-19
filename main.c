#include "sort.h"
#include <stdio.h>

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
    (void) argc;
    (void) argv;

    int arr[] = {3, 1, 4, 2, 5, 7, 10, 9, 8, 6, 16, 14, 12, 11, 13, 15};
    sort((void *)arr, 16, sizeof(int), cmp_int);
    print_array(arr, 16);
    return 0;
}
