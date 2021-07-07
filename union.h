#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/mman.h>
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "../hps_0.h"

#define DATA_SIZE 65535
#define TRIGGER_LEVEL 13
#define ZERO_TRIGGER_LEVEL 6
#define BOOSTED_TRIGGER_LEVEL 20
#define JUDGE_LEVEL 30

#define HW_REGS_BASE (ALT_STM_OFST)
#define HW_REGS_SPAN (0x04000000)
#define HW_REGS_MASK (HW_REGS_SPAN - 1)

int num; // 8 bits will be the limit
int limit;
int notify_changed;


int end_flag;

int init_num(int n);
int get_num();
int in();
int out();

void for_sync();
void notify();
int get_sync();
int get_total();

int __judge(float ap1, float ap2, float ap3);
int _judge(float ap1, float ap2, float ap3);
void clear(int data[DATA_SIZE]);
int judge(int data[DATA_SIZE], int count, int opt);