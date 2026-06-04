#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "device.h"

Device* device_create(int id, const char* name)
{
    Device* dev = malloc(sizeof(Device));
    if (dev == NULL)
    {
        return NULL;
    }
    dev->id = id;
    dev->online = 0;
    snprintf(dev->name, sizeof(dev->name), "%s", name);

    return dev;
}

void device_print(const Device* dev)
{
    printf("Device %d: %-20s [%s]\n",dev->id,dev->name,dev->online ? "online" : "offline");
}

void device_free(Device* dev)
{
    free(dev);
}





