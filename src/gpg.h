#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <locale.h>
#include <gpgme.h>

char * encrypt_text(char *buf, const char *key);
char * decrypt_text(char *cipher, const char *key);
