#ifndef LIBRND_H
#define LIBRND_H

#include <my_global.h>
#include <my_sys.h>
#include <mysql.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RNG_DEV "/dev/urandom"
#define DEFAULT_ENTROPY (16)
#define MIN_BYTES (1)
#define MAX_BYTES (1 * 1024 * 1024)

my_bool urandom_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void urandom_deinit(UDF_INIT *initid);
char *urandom(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

#endif
