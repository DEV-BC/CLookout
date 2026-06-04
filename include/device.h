#ifndef DEVICE_H
#define DEVICE_H

typedef struct
{
    int id;
    char name[64];
    int online;
} Device;

Device* device_create(int id, const char* name);
void device_print(const Device* dev);
void device_free(Device* dev);

#endif
