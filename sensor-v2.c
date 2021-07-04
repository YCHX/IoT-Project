#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define DATA_SIZE 65535
#define TRIGGER_LEVEL 20
#define ZERO_TRIGGER_LEVEL 12
#define BOOSTED_TRIGGER_LEVEL 20
#define JUDGE_LEVEL 40

int _judge(float ap1, float ap2, float ap3){
    if ((ap1 < ap3)&&(ap3 < ap2)){
        return 1;
    }
    if ((ap2 < ap3)&&(ap3 < ap1)){
        return 2;
    }
    return 3;
}

void clear(int data[DATA_SIZE]){
    for (int i = 0; i < 25; i++){
        data[i] = -1;
    }
}

int judge(int data[DATA_SIZE], int count, int dominate){
    float ap1 = 0, ap2 = 0, ap3 = 0;
    int c0 = 0, c1 = 0, c2 = 0, c3 = 0;
    for (int i = 0; i < count; i++){
        switch (data[i]){
            case 0:
                c0++;
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
    if (c0 > JUDGE_LEVEL / 2) {
        return 0;
    }
    if (dominate == 1){
        return 1;
    }
    if (dominate == 2){
        return 2;
    }
    ap1 /= (float)c1;
    ap2 /= (float)c2;
    ap3 /= (float)c3;

    return _judge(ap1,ap2,ap3);
}

int main(void){
    char buf[10];
    int data[DATA_SIZE];
    int before = 0;
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
    before = 0;
    // while (1){
    //     printf(". . .");
    //     usleep(10000);
    //     read(fd, buf,4);
    //     if (buf[0] == '0'){
    //         break;
    //     }
    // }
    printf("\n");
    printf("Initialization Complete!\n");

    while (1){
    
    if (count >= DATA_SIZE){
        printf("data overflow\n");
        //Overflow dealing
        return 0;

    } else if (count >= JUDGE_LEVEL){
        dominate = judge(data, count, dominate);
        if (dominate == 1){
            printf("enter\n");
            clear(data);
            count = 0;
            interval = 0;
            trigger = 0;
            dominate = 0;
            dominate_flag = 0;
        }else if (dominate == 2){
            printf("leave\n");
            clear(data);
            count = 0;
            interval = 0;
            trigger = 0;
            dominate = 0;
            dominate_flag = 0;
        }else if (dominate == 3){
            //cannot judge, continue
        }else{
            clear(data);
            count = 0;
            trigger = 0;
            dominate = 0;
            dominate_flag = 0;
        }
        
    }

    //***SLEEP*** (Significant because if no sleep the system will hang up)
    
        usleep(20000);
    
    if (read(fd, buf,4)<0) perror("read");
    printf("%s",buf);

    if (buf[0]!='e') {
        if (trigger){
            if (interval > TRIGGER_LEVEL){
                dominate_flag = 1;
            }
            clear(data);
            count = 0;
        }

        interval = 0;
        
        
        
        
        
        switch (buf[0])
        {
        case '0':
            if (dominate_flag){
                dominate_flag = 0;
            }
            data[count] = 0;
            before = 0;
            count++;

            break;
        case '1':
            if (dominate_flag){
                dominate = 1;
            }
            data[count] = 1;
            before = 1;
            count++;

            break;
        case '2':
            if (dominate_flag){
                dominate = 2;
            }
            data[count] = 2;
            before = 2;
            count++;

            break;
        case '3':
            if (dominate_flag){
                dominate_flag = 0;
            }
            data[count] = 3;
            before = 3;
            count++;

            break;
        default:
            break;
        }
    
        }else{
            interval++;
            printf("-%d-",interval);
            if (trigger){
                data[count] = before;
                count++;
            }
            if (before == 0){
                if (interval > ZERO_TRIGGER_LEVEL){
                    trigger = 1;
                }
            }else{
                if (interval > TRIGGER_LEVEL){
                    trigger = 1;
                }
            }
            
        }
        

    }
    if (close(fd) !=0) perror("close");

}