#include "union.h"

int main(int argc, char** argv){
    //for sensor
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
    //for send
    void *virtual_base;
    int fds;
    int send_mask;
    void *h2p_lw_led_addr;
    int total;


    if (argc == 3){
        init_num(atoi(argv[1]),atoi(argv[2]));
    }else{
        init_num(0,10);
    }

    //open sensor device file
    if ((fd = open("/dev/test_device0", O_RDONLY))<0) perror("open");

    //set sending mask
    if ((fds = open( "/dev/mem", (O_RDWR|O_SYNC))) == -1) {
        printf("ERROR: could not open \"/dev/mem\"...\n");
        return 1;
    }
    virtual_base = mmap(NULL, HW_REGS_SPAN, (PROT_READ|PROT_WRITE),
                        MAP_SHARED, fds, HW_REGS_BASE);
    if (virtual_base == MAP_FAILED) {
        printf("ERROR: mmap() failed...\n");
        close(fds);
        return 1;
    }
    h2p_lw_led_addr = virtual_base
        + ((unsigned long)(ALT_LWFPGASLVS_OFST + MYPIO_0_BASE)
            & (unsigned long)(HW_REGS_MASK));

    //init message (actually no init, just prints)
    printf("\n");
    printf("Initializing in progress...\n");
    printf("\n");
    printf("It will take several seconds.\n");
    printf("\n");
    printf("Please make sure there are no moving objects in front of the sensor.\n");
    printf("\n");
    printf("If there are objects, the sensor may give a wrong feedback\n");
    printf("\n");
    //No init any more!
    printf("\n");
    printf("Initialization Complete!\n");

    while (1){
    //prepare for syncing
    //for_sync();
    
    //send to fpga
    total = num;
    *(uint32_t *)h2p_lw_led_addr = total;
    
    
    //check the limit
    // if ((total > limit)){
    //     if (notify_changed != total){
    //         notify();
    //         notify_changed = total;
    //     }
    // }

    if (count >= DATA_SIZE){
        printf("data overflow\n");
        //Overflow dealing

    } else if (count >= JUDGE_LEVEL){
        dominate = judge(data, count, 1);
        if (dominate == 1){
            printf("enter\n");
            in();
            wait = 1;
            boost = 0;
            clear(data);
            count = 0;
            interval = 0;
        }else if (dominate == 2){
            printf("leave\n");
            out();
            wait = 1;
            boost = 0;
            clear(data);
            count = 0;
            interval = 0;
        }else{
            
        }
        
    }
    //***SLEEP*** (Significant because if no sleep the system will hang up)
    if (boost){
        usleep(50000);
    }else {
        usleep(100000);
    }
    if (read(fd, buf,4)<0) perror("read");
    

    if (buf[0]!='e') {
        printf("%s",buf);
        interval = 0;
        if (wait) {
            data[count] = buf[0] - '0';
            before = data[count];
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
            before = data[count];
            count++;
            zero_trigger = 1;
            break;
        case '1':
            if (dominate_flag){
                dominate = 1;
                dominate_flag = 0;
            }
            data[count] = 1;
            before = data[count];
            count++;
            zero_trigger = 0;
            break;
        case '2':
            if (dominate_flag){
                dominate = 2;
                dominate_flag = 0;
            }
            data[count] = 2;
            before = data[count];
            count++;
            zero_trigger = 0;

            break;
        case '3':
            if (dominate_flag){
                dominate = -1;
                dominate_flag = 0;
            }
            data[count] = 3;
            before = data[count];
            count++;
            zero_trigger = 0;
            break;
        
        default:
            break;
        }
    
        }else{
            interval++;
            printf("-");
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
                        dominate = judge(data, count, 0);
                        if (dominate == 1){
                            printf("enter\n");
                            in();
                            trigger = 1;
                            boost = 0;
                            clear(data);
                            count = 0;
                            interval = 0;
                        }else if (dominate == 2){
                            printf("leave\n");
                            out();
                            trigger = 1;
                            boost = 0;
                            clear(data);
                            count = 0;
                            interval = 0;
                        }else{
                            data[count] = before;
                            count++;
                        }
                        
                    }
                }else{
                    data[count] = before;
                    count++;
                }
            }

        }
        

    }
    if (close(fd) !=0) perror("close");
    if (munmap( virtual_base, HW_REGS_SPAN ) != 0) {
        printf("ERROR: munmap() failed...\n");
        close(fds);
        return 1;
    }
    close(fd);
}

