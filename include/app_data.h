#ifndef APP_DATA_H
#define APP_DATA_H

#include "device.h"
#include "incident.h"
#include "todo.h"
#include "chat.h"

#define MAX_DEVICES 64
#define MAX_INCIDENTS 256
#define MAX_TODOS 256
#define MAX_MESSAGES 512

typedef struct {
    Device *devices[MAX_DEVICES];
    int device_count;

    Incident *incidents[MAX_INCIDENTS];
    int incident_count;

    Todo *todos[MAX_TODOS];
    int todo_count;

    ChatMessage *messages[MAX_MESSAGES];
    int message_count;
} AppData;


#endif