#include <stdint.h>
#include <stdlib.h>
#include "freertos_mock.h"

uint16_t usGenerateChecksum( uint16_t usSum,
                             const uint8_t * pucNextData,
                             size_t uxByteCount )
{
    uint32_t ulAccum = FreeRTOS_htons( usSum );
    const uint16_t * pusPointer;
    const size_t uxUnrollCount = 16U;
    const uint8_t * pucData = pucNextData;
    uintptr_t uxBufferAddress;
    BaseType_t xUnaligned = pdFALSE;
    uint16_t usTerm = 0U;
    size_t uxBytesLeft = uxByteCount;

    if( uxBytesLeft >= 1U )
    {
        /* Transform a pointer to a numeric value, in order to determine
         * the alignment. */
        uxBufferAddress = ( uintptr_t ) pucData;

        if( ( uxBufferAddress & 1U ) != 0U )
        {
            ulAccum = ( ( ulAccum & 0xffU ) << 8 ) | ( ( ulAccum & 0xff00U ) >> 8 );
            usTerm = pucData[ 0 ];
            usTerm = FreeRTOS_htons( usTerm );
            /* Now make pucData 16-bit aligned. */
            uxBufferAddress++;
            /* One byte has been summed. */
            uxBytesLeft--;
            xUnaligned = pdTRUE;
        }

        /* The alignment of 'pucData' has just been tested and corrected
         * when necessary.
         */
        pusPointer = ( const uint16_t * ) uxBufferAddress;

        /* Sum 'uxUnrollCount' shorts in each loop. */
        while( uxBytesLeft >= ( sizeof( *pusPointer ) * uxUnrollCount ) )
        {
            ulAccum += pusPointer[ 0 ];
            ulAccum += pusPointer[ 1 ];
            ulAccum += pusPointer[ 2 ];
            ulAccum += pusPointer[ 3 ];
            ulAccum += pusPointer[ 4 ];
            ulAccum += pusPointer[ 5 ];
            ulAccum += pusPointer[ 6 ];
            ulAccum += pusPointer[ 7 ];
            ulAccum += pusPointer[ 8 ];
            ulAccum += pusPointer[ 9 ];
            ulAccum += pusPointer[ 10 ];
            ulAccum += pusPointer[ 11 ];
            ulAccum += pusPointer[ 12 ];
            ulAccum += pusPointer[ 13 ];
            ulAccum += pusPointer[ 14 ];
            ulAccum += pusPointer[ 15 ];

            uxBytesLeft -= sizeof( *pusPointer ) * uxUnrollCount;
            pusPointer = &( pusPointer[ uxUnrollCount ] );
        }

        /* Between 0 and 7 shorts might be left. */
        while( uxBytesLeft >= sizeof( *pusPointer ) )
        {
            ulAccum += *pusPointer;
            uxBytesLeft -= sizeof( *pusPointer );
            pusPointer = &( pusPointer[ 1 ] );
        }

        /* A single byte may be left. */
        if( uxBytesLeft == 1U )
        {
            usTerm |= ( pusPointer[ 0 ] ) & FreeRTOS_htons( ( ( uint16_t ) 0xFF00U ) );
        }

        ulAccum += usTerm;

        /* Add the carry bits. */
        while( ( ulAccum >> 16 ) != 0U )
        {
            ulAccum = ( ulAccum & 0xffffU ) + ( ulAccum >> 16 );
        }

        if( xUnaligned == pdTRUE )
        {
            /* Quite unlikely, but pucNextData might be non-aligned, which would
            * mean that a checksum is calculated starting at an odd position. */
            ulAccum = ( ( ulAccum & 0xffU ) << 8 ) | ( ( ulAccum & 0xff00U ) >> 8 );
        }
    }

    /* The high bits are all zero now. */
    return FreeRTOS_ntohs( ( uint16_t ) ulAccum );
}
