#include "union.h"

#define DATA_SIZE 65535
#define TRIGGER_LEVEL 13
#define ZERO_TRIGGER_LEVEL 6
#define BOOSTED_TRIGGER_LEVEL 20
#define JUDGE_LEVEL 30

int __judge(float ap1, float ap2, float ap3){
    if ((ap1 < ap3)&&(ap3 < ap2)){
        return 1;
    }
    if ((ap2 < ap3)&&(ap3 < ap1)){
        return 2;
    }
    if ((ap3 >= ap1)&&(ap1 > ap2)){
        return 2;
    }
    if ((ap3 >= ap2)&&(ap2 > ap1)){
        return 1;
    }
    if ((ap3 <= ap1)&&(ap1 < ap2)){
        return 1;
    }
    if ((ap3 <= ap2)&&(ap2 < ap1)){
        return 2;
    }
    return (int)ap3%2+1;
}

int _judge(float ap1, float ap2, float ap3){
    if ((ap1 < ap3)&&(ap3 < ap2)){
        return 1;
    }
    if ((ap2 < ap3)&&(ap3 < ap1)){
        return 2;
    }
    return 0;
}

void clear(int data[DATA_SIZE]){
    for (int i = 0; i < 25; i++){
        data[i] = -1;
    }
}

int judge(int data[DATA_SIZE], int count, int opt){
    float ap1 = 0, ap2 = 0, ap3 = 0;
    int c0 = 0, c1 = 0, c2 = 0, c3 = 0;
    if (data[0] == 1){
        return 1;
    }
    if (data[0] == 2){
        return 2;
    }
    for (int i = 0; i < count; i++){
        switch (data[i]){
            case 0:
                c0++;
                break;
            case 1:
                c1++;
                ap1 += i;
            break;
            case 2:
                c2++;
                ap2 += i;
            break;
            case 3:
                c3++;
                ap3 += i;
            break;
            default:
            break;
        }
    }
    if (c0 > count / 2){
        return 0;
    }
    ap1 /= (float)c1;
    ap2 /= (float)c2;
    ap3 /= (float)c3;
    if (opt){
        return _judge(ap1,ap2,ap3);
    }else{
        return __judge(ap1,ap2,ap3);
    }
}

