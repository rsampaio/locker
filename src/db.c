#include "db.h"

#define MAX_SIZE 1024

sqlite3 *db;

int db_init(const char *db_file)
{
  char *err = NULL;
  
  if (sqlite3_open(db_file, &db) != SQLITE_OK) {
    fprintf(stderr, "open error: %s\n", sqlite3_errmsg(db));
    return 0;
  }

  if (sqlite3_exec(db, "create table if not exists passtable (name TEXT primary key, password BLOB);", NULL, NULL, &err) && err != NULL) {
    fprintf(stderr, "could not create table: %s\n", err);
    exit(EXIT_FAILURE);
  }
  
  return 1;
}

int db_set(char *name, char *password)
{
  char *err = NULL;
  char qry[MAX_SIZE];
  int sz;

  sz = sprintf(qry, "insert or replace into passtable(name, password) values('%s', '%s')", name, password);
  
  if (sqlite3_exec(db, qry, NULL, NULL, &err) && err != NULL) {
    fprintf(stderr, "could not write entry: %s\n", err);
    exit(EXIT_FAILURE);
  }
  return EXIT_SUCCESS;  
}

void *db_get(const char *key,int (*callback)(void *, int,  char **, char **) )
{
  char *err = NULL;
  char qry[MAX_SIZE];
  char password[MAX_SIZE];
  
  int sz;
  sz = sprintf(qry, "select password from passtable where name='%s';", key);
  qry[sz] = '\0';

  if (sqlite3_exec(db, qry, callback, 0, &err) && err != NULL) {
    fprintf(stderr, "could not create table: %s\n", err);
    exit(EXIT_FAILURE);
  }
}

void *db_list(const char *prefix, int (*callback)(void *, int,  char **, char **))
{
  char *err = NULL;
  char qry[MAX_SIZE];
  int sz;
  sz = sprintf(qry, "select name from passtable where name like '%s%%'", prefix);
  qry[sz] = '\0';
  if (sqlite3_exec(db, qry, callback, 0, &err) && err != NULL) {
    fprintf(stderr, "could not create table: %s\n", err);
    exit(EXIT_FAILURE);
  }
}

void db_close(void)
{
  if (sqlite3_close(db) != SQLITE_OK) {
    fprintf(stderr, "close error: %s\n", sqlite3_errmsg(db));
  }
}
