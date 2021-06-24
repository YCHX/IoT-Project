#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "../hps_0.h"

#include "union.h"

#define HW_REGS_BASE (ALT_STM_OFST)
#define HW_REGS_SPAN (0x04000000)
#define HW_REGS_MASK (HW_REGS_SPAN - 1)

int 
send()
{
    void *virtual_base;
    int fd;
    int sfd;
    int send_mask;
    void *h2p_lw_led_addr;

    if ((fd = open( "/dev/mem", (O_RDWR|O_SYNC))) == -1) {
        printf("ERROR: could not open \"/dev/mem\"...\n");
        return 1;
    }
    virtual_base = mmap(NULL, HW_REGS_SPAN, (PROT_READ|PROT_WRITE),
                        MAP_SHARED, fd, HW_REGS_BASE);
    if (virtual_base == MAP_FAILED) {
        printf("ERROR: mmap() failed...\n");
        close(fd);
        return 1;
    }
    h2p_lw_led_addr = virtual_base
        + ((unsigned long)(ALT_LWFPGASLVS_OFST + MYPIO_0_BASE)
            & (unsigned long)(HW_REGS_MASK));
    
    while (1){
        *(uint32_t *)h2p_lw_led_addr = get_total();
        usleep(200*100);
    }
}