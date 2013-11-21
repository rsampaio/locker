#include "cfg.h"

/* borrowed and modified from inih example */
static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
     cfg_t* pconfig = (cfg_t*)user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

     if (MATCH("db", "file")) {
          pconfig->db_file = strdup(value);
     } else if (MATCH("gpg", "key")) {
          pconfig->gpg_key = strdup(value);
     } else {
          return 0;  /* unknown section/name, error */
     }
     return 1;
}

int cfg_init(char *cfg_file, cfg_t *cfg)
{
     if(ini_parse(cfg_file, handler, cfg) < 0)
          return EXIT_FAILURE;
     else
          return EXIT_SUCCESS;
}
