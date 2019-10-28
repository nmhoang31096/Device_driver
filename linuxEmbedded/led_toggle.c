#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#define OE_ADDR 0x134
#define GPIO_DATAOUT 0x13C
#define GPIO_DATAIN 0x138
#define GPIO2_ADDR 0x481AC000
#define GPIO_SETDATAOUT		0x194
#define GPIO_CLEARDATAOUT	0x190

int main(){
    int fd = open("/dev/mem",O_RDWR | O_SYNC);
    unsigned long* addr =  (unsigned long*) mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO2_ADDR);
    perror("Error: ");
    /*
    /set pin 3 of GPIO2 as output
    /we have to divide by 4 since addr is an unsigned long pointer 
    /and the offsets are in bytes
    */
    addr[OE_ADDR/4] &= (0xFFFFFFFF ^ (1 << 3));
    while(1)
    {
      addr[GPIO_SETDATAOUT/4]  |= (1 << 3);
      sleep(1);
      addr[GPIO_CLEARDATAOUT/4]  |= (1 << 3);
      sleep(1);
    }
    return 0;
}