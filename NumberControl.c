#include "union.h"

int init_num(int n, int l){
    if (n > 0xFF) return -1;
    num = 0x00;
    limit = l;
    return 0;
}

int get_num(){
    return num;
}

int in(){
    return num++;
}

int out(){
    if (num > 0){
        num--;
    }
    return num;
}