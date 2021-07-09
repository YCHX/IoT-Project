#include "union.h"

void notify(){
    char x[100];
    sprintf(x,"python ./Notify.py %d",get_total());
    system(x);
}

void for_sync(){
    fn = fopen("./data/lnum.txt","w");
    fprintf(fn,"%d",num);
    fclose(fn);
}

int get_sync(){
    int snum;
    fs = fopen("./data/snum.txt","r");
    fscanf(fs,"%d",&snum);
    if ((snum < 0)||(snum > 0xFF)){
        return 0;
    }
    fclose(fs);
    return snum;
}

int get_total(){
    return num + get_sync();
}

void reset_sync(){
    FILE *tmp;
    tmp = fopen("./data/snum.txt","w");
    fprintf(tmp,"0");
    fclose(tmp);
}