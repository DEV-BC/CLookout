#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void strings_demo(void)
{
    char name[] = "Sentinel";
    printf("String: %s\n", name);
    printf("Length: %zu\n", strlen(name));
    printf("Size: %zu\n", sizeof(name));
    printf("\n");

    for (int i = 0; i <= (int)strlen(name); i++)
    {
        printf("name[%d] = %3d  '%c'\n", i, name[i], name[i] ? name[i] : '0');
    }

    char modifiable[] = "hello";
    char *literal = "Hello";

    modifiable[0] = 'H';
    printf("Modified: %s\n", modifiable);

    printf("Literal: %s\n", literal);
    // literal[0] = 'H';

    /*
     * safe string copying
     */
    char src[] = "Sentinel-C";
    char dst[5];

    /*strncpy - bounded copy, manual null termination required*/
    strncpy(dst, src, sizeof(dst) - 1);
    dst[sizeof(dst) - 1] = '\0';
    printf("strncpy: %s\n", dst);

    /* snprintf — always safe, always null-terminated */
    char sbuf[11];
    snprintf(sbuf, sizeof(sbuf), "%s", src);
    printf("snprintf: %s\n", sbuf);

    char *x = "hello";
    char *y = "hello";
    char *z = "world";

    printf("strcmp(x, y) = %d\n", strcmp(x, y));  /* 0 = equal      */
    printf("strcmp(x, z) = %d\n", strcmp(x, z));  /* <0 = x < z     */
    printf("strcmp(z, x) = %d\n", strcmp(z, x));  /* >0 = z > x     */

}
