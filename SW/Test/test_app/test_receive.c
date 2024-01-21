#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "uni_coms.h"

int main(){

    int fd;
    fd = open(DEV_FN,O_RDWR);
    uint8_t duties;
    uint8_t karakter = 0x00;
    
    if(fd < 0){

        fprintf(stderr, "ERROR: \"%s\" not opened! \n", DEV_FN);
        fprintf(stderr, "fd = %d %s\n", fd, strerror(-fd));
        return 4;
    }
    
  
    
    int rd = read(fd, &karakter, sizeof(karakter));
    if (rd!=sizeof(karakter)){
        fprintf(stderr, "ERROR: write went wrong!");
        return 4;
    }

    close(fd);
    printf("karakter je %c (0x%02x) \n", karakter, karakter);
    printf("end \n");

    return 0;
}

