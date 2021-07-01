#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int judge(int p[],int i){
    int one,two,three;
    int before = 0, bbefore = 0;
    int bf, trigger, af;
    for (int k; k < i+1; k++){
        if (p[k] != 3){ 
            if (!trigger) {
                bf = p[k];
            }else{
                af = p[k];
            }
        }
        if (p[k] == 3){
            trigger = 1;
        }
    }
    return 0;
}

int main(void){
    char buf[10];
    int pool[100];
    int i = 0;
    int zero = 0, wait = 0, three = 0;
    int flag = 0;
    int exflag = 0;
    int pflag = 0, pexflag = 0 , noflag = 0;
    int lock = 0;
    int trigger;
    int fd;

    if ((fd = open("/dev/test_device0", O_RDONLY))<0) perror("open");

    
    while (1){
    usleep(100000);
    if (read(fd, buf,4)<0) perror("read");
    if (buf[0]!='e') {
        printf("%s",buf);
        if (wait){
            if (trigger == 0){
                if (buf[0] != '0') {
                    trigger = 1;
                    continue;
                }
                continue;
            }else if (trigger == 1){
                if (buf[0] != '0'){
                    trigger = 2;
                    continue;
                }
                continue;
            }else{
                if (buf[0] == '0'){
                    wait = 0;
                }
                trigger = 0;
            }
        }
        switch (buf[0])
        {
        case '0':
            
            zero = 1;
            if (!lock){
                flag = 0;
                exflag = 0;
            }else{
                if (zero) {
                    lock = 0;
                    flag = 0;
                    exflag = 0;
                    three = 0;
                    noflag = 0;
                }
                if (flag){
                    printf("leave\n");
                    lock = 0;
                    wait = 1;
                    flag = 0;
                    exflag = 0;
                    three = 0;
                    noflag = 0;
                }
                if (exflag){
                    printf("enter\n");
                    lock = 0;
                    wait = 1;
                    flag = 0;
                    exflag = 0;
                    three = 0;
                    noflag = 0;
                }
            }
            break;
        case '1':
            
            zero = 0;
            if (!lock){
                flag = 1; 
                exflag = 0;
            }else{
                if (exflag){
                    printf("enter\n");
                    lock = 0;
                    wait = 1;
                    flag = 0;
                    exflag = 0;
                    three = 0;
                    noflag = 0;
                }
                if (noflag&&(three > 2)){
                    printf("leave\n");
                    lock = 0;
                    wait = 1;
                    flag = 0;
                    exflag = 0;
                    three = 0;
                    noflag = 0;
                }
            }

            break;
        case '2':
            
            zero = 0;
            if (!lock){
                exflag = 1; 
                flag = 0;
            }else{
                if (flag){
                    printf("leave\n");
                    lock = 0;
                    wait = 1;
                    flag = 0;
                    exflag = 0;
                    three = 0;
                    noflag = 0;
                }
                if (noflag&&(three > 2)){
                    printf("enter\n");
                    lock = 0;
                    wait = 1;
                    flag = 0;
                    exflag = 0;
                    three = 0;
                    noflag = 0;
                }
            }

            break;
        case '3':
            
            zero = 0;
            three++;
            if (!lock){
                if ((flag==0)&&(exflag==0)) noflag = 1;
                lock = 1;
                
            }else{
                if (three > 2){
                    if (flag){
                        printf("leave\n");
                        lock = 0;
                        wait = 1;
                        flag = 0;
                        exflag = 0;
                        three = 0;
                        noflag = 0;
                    }
                    if (exflag){
                        printf("enter\n");
                        lock = 0;
                        wait = 1;
                        flag = 0;
                        exflag = 0;
                        three = 0;
                        noflag = 0;
                    }
                }
            }
            break;
        
        default:
            break;
        }
    
        }
        

    }
    if (close(fd) !=0) perror("close");

}