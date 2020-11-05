#include "mcc_generated_files/system.h"
#include "mcc_generated_files/interrupt_manager.h"
#include <xc.h>
#include <stdio.h>

#define FCY 8000000UL
#include <libpic30.h>

#include "nb-iot/nb-iot.h"

int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    INTERRUPT_GlobalEnable();
    
    NB_IoT_Initialize();
    
    __delay_ms(1000);
    UARTSend(UART_PC, "\r\nStarting...\r\n");
    __delay_ms(2000);
    
    static char sensor[70];
    
    while (1) {
        getSensorData(sensor);
        
        UARTSend(UART_PC, sensor);
        
        __delay_ms(2000);
    }

    return 1;
}
