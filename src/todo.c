#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "todo.h"

Todo *todo_create(int id, const char *title, int priority) {
    Todo *t = malloc(sizeof(Todo));
    if (t == NULL) return NULL;
    t->id = id;
    t->done = 0;
    t->priority = priority;
    snprintf(t->title, sizeof(t->title), "%s", title);
    
    return t;
    
}

void todo_print(const Todo *t) {
    const char *status = t->done ? "[x]" : "[ ]";
    printf("%s P%d %s\n", status, t->priority, t->title);
}

void todo_free(Todo *t) {
    free(t);
}