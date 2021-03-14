#include <stdlib.h>
#include <stdint.h>
#include "freertos_mock.h"

uint16_t usGenerateChecksum32( uint16_t usSum,
                            const uint8_t * pucNextData,
                            size_t uxByteCount )
{
    const uint32_t masks[] = {0x00000000,0x000000FF, 0x0000FFFF, 0x00FFFFFF };
    const uint8_t * pBuffer8 = pucNextData;
    const uint8_t * pBuffer8_lastByte = &pucNextData[uxByteCount];
    const uintptr_t bytes2align = 4 - ((uintptr_t)pucNextData)&3U;
    const uint32_t loopCount = (uint32_t) (uxByteCount - bytes2align)/4;
    const uint32_t * const pBuffer32_begin =  ( const uint32_t * ) (((uintptr_t) pucNextData) + bytes2align);
    const uint32_t * const pBuffer32_end =  &pBuffer32_begin[loopCount];
    
    uint64_t ulAccum = FreeRTOS_ntohs(usSum);
    size_t uxBytesLeft = uxByteCount;
    
    if( uxBytesLeft >= 1U )
    {
        
        // It is critical to get odd-even alignment sorted here.
        // if there is an odd number of bytes to align, the word order will be odd-even
        // if there are an even number of bytes to align, the word order will be even-odd
        // this will match with the words of the buffer.
        // at the end, you need to swap the bytes depending upon the alignment.
        // Strategy:
        // Grab the first 3 bytes, Mask out the ones that I don't need, swap odd/even if required
        ulAccum += pBuffer8[0] | pBuffer8[1] << 8 | pBuffer8[2] << 16;
        ulAccum &= masks[bytes2align];
        ulAccum = bytes2align&1?(ulAccum & 0xFF00FF00)>>8 | (ulAccum & 0x00FF00FF)<<8:ulAccum;
        
        uxBytesLeft -= bytes2align;
        
        // set the buffer pointer to be 32 bit
        uxBytesLeft -= loopCount * 4;
        for(const uint32_t *ptr = pBuffer32_begin; ptr != pBuffer32_end; ptr++)
        {
            ulAccum += *ptr;
        }
        // put the buffer pointer to 8-bit
        pBuffer8 = (const uint8_t *) (uintptr_t)pBuffer32_end;
        
        // finish off the last few bytes... could be as many as 3

        uint32_t theLastWord = pBuffer8_lastByte[-1] | pBuffer8_lastByte[-2]<<8 | pBuffer8_lastByte[-3]<<16;
        theLastWord &= masks[uxBytesLeft];
        theLastWord = !(uxBytesLeft&1)?(theLastWord & 0xFF00FF00)>>8 | (theLastWord & 0x00FF00FF)<<8:theLastWord;

        ulAccum += theLastWord;
        /* Add the carry bits. */
        while( ( ulAccum >> 16 ) != 0U )
        {
            ulAccum = ( ulAccum & 0xffffU ) + ( ulAccum >> 16 );
        }
    }
    
    return bytes2align&1?(uint16_t)ulAccum:FreeRTOS_ntohs((uint16_t)ulAccum);
}

