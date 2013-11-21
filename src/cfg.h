#include <string.h>
#include <stdlib.h>
#include "inih/ini.h"

typedef struct {
  const char *db_file;
  const char *gpg_key;
} cfg_t;

int cfg_init(char *cfg_file, cfg_t *cfg);
char * cfg_get(char *key);
