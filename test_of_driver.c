#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(void){
    char buf[255];
    
    int fd;

    if ((fd = open("/dev/test_device0", O_RDWR))<0) perror("open");

    
    if (write(fd,"00",4)<0) perror("write");

    if (read(fd, buf,4)<0) perror("read");
    printf("%s",buf);

    if (close(fd) !=0) perror("close");

}