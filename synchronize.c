#include "union.h"

void notify(){
    system("python ./Notify.py");
}


int main(void){
    FILE *fnum;
    
    while (1){
        fnum = fopen("./data/lnum.txt","w");
        fprintf(fnum,"%d",num);
        fclose(fnum);
        sleep(1);
    }
}