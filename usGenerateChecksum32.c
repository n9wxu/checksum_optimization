#include <stdlib.h>
#include <stdint.h>
#include "freertos_mock.h"

uint16_t usGenerateChecksum32( uint16_t usSum,
                            const uint8_t * pucNextData,
                            size_t uxByteCount )
{
    const uint16_t * ptr =  ( const uint16_t * ) ((uintptr_t) pucNextData);
    const uintptr_t bytes2align = (uintptr_t)(ptr)&1;
    
    size_t uxBytesLeft = uxByteCount;
    
    uint32_t ulAccum = 0;
    
    if( uxBytesLeft >= 1U )
    {
        // Read the first full word and let the compiler figure out how to read it if it is misaligned.
        // Then mask off the bits we are duplicated with reading the first ALIGNED 32-bit word.
        if(bytes2align!=0)
        {
            ulAccum = *pucNextData << 8;
            ptr = ( const uint16_t * ) ((uintptr_t) &pucNextData[1]);
            uxBytesLeft --;
        }

        while(uxBytesLeft>=32)
        {
            ulAccum += ptr[ 0 ];
            ulAccum += ptr[ 1 ];
            ulAccum += ptr[ 2 ];
            ulAccum += ptr[ 3 ];
            ulAccum += ptr[ 4 ];
            ulAccum += ptr[ 5 ];
            ulAccum += ptr[ 6 ];
            ulAccum += ptr[ 7 ];
            ulAccum += ptr[ 8 ];
            ulAccum += ptr[ 9 ];
            ulAccum += ptr[ 10 ];
            ulAccum += ptr[ 11 ];
            ulAccum += ptr[ 12 ];
            ulAccum += ptr[ 13 ];
            ulAccum += ptr[ 14 ];
            ulAccum += ptr[ 15 ];
            uxBytesLeft -= 32;
        }
        
        while(uxBytesLeft>1)
        {
            ulAccum += *(ptr++);
            uxBytesLeft -= 2;
        }

        if(uxBytesLeft)
        {
            ulAccum += *((const uint8_t *) (uintptr_t)ptr);
        }
        
        // If we started off on an ODD alignment, then swap the accumulator to match the first word.
        // clear the duplicate byte from the first word
        if(bytes2align != 0)
        {
            ulAccum <<= 8;
        }
        
        // bring in the initial value
        ulAccum += usSum;
        
        /* Add the carry bits. */
        ulAccum = (ulAccum & 0x0000FFFF) + (ulAccum >> 16);
        ulAccum = (ulAccum & 0x0000FFFF) + (ulAccum >> 16);
        ulAccum = (ulAccum & 0x0000FFFF) + (ulAccum >> 16);

    }
    
    return FreeRTOS_ntohs(ulAccum);
}
