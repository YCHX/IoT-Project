#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define DATA_SIZE 65535
#define TRIGGER_LEVEL 13
#define ZERO_TRIGGER_LEVEL 6
#define BOOSTED_TRIGGER_LEVEL 20
#define JUDGE_LEVEL 30

int _judge(int ap1, int ap2, int ap3){
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
    return ap3%2+1;
}

void clear(int data[DATA_SIZE]){
    for (int i = 0; i < 25; i++){
        data[i] = -1;
    }
}

int judge(int data[DATA_SIZE], int count){
    float ap1 = -1, ap2 = -1, ap3 = -1;
    int c1 = 0, c2 = 0, c3 = 0;
    if (data[0] == 1){
        return 1;
    }
    if (data[0] == 2){
        return 2;
    }
    for (int i = 0; i < count; i++){
        switch (data[i]){
            case -1:

            break;
            case 0:

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
    ap1 /= (float)c1;
    ap2 /= (float)c2;
    ap3 /= (float)c3;

    return 0;
}

int main(void){
    char buf[10];
    int data[DATA_SIZE];
    int boost = 0;
    int interval = 0;
    int dominate = 0, dominate_flag = 0;
    int wait = 0;
    int count = 0;
    int trigger, zero_trigger;
    int fd;

    if ((fd = open("/dev/test_device0", O_RDONLY))<0) perror("open");

    printf("\n");
    printf("Initializing in progress...\n");
    printf("\n");
    printf("It will take several seconds.\n");
    printf("\n");
    printf("Please make sure there are no moving objects in front of the sensor.\n");
    printf("\n");
    printf("If there are objects, the sensor may give a wrong feedback\n");
    printf("\n");
    while (1){
        printf(". . .");
        read(fd, buf,4);
        if (buf[0] == '0'){
            break;
        }
    }
    printf("\n");
    printf("Initialization Complete!\n");

    while (1){
    if (count >= DATA_SIZE){
        printf("data overflow\n");
        //Overflow dealing

    } else if (count >= JUDGE_LEVEL){
        dominate = judge(data, count);
        if (dominate == 1){
            printf("enter\n");
        }else{
            printf("leave\n");
        }
        wait = 1;
        boost = 0;
        clear(data);
        count = 0;
        interval = 0;
    }
    //***SLEEP*** (Significant because if no sleep the system will hang up)
    if (boost){
        usleep(50000);
    }else {
        usleep(100000);
    }
    if (read(fd, buf,4)<0) perror("read");
    //printf("%s",buf);

    if (buf[0]!='e') {
        interval = 0;
        if (wait) {
            data[count] = buf[0] + '0';
            if (buf[0] == '0'){
                zero_trigger = 1;
            }
            count++;
            continue;
        }
        if (trigger){
            boost = 1;
            trigger = 0;
            dominate_flag = 1;
            clear(data);
            count = 0;
        }
        
        
        
        
        switch (buf[0])
        {
        case '0':
            if (dominate_flag){
                dominate = -1;
                dominate_flag = 0;
            }
            data[count] = 0;
            count++;
            zero_trigger = 1;
            break;
        case '1':
            if (dominate_flag){
                dominate = 1;
                dominate_flag = 0;
            }
            data[count] = 1;
            count++;
            zero_trigger = 0;
            break;
        case '2':
            if (dominate_flag){
                dominate = 2;
                dominate_flag = 0;
            }
            data[count] = 2;
            count++;
            zero_trigger = 0;

            break;
        case '3':
            if (dominate_flag){
                dominate = -1;
                dominate_flag = 0;
            }
            data[count] = 3;
            count++;
            zero_trigger = 0;
            break;
        
        default:
            break;
        }
    
        }else{
            interval++;
            //printf("-%d-",interval);
            if (zero_trigger){
                if (interval > ZERO_TRIGGER_LEVEL){
                    trigger = 1;
                    wait = 0;
                    zero_trigger = 0;
                }
            }else{
                if (interval > TRIGGER_LEVEL){
                    trigger = 1;
                    wait = 0;
                }
            }
            if (boost){
                if (interval > BOOSTED_TRIGGER_LEVEL){
                    if (!wait){
                        dominate = judge(data, count);
                        if (dominate == 1){
                            printf("enter\n");
                        }else{
                            printf("leave\n");
                        }
                        trigger = 1;
                        boost = 0;
                        clear(data);
                        count = 0;
                        interval = 0;
                    }
                }else{
                    data[count] = -1;
                    count++;
                }
            }

        }
        

    }
    if (close(fd) !=0) perror("close");

}