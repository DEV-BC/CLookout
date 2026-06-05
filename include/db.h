#ifndef DB_H
#define DB_H

#include "app_data.h"

int  db_open(const char *path);
void db_save_message(const char *role, const char *content);
void db_load_messages(AppData *data);
void db_close(void);

#endif