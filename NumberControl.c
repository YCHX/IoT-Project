#include <stdio.h>

static int num; // 8 bits will be the limit

int init_num(int n){
    if (n > 0xFF) return -1;
    num = 0x00;
    return 0;
}

int get_num(){
    return num;
}

int in(){
    return num++;
}

int out(){
    return num--;
}