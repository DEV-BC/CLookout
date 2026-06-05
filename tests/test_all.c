#include <stdio.h>
#include <string.h>
#include "device.h"
#include "incident.h"
#include "todo.h"
#include "chat.h"

#define ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "  FAIL %s:%d  %s\n", __FILE__, __LINE__, msg); \
            return 1; \
        } \
    } while (0)

/* ── device ───────────────────────────────── */

static int test_device_create(void) {
    Device *d = device_create(1, "server-01");
    ASSERT(d != NULL,                          "device_create returned NULL");
    ASSERT(d->id == 1,                         "id should be 1");
    ASSERT(d->online == 0,                     "online should default to 0");
    ASSERT(strcmp(d->name, "server-01") == 0,  "name mismatch");
    device_free(d);
    return 0;
}

static int test_device_name_truncation(void) {
    const char *long_name =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789__";
    Device *d = device_create(2, long_name);
    ASSERT(d != NULL,              "device_create returned NULL");
    ASSERT(strlen(d->name) <= 63,  "name longer than buffer allows");
    ASSERT(d->name[63] == '\0',    "name not null-terminated at boundary");
    device_free(d);
    return 0;
}

/* ── incident ─────────────────────────────── */

static int test_incident_create(void) {
    Incident *inc = incident_create(1, "Disk full", SEVERITY_CRITICAL);
    ASSERT(inc != NULL,                          "incident_create returned NULL");
    ASSERT(inc->id == 1,                         "id should be 1");
    ASSERT(inc->severity == SEVERITY_CRITICAL,   "severity mismatch");
    ASSERT(inc->status == STATUS_OPEN,           "status should default to STATUS_OPEN");
    ASSERT(strcmp(inc->title, "Disk full") == 0, "title mismatch");
    incident_free(inc);
    return 0;
}

/* ── todo ─────────────────────────────────── */

static int test_todo_create(void) {
    Todo *t = todo_create(1, "Write tests", 2);
    ASSERT(t != NULL,                            "todo_create returned NULL");
    ASSERT(t->id == 1,                           "id should be 1");
    ASSERT(t->priority == 2,                     "priority should be 2");
    ASSERT(t->done == 0,                         "done should default to 0");
    ASSERT(strcmp(t->title, "Write tests") == 0, "title mismatch");
    todo_free(t);
    return 0;
}

/* ── chat message ─────────────────────────── */

static int test_chat_message_user(void) {
    ChatMessage *msg = chat_message_create(ROLE_USER, "hello");
    ASSERT(msg != NULL,                       "chat_message_create returned NULL");
    ASSERT(msg->role == ROLE_USER,            "role should be ROLE_USER");
    ASSERT(strcmp(msg->content, "hello") == 0, "content mismatch");
    ASSERT(msg->created_at > 0,               "created_at should be set");
    chat_message_free(msg);
    return 0;
}

static int test_chat_message_assistant(void) {
    ChatMessage *msg = chat_message_create(ROLE_ASSISTANT, "hi there");
    ASSERT(msg != NULL,                "chat_message_create returned NULL");
    ASSERT(msg->role == ROLE_ASSISTANT, "role should be ROLE_ASSISTANT");
    chat_message_free(msg);
    return 0;
}

/* ── message list ─────────────────────────── */

static int test_message_list_init(void) {
    MessageList list;
    message_list_init(&list);
    ASSERT(list.data != NULL,   "data should be allocated");
    ASSERT(list.count == 0,     "count should start at 0");
    ASSERT(list.capacity == 8,  "capacity should start at 8");
    message_list_free(&list);
    return 0;
}

static int test_message_list_push(void) {
    MessageList list;
    message_list_init(&list);
    ChatMessage *msg = chat_message_create(ROLE_USER, "test");
    message_list_push(&list, msg);
    ASSERT(list.count == 1,      "count should be 1 after one push");
    ASSERT(list.data[0] == msg,  "data[0] should be the pushed message");
    message_list_free(&list);
    return 0;
}

static int test_message_list_grows(void) {
    MessageList list;
    message_list_init(&list);
    for (int i = 0; i < 9; i++) {
        ChatMessage *msg = chat_message_create(ROLE_USER, "x");
        message_list_push(&list, msg);
    }
    ASSERT(list.count == 9,      "count should be 9");
    ASSERT(list.capacity == 16,  "capacity should have doubled to 16");
    message_list_free(&list);
    return 0;
}

/* ── runner ───────────────────────────────── */

typedef int (*TestFn)(void);

static int run_test(const char *name, TestFn fn) {
    int result = fn();
    printf("%s  %s\n", result == 0 ? "PASS" : "FAIL", name);
    return result;
}

int main(void) {
    int failures = 0;

    failures += run_test("device_create",          test_device_create);
    failures += run_test("device_name_truncation", test_device_name_truncation);
    failures += run_test("incident_create",        test_incident_create);
    failures += run_test("todo_create",            test_todo_create);
    failures += run_test("chat_message_user",      test_chat_message_user);
    failures += run_test("chat_message_assistant", test_chat_message_assistant);
    failures += run_test("message_list_init",      test_message_list_init);
    failures += run_test("message_list_push",      test_message_list_push);
    failures += run_test("message_list_grows",     test_message_list_grows);

    printf("\n%d test(s) failed.\n", failures);
    return failures > 0 ? 1 : 0;
}