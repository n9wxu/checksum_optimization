#include <stdlib.h>
#include <stdint.h>
#include "freertos_mock.h"

uint16_t usGenerateChecksum32( uint16_t usSum,
                            const uint8_t * pucNextData,
                            size_t uxByteCount )
{
    uint64_t ulAccum = 0;
    const uint32_t * pBuffer32;
    const uint8_t * pBuffer8 = pucNextData;
    
    size_t uxBytesLeft = uxByteCount;
    
    uintptr_t bytes2align = 4 - ((uintptr_t)pBuffer8)&3U;
    
    if( uxBytesLeft >= 1U )
    {
        
        // It is critical to get odd-even alignment sorted here.
        // if there is an odd number of bytes to align, the word order will be odd-even
        // if there are an even number of bytes to align, the word order will be even-odd
        // this will match with the words of the buffer.
        // at the end, you need to swap the bytes depending upon the alignment.
        switch(bytes2align)
        {
            case 1:
                ulAccum = pBuffer8[0]<<8;
                break;
            case 2:
                ulAccum = pBuffer8[0] | pBuffer8[1]<<8;
                break;
            case 3:
                ulAccum = pBuffer8[0]<<8 | pBuffer8[1] | pBuffer8[2] << 8;
                break;
            case 0:
            default:
                break;
        }
        
        uxBytesLeft -= bytes2align;
        
        // set the buffer pointer to be 32 bit
        pBuffer32 = ( const uint32_t * ) (((uintptr_t) pBuffer8)+bytes2align);
        uint32_t loopCount = (uint32_t) uxBytesLeft/4;
        uxBytesLeft -= loopCount * 4;
        while( loopCount != 0 )
        {
            ulAccum += *(pBuffer32++);
            loopCount --;
        }
        // put the buffer pointer to 8-bit
        pBuffer8 = (const uint8_t *) (uintptr_t)pBuffer32;
        
        // finish off the last few bytes... could be as many as 3
        uint32_t theLastWord = 0;
        if(uxBytesLeft)
        {
            theLastWord = *(pBuffer8++);
            if(uxBytesLeft>1)
            {
                theLastWord |= (*pBuffer8++)<<8;
            }
            if(uxBytesLeft>2)
            {
                theLastWord |= (*pBuffer8++)<<16;
            }
        }
        
        ulAccum += theLastWord;
        /* Add the carry bits. */
        while( ( ulAccum >> 16 ) != 0U )
        {
            ulAccum = ( ulAccum & 0xffffU ) + ( ulAccum >> 16 );
        }
    }
    
    /* The high bits are all zero now. */
    if(bytes2align&1)
    {
        return (uint16_t) ulAccum + FreeRTOS_ntohs(usSum);
    }
    else
    {
        return FreeRTOS_ntohs( ( uint16_t ) ulAccum + usSum );
    }
}
