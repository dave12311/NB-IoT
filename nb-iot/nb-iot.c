#define FCY 8000000UL
#include <libpic30.h>

#include "nb-iot.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "i2c/i2c_simple_master.h"

#include "../mcc_generated_files/uart1.h"
#include "../mcc_generated_files/uart2.h"
#include "../mcc_generated_files/tmr1.h"
#include "../mcc_generated_files/tmr2.h"
#include "../mcc_generated_files/interrupt_manager.h"
#include "../mcc_generated_files/adc1.h"

volatile char UART_data[UART_BUFFER];
volatile uint8_t UART_counter = 0;
volatile UART_STATUS UART_status = UART_RECEIVING;

// Send messages through UART
void UARTSend(UART_OUTPUT output, char text[]) {
    if (output == 0) {
        UARTSend(1, text);
        UARTSend(2, text);
    } else {
        __C30_UART = output;
        printf(text);
    }
}

// Incoming UART message ISR
static void UART_ISR() {
    if (UART2_IsRxReady()) {
        // Read incoming byte
        UART_data[UART_counter] = UART2_Read();
        UART_counter++;

        // Stop and reset response timeout
        TMR2_Stop();
        TMR2_Counter16BitSet(0);

        // Reset timeout
        TMR1_Counter16BitSet(0);
        TMR1_Start();
    }
}

// Symbol timeout ISR

static void UART_Symbol_Timeout() {
    // Stop and reset timer
    TMR1_Stop();
    TMR1_Counter16BitSet(0);

    UART_status = UART_FINISHED;
    UART_counter = 0;
}

// Response timeout ISR

static void UART_Response_Timeout() {
    // Stop and reset timer
    TMR2_Stop();
    TMR2_Counter16BitSet(0);

    UART_status = UART_NO_RESPONSE;
}

void NB_IoT_Initialize() {
    UART2_SetRxInterruptHandler(UART_ISR);
    TMR1_SetInterruptHandler(UART_Symbol_Timeout);
    TMR2_SetInterruptHandler(UART_Response_Timeout);
}

// Send AT command and wait for response
// Returns the status of the UART response
// Message in UART_data
UART_STATUS sendCommand(char command[]) {
    uint8_t status;

    // Send AT command
    UARTSend(UART_BOTH, command);

    // Start response timeout
    TMR2_Start();

    // Wait for done
    while (UART_status == UART_RECEIVING) {
    }

    // Send DEBUG data
    UARTSend(UART_PC, UART_data);
    UARTSend(UART_PC, "\r\n");

    // Reset
    status = UART_status;
    UART_status = UART_RECEIVING;
    UART_counter = 0;

    // Reset buffer
    uint8_t i;
    for (i = 0; i < UART_BUFFER; i++) {
        if (UART_data[i] == 0) {
            break;
        }

        UART_data[i] = 0;
    }

    return status;
}

static uint16_t getLightValue() {
    return ADC1_GetConversion(0x8);
}

static int16_t getTempValue() {
    int32_t temperature = 0;
    
    temperature = i2c_read2ByteRegister(0x18, 0x05);
    
    temperature = temperature << 19;
    temperature = temperature >> 19;
    
    temperature *= 100;
    temperature /= 16;
    
    return temperature;
}

bool getSensorData(char buffer[]) {
    uint16_t light = getLightValue();
    int16_t temp = getTempValue();
    
    uint8_t len = sprintf(buffer, JSON_FORMAT, light,temp/100,abs(temp)%100);
    
    if(len > 0) {
        return true;
    } else {
        return false;
    }
}
