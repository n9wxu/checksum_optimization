// reference code from RFC1071
#include <stdint.h>
#include <stdlib.h>

#define swap16(data) ( (((data) >> 8) & 0x00FF) | (((data) << 8) & 0xFF00) ) 

#define htons swap16
#define ntohs swap16

uint16_t rfcChecksum(uint16_t isum, const uint8_t * buffer, size_t count)
{
    register uint32_t sum = 0;
    
    const uint16_t *ptr = (uint16_t*)buffer;
    
    sum += isum;

    while(count > 1)
    {
        sum += *(ptr++);
        count -= 2;
    }

    if(count > 0)
    {
        sum +=  * (uint8_t *) (ptr);
    }

    while(sum >> 16)
    {
        sum = (sum & 0xffff) + (sum >> 16);
    }

    return ntohs(sum) ;
}
