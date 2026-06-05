#include <pthread.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "colors.h"
#include "layout.h"
#include "app_data.h"
#include "db.h"
#include <cjson/cJSON.h>

#define AI_USER_PROMPT "What is a mutex in C? One paragraph."

static pthread_mutex_t g_clock_mutex = PTHREAD_MUTEX_INITIALIZER;
static char            g_clock_str[32] = "--:--:--";


static pthread_mutex_t g_ai_mutex    = PTHREAD_MUTEX_INITIALIZER;
static char            g_ai_response[4096] = "";
static int             g_ai_loading  = 0;

static void *clock_thread(void *arg) {
    (void)arg;
    while (1) {
        time_t     now = time(NULL);
        struct tm *t   = localtime(&now);
        pthread_mutex_lock(&g_clock_mutex);
        strftime(g_clock_str, sizeof(g_clock_str), "%H:%M:%S", t);
        pthread_mutex_unlock(&g_clock_mutex);
        sleep(1);
    }
    return NULL;
}

typedef struct {
    char linebuf[2048];
    int  linebuf_len;
} StreamState;

static void process_sse_line(const char *line) {
    if (strncmp(line, "data: ", 6) != 0) return;
    const char *json_str = line + 6;
    if (strcmp(json_str, "[DONE]") == 0) return;

    cJSON *root    = cJSON_Parse(json_str);
    if (root == NULL) return;
    cJSON *choices = cJSON_GetObjectItem(root, "choices");
    cJSON *first   = cJSON_GetArrayItem(choices, 0);
    cJSON *delta   = cJSON_GetObjectItem(first, "delta");
    cJSON *content = cJSON_GetObjectItem(delta, "content");
    if (content != NULL && content->valuestring != NULL) {
        pthread_mutex_lock(&g_ai_mutex);
        strncat(g_ai_response, content->valuestring,
                sizeof(g_ai_response) - strlen(g_ai_response) - 1);
        pthread_mutex_unlock(&g_ai_mutex);
    }
    cJSON_Delete(root);
}

static size_t stream_write_cb(char *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t       total = size * nmemb;
    StreamState *state = (StreamState *)userdata;

    for (size_t i = 0; i < total; i++) {
        char c = ptr[i];
        if (c == '\n') {
            state->linebuf[state->linebuf_len] = '\0';
            if (state->linebuf_len > 0)
                process_sse_line(state->linebuf);
            state->linebuf_len = 0;
        } else if (state->linebuf_len < (int)sizeof(state->linebuf) - 1) {
            state->linebuf[state->linebuf_len++] = c;
        }
    }
    return total;
}
static void *ai_request_thread(void *arg) {
    (void)arg;
    const char *api_key = getenv("OPENAI_API_KEY");
    if (api_key == NULL) {
        pthread_mutex_lock(&g_ai_mutex);
        snprintf(g_ai_response, sizeof(g_ai_response), "Error: OPENAI_API_KEY not set");
        g_ai_loading = 0;
        pthread_mutex_unlock(&g_ai_mutex);
        return NULL;
    }

     const char *body =
        "{\"model\":\"gpt-4o-mini\",\"stream\":true,"
        "\"messages\":[{\"role\":\"user\",\"content\":\"" AI_USER_PROMPT "\"}]}";

    char auth_header[256];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);

    StreamState state = { .linebuf_len = 0 };

    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        pthread_mutex_lock(&g_ai_mutex);
        snprintf(g_ai_response, sizeof(g_ai_response), "Error: curl_easy_init failed");
        g_ai_loading = 0;
        pthread_mutex_unlock(&g_ai_mutex);
        return NULL;
    }

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth_header);

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, stream_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &state);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    pthread_mutex_lock(&g_ai_mutex);
    if (res != CURLE_OK) {
        snprintf(g_ai_response, sizeof(g_ai_response),
                 "curl error: %s", curl_easy_strerror(res));
    }
    g_ai_loading = 0;
    pthread_mutex_unlock(&g_ai_mutex);

    return NULL;
}
    

static void load_sample_data(AppData *data) {
    const char *json =
        "[{\"id\":1,\"name\":\"server-01\",\"online\":true},"
        " {\"id\":2,\"name\":\"router-02\",\"online\":false},"
        " {\"id\":3,\"name\":\"switch-03\",\"online\":true}]";

    cJSON *arr = cJSON_Parse(json);
    if (arr == NULL) return;

    cJSON *item;
    cJSON_ArrayForEach(item, arr) {
        int  id     = cJSON_GetObjectItem(item, "id")->valueint;
        const char *name = cJSON_GetObjectItem(item, "name")->valuestring;
        int  online = cJSON_IsTrue(cJSON_GetObjectItem(item, "online"));

        if (data->device_count < MAX_DEVICES) {
            Device *d = device_create(id, name);
            if (d != NULL) {
                d->online = online;
                data->devices[data->device_count++] = d;
            }
        }
    }

    cJSON_Delete(arr);

     data->incidents[data->incident_count++] = incident_create(1, "Database connection dropped", SEVERITY_HIGH);
    data->incidents[data->incident_count++] = incident_create(2, "Disk usage at 94%", SEVERITY_CRITICAL);

    data->todos[data->todo_count++] = todo_create(1, "Review server logs", 2);
    data->todos[data->todo_count++] = todo_create(2, "Update firewall rules", 3);
    data->todos[0]->done = 1;
}

