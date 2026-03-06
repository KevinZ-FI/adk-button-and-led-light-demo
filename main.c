#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.
#define	LED	0

#define SLAVE_1_DEVICE_ID 0x20
#define SLAVE_2_DEVICE_ID 0x23

#define REG_IODIR     0x00
#define REG_IPOL      0x01
#define REG_GPINTEN   0x02
#define REG_DEFVAL    0x03
#define REG_INTCON    0x04
#define REG_IOCON     0x05
#define REG_GPPU      0x06
#define REG_INTF      0x07
#define REG_INTCAP    0x08  //Read-only
#define REG_GPIO      0x09
#define REG_OLAT      0x0a

struct stillLight
{
    int state;
    unsigned char value_s1;
    unsigned char value_s2;
};

struct stillLight light[] = {{0, 0b01000000, 0b00100000},
                             {1, 0b00100000, 0b00100011},
                             {2, 0b01000000, 0b00111111},
                             {3, 0b00100000, 0b01111111},
                             {4, 0b01000000, 0b10111111},
                             {5, 0b01100000, 0b11111111}};

int main (void)
{
    printf ("Raspberry Pi blink\n") ;

    wiringPiSetup () ;
    pinMode (LED, OUTPUT) ;

    for (int i = 0; i < 5; i++)
    {
        digitalWrite (LED, HIGH) ;	// On
        delay (500) ;		// mS
        digitalWrite (LED, LOW) ;	// Off
        delay (500) ;
    }

    int fd_s1 = wiringPiI2CSetup(SLAVE_1_DEVICE_ID);
    if (fd_s1 == -1)
    {
        printf("Failed to init I2C Slave 1 communication\n");
        return -1;
    }
    
    int fd_s2 = wiringPiI2CSetup(SLAVE_2_DEVICE_ID);
    if (fd_s2 == -1)
    {
        printf("Failed to init I2C Slave 2 communication\n");
        return -1;
    }

    wiringPiI2CWriteReg8(fd_s1, REG_IODIR, 0b00000111); //0,1,2: button; 5,6: light 
    wiringPiI2CWriteReg8(fd_s2, REG_IODIR, 0b00000000); //0,1,2,3,4,5,6,7: light

    wiringPiI2CWriteReg8(fd_s1, REG_GPIO, 0b00000000);
    wiringPiI2CWriteReg8(fd_s2, REG_GPIO, 0b00000000);

    for(int i = 0; i < 6; i++)
    {
        delay(500);
        wiringPiI2CWriteReg8(fd_s1, REG_GPIO, light[i].value_s1);
        wiringPiI2CWriteReg8(fd_s2, REG_GPIO, light[i].value_s2);
    }

    return 0 ;
}
