#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "include/uni_coms.h"

int main(){

    //printf("Radim \n");
    int fd;
    //printf("otvaram fajl \n");
    fd = open(DEV_FN,O_RDWR);
    //printf("otvorio fajl");
    uint8_t duties;
    char karakter;
    
    if(fd < 0){

        fprintf(stderr, "ERROR: \"%s\" not opened! \n", DEV_FN);
        fprintf(stderr, "fd = %d %s\n", fd, strerror(-fd));
        return 4;
    }
    
    int r = write(fd,(char*)&duties, sizeof(duties));
    
    if (r!=sizeof(duties)){
        fprintf(stderr, "ERROR: write went wrong!");
        return 4;
    }
    
    printf("pre citanja");
    int rd = read(fd, &karakter, sizeof(karakter));
    printf("posle citanja");
    if (r!=sizeof(karakter)){
        fprintf(stderr, "ERROR: write went wrong!");
        return 4;
    }

    close(fd);
    printf("karakter je %c ", karakter);
    printf("end");


/*
    int fd;
    printf("otvaram fajl");
    fd = open(DEV_FN,O_RDWR);
    printf("otvorio fajl");
    uint8_t duties;
    char karakter;


    if(fd < 0){

        fprintf(stderr, "ERROR: \"%s\" not opened! \n", DEV_FN);
        fprintf(stderr, "fd = %d %s\n", fd, strerror(-fd));
        return 4;
    }

    int r = write(fd,(char*)&duties, sizeof(duties));

    if (r!=sizeof(duties)){
        fprintf(stderr, "ERROR: write went wrong!");
        return 4;
    }

    int rd = read(fd, &karakter, sizeof(karakter));

    if (r!=sizeof(karakter)){
        fprintf(stderr, "ERROR: write went wrong!");
        return 4;
    }

    close(fd);
    printf("karakter je %c ", karakter);
    printf("end");
*/
    return 0;
}

