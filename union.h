#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int num; // 8 bits will be the limit
FILE *fn;
FILE *fs;

int end_flag;

int init_num(int n);
int get_num();
int in();
int out();

void for_sync();
void notify();
int get_sync();
int get_total();