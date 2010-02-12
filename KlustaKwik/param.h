/* $Id: param.h,v 1.1 2008/01/05 15:38:03 samn Exp $ */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

enum type_t {KK_FLOAT = 'f', KK_INT = 'd',  KK_BOOLEAN = 'b', KK_STRING = 's'};

#define FLOAT_PARAM(name) add_param(KK_FLOAT, #name, &name)
#define INT_PARAM(name) add_param(KK_INT, #name, &name)
#define BOOLEAN_PARAM(name) add_param(KK_BOOLEAN, #name, &name)
#define STRING_PARAM(name) add_param(KK_STRING, #name, name)

#define STRLEN 10000

void add_param(int t, char *name, void *addr);

int change_param(char *name, char *value);

void init_params(int argc, char **argv);

void print_params(FILE *fp);
