#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <sys/types.h>
#include <pwd.h>
#include "gpg.h"
#include "db.h"
#include "cfg.h"

cfg_t cfg;

void * hide_password(void)
{
  rl_save_prompt();
  if (rl_point > 0)
    rl_show_char('*');
  rl_restore_prompt();
}

static int db_get_callback(void* arg, int num_rows, char** argv, char** column_name)
{
  int i;
  for(i=0; i<num_rows; i++){
    printf("%s", decrypt_text(argv[i], cfg.gpg_key));
  }
  printf("\n");
  return 0;
}

static int db_list_callback(void* arg, int num_rows, char** argv, char** column_name)
{
  int i;
  for(i=0; i<num_rows; i++){
    printf("%s\n", argv[i]);
  }
  return 0;
}

int main(int argc, char **argv) 
{
  int i, c;
  char *optvalue = NULL;
  char *p, *line, *cfg_file;
  db_list_t *list;  
  struct passwd *pwdent;
  
  pwdent = getpwuid(getuid());

  cfg_file = (char *) malloc(strlen(pwdent->pw_dir) + 5);
  snprintf(cfg_file, strlen(pwdent->pw_dir) + 5, "%s/.2p", pwdent->pw_dir);
  
  if (cfg_init(cfg_file, &cfg) < 0) {
    fprintf(stderr, "could not open config file\n");
    return EXIT_FAILURE;
  }

  if (!db_init(cfg.db_file)) {
    fprintf(stderr, "cound not init db\n");
    return EXIT_FAILURE;
  }

  while ((c = getopt(argc, argv, "a:s:p:c:")) != -1) {
    switch (c) {
    case 'a':
      optvalue = optarg;
      db_list(optvalue, db_list_callback);
      break;

    case 's':
      optvalue = optarg;
      printf("password:");
      rl_redisplay_function = (void *) hide_password;
      line = readline("");
      
      db_set(optvalue, encrypt_text(line, cfg.gpg_key));
      break;

    case 'p':
      optvalue = optarg;
      db_get(optvalue, db_get_callback);
      break;
      
    default:
      fprintf(stderr, "Usage:\n\t-s <name>\tSet password\n\t-p <name>\tPrint decrypted password\n\t-a <prefix>\tList all names\n");
    }
  }
  
  db_close();
  return EXIT_SUCCESS;
}
