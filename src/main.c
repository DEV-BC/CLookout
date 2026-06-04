#include <stdio.h>
#include <time.h>
#include "chat.h"
#include "device.h"
#include "incident.h"
#include "todo.h"


int main(void) {

    Device *dev = device_create(1, "server-01");
    if (dev == NULL) return 1;

    device_print(dev);

    dev->online = 1;
    device_print(dev);

    device_free(dev);
    dev = NULL;

     printf("\n");

    Incident *inc = incident_create(1, "Database connection dropped", SEVERITY_HIGH);
    if (inc == NULL) return 1;
    incident_print(inc);
    inc->status = STATUS_INVESTIGATING;
    incident_print(inc);

    incident_free(inc);
    inc = NULL;


    printf("\n");

    Todo *t = todo_create(1, "Review Server Logs", 2);
    if (t == NULL) return 1;
    todo_print(t);
    t->done = 1;
    todo_print(t);

    todo_free(t);

    t = NULL;

     printf("\n");

     ChatMessage *m1 = chat_message_create(ROLE_USER, "What is wrong with the database?");
     ChatMessage *m2 = chat_message_create(ROLE_ASSISTANT, "The connection pool is exhausted.");
     if (m1 == NULL || m2 == NULL) {
        return 1;
     }

     chat_message_print(m1);
     chat_message_print(m2);

    chat_message_free(m1);
    chat_message_free(m2);
    m1 = NULL;
    m2 = NULL;

    return 0;
}

