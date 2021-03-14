//
//  main.c
//  ip_checksum
//
//  Created by Julicher, Joe on 3/12/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "pico.h"
#include "pico/stdlib.h"
#include "hardware/sync.h"

uint16_t rfcChecksum( uint16_t, const uint8_t *,size_t );
uint16_t GenerateChecksum( uint16_t, const uint8_t *, size_t );
uint16_t usGenerateChecksum( uint16_t, const uint8_t *, size_t );
uint16_t usGenerateChecksum2( uint16_t, const uint8_t *, size_t );
uint16_t usGenerateChecksum32( uint16_t, const uint8_t *, size_t );

uint8_t buffer0[] = {0x00, 0x01, 0xF2, 0x03, 0xF4, 0xF5, 0xF6, 0xF7}; // RFC1071 test case
uint8_t buffer1[128];
uint8_t buffer2[512];
uint8_t buffer3[1024];
uint8_t buffer4[1518];

void testAlgorithm(unsigned testCount, uint16_t (*checksum)(uint16_t, const uint8_t *, size_t))
{
    volatile uint16_t sum;
    sum = checksum(0,buffer0, sizeof(buffer0));
    sum = checksum(0,buffer1, sizeof(buffer1));
    sum = checksum(0,buffer2, sizeof(buffer2));
    sum = checksum(0,buffer3, sizeof(buffer3));
    sum = checksum(0,buffer4, sizeof(buffer4));
}

#define TESTCOUNTS 10000

#define PIN0 (0)
#define PIN1 (1)
#define PIN2 (2)
#define PIN3 (3)

int main(int argc, const char * argv[]) {
    
    stdio_init_all();

    gpio_init(PIN0);
    gpio_init(PIN1);
    gpio_init(PIN2);
    gpio_init(PIN3);
    gpio_set_dir(PIN0,GPIO_OUT);
    gpio_set_dir(PIN1,GPIO_OUT);
    gpio_set_dir(PIN2,GPIO_OUT);
    gpio_set_dir(PIN3,GPIO_OUT);

    uint16_t rfcSum = rfcChecksum(0,buffer0, sizeof(buffer0));
    uint16_t frtSum = usGenerateChecksum(0, buffer0, sizeof(buffer0));
    uint16_t frtSum2 = usGenerateChecksum2(0, buffer0, sizeof(buffer0));
    uint16_t frtSum32 = usGenerateChecksum32(0, buffer0, sizeof(buffer0));

    printf("rfcSum %04X - frtSum %04X - frtSum2 %04X - frtSum32 %04X\n", rfcSum, frtSum, frtSum2, frtSum32);

    srand((unsigned int)clock());
    
    for(int x=0;x<sizeof(buffer1);x++)
    {
        buffer1[x] = rand();
        buffer2[x] = rand();
        buffer3[x] = rand();
        buffer4[x] = rand();
    }
    
    for(int x=sizeof(buffer1);x<sizeof(buffer2);x++)
    {
        buffer2[x] = rand();
        buffer3[x] = rand();
        buffer4[x] = rand();
    }

    for(int x=sizeof(buffer2);x<sizeof(buffer3);x++)
    {
        buffer3[x] = rand();
        buffer4[x] = rand();
    }

    for(int x=sizeof(buffer3);x<sizeof(buffer4);x++)
    {
        buffer4[x] = rand();
    }

    while(1)
    {
        uint32_t irq_status = save_and_disable_interrupts();
        gpio_put(PIN0,1);
        rfcSum = rfcChecksum(0,buffer4, sizeof(buffer4));
        gpio_put(PIN0,0);
        gpio_put(PIN1,1);
        frtSum = usGenerateChecksum(0, buffer4, sizeof(buffer4));
        gpio_put(PIN1,0);
        gpio_put(PIN2,1);
        frtSum2 = usGenerateChecksum2(0, buffer4, sizeof(buffer4));
        gpio_put(PIN2,0);
        gpio_put(PIN3,1);
        frtSum32 = usGenerateChecksum32(0, buffer4, sizeof(buffer4));
        gpio_put(PIN3,0);
        restore_interrupts(irq_status);

        printf("rfcSum %04X - frtSum %04X - frtSum2 %04X - frtSum32 %04X\n", rfcSum, frtSum, frtSum2, frtSum32);
    }
}