int main(void) {
    initscr();
    curl_global_init(CURL_GLOBAL_DEFAULT);
    db_open("sentinel.db");
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
     timeout(100);

    pthread_t clock_tid;
    pthread_create(&clock_tid, NULL, clock_thread, NULL);
    pthread_detach(clock_tid);
    colors_init();

    AppData data = {0};
    load_sample_data(&data);
    db_load_messages(&data);

    Layout *l = layout_create();
    if (l == NULL) { endwin(); return 1; }

    layout_draw(l, &data, g_clock_str, g_ai_response, g_ai_loading);
    layout_refresh(l);

    int ch;
    int prev_loading = 0;
    while ((ch = getch()) != 'q') {
        if (ch != ERR) {
             switch (ch) {
            case 'd': case 'D': l->active = SECTION_DEVICES;   l->cursor = 0; break;
            case 'i': case 'I': l->active = SECTION_INCIDENTS; l->cursor = 0; break;
            case 't': case 'T': l->active = SECTION_TODOS;     l->cursor = 0; break;
            case 'a': case 'A': l->active = SECTION_AI;        l->cursor = 0; break;
        case ' ':
                if (l->active == SECTION_TODOS && data.todo_count > 0) {
                    data.todos[l->cursor]->done ^= 1;
                }
                break;
            case '\n':
            case KEY_ENTER:
                if (l->active == SECTION_DEVICES && data.device_count > 0) {
                    data.devices[l->cursor]->online ^= 1;
                } else if (l->active == SECTION_INCIDENTS && data.incident_count > 0) {
                    data.incidents[l->cursor]->status =
                        (data.incidents[l->cursor]->status + 1) % 3;
                } else if (l->active == SECTION_TODOS && data.todo_count > 0) {
                    data.todos[l->cursor]->done ^= 1;
                } else if (l->active == SECTION_AI) {
                pthread_mutex_lock(&g_ai_mutex);
                int already_loading = g_ai_loading;
                pthread_mutex_unlock(&g_ai_mutex);
                if (!already_loading) {
                    pthread_mutex_lock(&g_ai_mutex);
                    g_ai_loading = 1;
                    g_ai_response[0] = '\0';
                    pthread_mutex_unlock(&g_ai_mutex);
                    pthread_t ai_tid;
                    pthread_create(&ai_tid, NULL, ai_request_thread, NULL);
                    pthread_detach(ai_tid);
                }
            }
                break;
            case KEY_UP:   if (l->cursor > 0) l->cursor--; break;
            case KEY_DOWN: {
            int counts[] = {
                data.device_count,
                data.incident_count,
                data.todo_count,
                data.message_count
            };
            int max = counts[l->active];
            if (max > 0 && l->cursor < max - 1) l->cursor++;
            break;
        }
            }
        }
        char clock_snap[32];
        pthread_mutex_lock(&g_clock_mutex);
        strncpy(clock_snap, g_clock_str, sizeof(clock_snap));
        pthread_mutex_unlock(&g_clock_mutex);
        char ai_snap[4096];
        int  ai_loading_snap;
        pthread_mutex_lock(&g_ai_mutex);
        strncpy(ai_snap, g_ai_response, sizeof(ai_snap));
        ai_loading_snap = g_ai_loading;
        pthread_mutex_unlock(&g_ai_mutex);
        if (prev_loading == 1 && ai_loading_snap == 0 && ai_snap[0] != '\0') {
            db_save_message("user", AI_USER_PROMPT);
            db_save_message("assistant", ai_snap);
            if (data.message_count < MAX_MESSAGES) {
                ChatMessage *u = chat_message_create(ROLE_USER, AI_USER_PROMPT);
                if (u) data.messages[data.message_count++] = u;
            }
            if (data.message_count < MAX_MESSAGES) {
                ChatMessage *a = chat_message_create(ROLE_ASSISTANT, ai_snap);
                if (a) data.messages[data.message_count++] = a;
            }
        }
        prev_loading = ai_loading_snap;
        layout_draw(l, &data, clock_snap, ai_snap, ai_loading_snap);
        layout_refresh(l);
    }

    for (int i = 0; i < data.device_count; i++) device_free(data.devices[i]);
    for (int i = 0; i < data.incident_count; i++) incident_free(data.incidents[i]);
    for (int i = 0; i < data.todo_count;     i++) todo_free(data.todos[i]);
    for (int i = 0; i < data.message_count; i++) chat_message_free(data.messages[i]);
    layout_free(l);
    curl_global_cleanup();
    db_close();
    endwin();
    return 0;
}