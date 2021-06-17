#include "union.h"

int main(void){
    FILE *tmp;
    int a = 0;
    while (a < 100){
        num = a;
        for_sync();
        printf("%d\n",get_total());
        sleep(1);
        a++;
        if (get_total()>190){
            notify();
        }
    }
    /*
    tmp = fopen("./data/lnum.txt","w");
    fprintf(tmp,"%s","end");
    fclose(tmp);
    */
}