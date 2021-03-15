#include <stdlib.h>
#include <stdint.h>
#include "freertos_mock.h"

uint16_t usGenerateChecksum32( uint16_t usSum,
                            const uint8_t * pucNextData,
                            size_t uxByteCount )
{
    const uint16_t * ptr =  ( const uint16_t * ) ((uintptr_t) pucNextData);
    const uintptr_t notAligned = (uintptr_t)(ptr)&1;
    
    size_t uxBytesLeft = uxByteCount;
    
    uint32_t ulAccum = 0;
    
    if( uxBytesLeft >= 1U )
    {
        if(notAligned!=0)
        {
            ulAccum = ((uint32_t)*pucNextData) << 8;
            ptr = ( const uint16_t * ) ((uintptr_t) &pucNextData[1]);
            uxBytesLeft --;
        }
        
        #define UNROLL 16
        while(uxBytesLeft>=UNROLL*2)
        {
            for(int x = 0; x < UNROLL; x++)
            ulAccum += ptr[ x ];
            ptr = &ptr[UNROLL];
            uxBytesLeft -= UNROLL*2;
        }
        
        while(uxBytesLeft > 1)
        {
            ulAccum += *(ptr++);
            uxBytesLeft -= 2;
        }
        
        if(uxBytesLeft)
        {
            ulAccum += *((const uint8_t *) (uintptr_t)ptr);
        }
        
        if(notAligned != 0)
        {
            ulAccum <<= 8;
        }
        
        // bring in the initial value
        ulAccum += usSum;
        
        /* Add the carry bits. */
        ulAccum = (ulAccum & 0x0000FFFF) + (ulAccum >> 16);
        ulAccum = (ulAccum & 0x0000FFFF) + (ulAccum >> 16);
        ulAccum = (ulAccum & 0x0000FFFF) + (ulAccum >> 16);
        ulAccum = FreeRTOS_ntohs(ulAccum);
    }
    
    return ulAccum;
}
