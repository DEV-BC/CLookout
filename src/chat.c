#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "chat.h"

ChatMessage *chat_message_create(ChatRole role, const char *content) {
    ChatMessage *msg = malloc(sizeof(ChatMessage));
    if (msg == NULL) return NULL;
    msg->role = role;
    msg->created_at = time(NULL);
    snprintf(msg->content, sizeof(msg->content), "%s", content);

    return msg;
}

void chat_message_print(const ChatMessage *msg) {
    const char *role = msg->role == ROLE_USER ? "user" : "assistant";
    printf("[%s]: %s\n", role, msg->content);
}

void chat_message_free(ChatMessage *msg) {
    free(msg);
}

void message_list_init(MessageList *list) {
    list->capacity = 8;
    list->count    = 0;
    list->data     = malloc(list->capacity * sizeof(ChatMessage *));
}

void message_list_push(MessageList *list, ChatMessage *msg) {
    if (list->data == NULL) return;
    if (list->count == list->capacity) {
        int new_cap = list->capacity * 2;
        ChatMessage **new_data = realloc(list->data, new_cap * sizeof(ChatMessage *));
        if (new_data == NULL) return;
        list->data     = new_data;
        list->capacity = new_cap;
    }
    list->data[list->count++] = msg;
}

void message_list_free(MessageList *list) {
    for (int i = 0; i < list->count; i++)
        chat_message_free(list->data[i]);
    free(list->data);
    list->data     = NULL;
    list->count    = 0;
    list->capacity = 0;
}