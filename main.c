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

    int arr[1358], i;
    for (i=0; i<1024; i++)
        arr[i] = 1024-i;
//    print_array(arr, 1024);
    sort((void *)arr, 1024, sizeof(int), cmp_int);
    print_array(arr, 1024);
    return 0;
}
