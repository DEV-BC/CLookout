#ifndef CHAT_H
#define CHAT_H

#include <time.h>

typedef enum {
    ROLE_USER,
    ROLE_ASSISTANT
} ChatRole;

typedef struct {
    ChatRole role;
    char content[2048];
    time_t created_at;
} ChatMessage;

ChatMessage *chat_message_create(ChatRole role, const char *content);
void chat_message_print(const ChatMessage *msg);
void chat_message_free(ChatMessage *msg);

#endif