#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int id;
    int online;
} Device;

void memory_demo(void)
{
    int a = 42;
    int b = 99;
    char c = 'X';

    printf("Value of a: %d\n", a);
    printf("Value of b: %d\n", b);
    printf("Value of c: %c\n", c);
    printf("\n");
    printf("Address of a: %p\n", (void*)&a);
    printf("Address of b: %p\n", (void*)&b);
    printf("Address of c: %p\n", (void*)&c);

    /*
     * Pointers
     */
    int* p = &a;
    printf("\np points to: %p\n", (void*)p);
    printf("Value at p:   %d\n",   *p);
    *p = 100;
    printf("a is now:     %d\n",   a);

    /*Heap Allocation*/
    Device* dev = malloc(sizeof(Device));
    if (dev == NULL)
    {
        printf("malloc failed!\n");
        return;
    }

    dev->id = 1;
    dev->online = 1;

    printf("\nDevice id:     %d\n",  dev->id);
    printf("Device online: %d\n",   dev->online);
    printf("Device addr:   %p\n",   (void *)dev);

    free(dev);

    printf("Device freed!\n");



}

