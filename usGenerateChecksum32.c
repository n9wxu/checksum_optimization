#include <stdlib.h>
#include <stdint.h>
#include "freertos_mock.h"

uint16_t usGenerateChecksum32( uint16_t usSum,
                            const uint8_t * pucNextData,
                            size_t uxByteCount )
{
    const uint32_t masks[] = {0, 0x000000FF, 0x0000FFFF, 0x00FFFFFF , 0xFFFFFFFF};
    const uint8_t * pBuffer8 = pucNextData;
    
    // the first aligned 32-bit word is when the least 2 bits are 0.  So masked them off and add 4 to get the first full word.
    // This might get the second full word if the buffer happened to be aligned to start with.  That is OK, we will read the first
    // word seperately.
    // The difference between this address and the first address is the # of bytes required to achieve alignment
    // Read the first unaligned 32-bits and then mask it to prevent duplicates
    const uint32_t * const pBuffer32_begin =  ( const uint32_t * ) (((uintptr_t) pucNextData & ~3) + 4);
    const uint32_t * const pBuffer32_end =  (const uint32_t *)((uintptr_t)&pucNextData[uxByteCount]&~3);
    const uintptr_t bytes2align = (uintptr_t)pBuffer32_begin - (uintptr_t)pucNextData;
    
    uint16_t result16=0;
    
    uint64_t ulAccum = 0;
    size_t uxBytesLeft = uxByteCount;
    
    uint32_t theFirstWord = 0;
    
    if( uxBytesLeft >= 8U )
    {
        // Read the first full word and let the compiler figure out how to read it if it is misaligned.
        // Then mask off the bits we are duplicated with reading the first ALIGNED 32-bit word.
        theFirstWord = *(uint32_t *)pBuffer8;
        theFirstWord &= masks[bytes2align];

        uxBytesLeft -= bytes2align;
        
        // set the buffer pointer to be 32 bit
        uxBytesLeft -= (pBuffer32_end - pBuffer32_begin) * 4;
        for(const uint32_t *ptr = pBuffer32_begin; ptr != pBuffer32_end; ptr++)
        {
            ulAccum += *ptr;
        }
        // put the buffer pointer to 8-bit
        pBuffer8 = (const uint8_t *) (uintptr_t)pBuffer32_end;
        
        // finish off the last few bytes... could be as many as 3
        // these last bytes are aligned with the bulk of the data.  No further alignment is required.
        if(uxBytesLeft > 1)
        {
            ulAccum += *((uint16_t*)(pBuffer8++));
            pBuffer8++;
            uxBytesLeft -= 2;
        }
        if(uxBytesLeft)
        {
            ulAccum += *pBuffer8;
        }

        // If we started off on an ODD alignment, then swap the accumulator to match the first word.
        if((bytes2align&1) != 0)
        {
            ulAccum = (ulAccum & 0xFF00FF00FF00FF00)>>8 | (ulAccum & 0x00FF00FF00FF00FF)<<8;
        }
        
        // bring in the first word
        ulAccum += theFirstWord + usSum;
        
        /* Add the carry bits. */
        // this should be the worst case number of carry's added in
        ulAccum = (ulAccum & 0x000000FFFFFFFFU) + (ulAccum >> 32);
        ulAccum = (ulAccum & 0x000000FFFFFFFFU) + (ulAccum >> 32);
        uint32_t result32 = (ulAccum & 0x000000FFFFFFFFU) + (ulAccum >> 32);
        result32 = (result32 & 0x0000FFFF) + (result32 >> 16);
        result32 = (uint16_t) (result32 & 0x0000FFFF) + (result32 >> 16);
        result16 = (uint16_t) (result32 & 0x0000FFFF) + (result32 >> 16);
        
    }
    else // TODO: Do we need to do something for < 8-byte packet?
    {
    // empty else
    }
    
    return FreeRTOS_ntohs(result16);
}
