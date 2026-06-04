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