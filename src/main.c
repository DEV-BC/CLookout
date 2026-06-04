#include <stdio.h>
#include "app_data.h"

int main(void) {
    AppData data = {0};

    data.devices[data.device_count++] = device_create(1, "server-01");
    data.devices[data.device_count++] = device_create(2, "router-02");
    data.devices[data.device_count++] = device_create(3, "switch-03");
    data.devices[1]->online = 1;

    printf("=== Devices ===\n");
    for (int i = 0; i < data.device_count; i++) {
        device_print(data.devices[i]);
    }

    data.incidents[data.incident_count++] = incident_create(1, "Database connection dropped", SEVERITY_HIGH);
    data.incidents[data.incident_count++] = incident_create(2, "Disk usage at 94%", SEVERITY_CRITICAL);
    data.incidents[0]->status = STATUS_INVESTIGATING;

    printf("\n=== Incidents ===\n");
    for (int i = 0; i < data.incident_count; i++) {
        incident_print(data.incidents[i]);
    }

    data.todos[data.todo_count++] = todo_create(1, "Review server logs", 2);
    data.todos[data.todo_count++] = todo_create(2, "Update firewall rules", 3);
    data.todos[0]->done = 1;

    printf("\n=== Todos ===\n");
    for (int i = 0; i < data.todo_count; i++) {
        todo_print(data.todos[i]);
    }

    for (int i = 0; i < data.device_count;   i++) device_free(data.devices[i]);
    for (int i = 0; i < data.incident_count; i++) incident_free(data.incidents[i]);
    for (int i = 0; i < data.todo_count;     i++) todo_free(data.todos[i]);

    return 0;
}


