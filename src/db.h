#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

typedef struct {
  int count;
  char **list;
} db_list_t;

int db_init(const char *);
void db_close(void);
int db_set(char *, char *);
void *db_get(const char *, int (*)(void *, int,  char **, char **));
void *db_list(const char *, int (*)(void *, int,  char **, char **));
