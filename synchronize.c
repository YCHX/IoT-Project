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
    fclose(fs);
    return snum;
}

int get_total(){
    return num + get_sync();
}