#include "db.h"
#include "chat.h"
#include <sqlite3.h>
#include <string.h>
#include <time.h>

static sqlite3 *g_db = NULL;

int db_open(const char *path) {
    if (sqlite3_open(path, &g_db) != SQLITE_OK) return -1;
    const char *sql =
        "CREATE TABLE IF NOT EXISTS messages ("
        "  id         INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  role       TEXT NOT NULL,"
        "  content    TEXT NOT NULL,"
        "  created_at INTEGER NOT NULL"
        ");";
    return sqlite3_exec(g_db, sql, NULL, NULL, NULL) == SQLITE_OK ? 0 : -1;
}

void db_save_message(const char *role, const char *content) {
    if (g_db == NULL) return;
    sqlite3_stmt *stmt;
    const char *sql =
        "INSERT INTO messages (role, content, created_at) VALUES (?, ?, ?);";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return;
    sqlite3_bind_text(stmt,  1, role,    -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt,  2, content, -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 3, (sqlite3_int64)time(NULL));
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void db_load_messages(AppData *data) {
    if (g_db == NULL) return;
    sqlite3_stmt *stmt;
    const char *sql =
        "SELECT role, content FROM messages ORDER BY id ASC;";
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) != SQLITE_OK) return;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *role    = (const char *)sqlite3_column_text(stmt, 0);
        const char *content = (const char *)sqlite3_column_text(stmt, 1);
        ChatRole r = strcmp(role, "user") == 0 ? ROLE_USER : ROLE_ASSISTANT;
        ChatMessage *msg = chat_message_create(r, content);
        if (msg != NULL)
            message_list_push(&data->messages, msg);
    }
    sqlite3_finalize(stmt);
}

void db_close(void) {
    if (g_db != NULL) {
        sqlite3_close(g_db);
        g_db = NULL;
    }
}