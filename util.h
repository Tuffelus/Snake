#include <limits.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FAILURE INT_MIN

int get_token_value(char *filename, char *token);
int write_token_value(char *filename, char *token, int new_val);
void add_token(char *filename, char *token, int val);
