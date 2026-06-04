#ifndef TODO_H
#define TODO_H

typedef struct {
    int id;
    char title[128];
    int done;
    int priority;
} Todo;

Todo *todo_create(int id, const char *title, int priority);
void todo_print(const Todo *t);
void todo_free(Todo *t);


#endif