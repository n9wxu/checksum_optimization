#ifndef _FREERTOS_MOCK_H
#define _FREERTOS_MOCK_H

#define swap16(data) ( (((data) >> 8) & 0x00FF) | (((data) << 8) & 0xFF00) ) 

#define htons swap16
#define ntohs swap16

#define pdTRUE 1
#define pdFALSE 0
#define FreeRTOS_htons htons
#define FreeRTOS_ntohs ntohs
typedef int BaseType_t;

#endif
