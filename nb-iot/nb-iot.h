#ifndef NB_IOT_H
#define NB_IOT_H

#include <stdint.h>
#include <stdbool.h>

// UART buffer size
#define UART_BUFFER 200

// AT commands
#define CFUN "\r\nAT+CFUN=1\r\n"
#define SET_NETWORK "\r\nAT+CGDCONT=1,\"IP\",\"eplpwa.vondafone.iot\"\r\n"
#define CONNECT "\r\nAT+CGATT=1\r\n"
#define IP_REQ "\r\nAT+CGPADDR\r\n"

// JSON format
#define JSON_FORMAT "{\"light\":%d,\"temp\":\"%d.%02d\"}"

// UART outputs
typedef enum {
    UART_BOTH,
    UART_PC,
    UART_NB
} UART_OUTPUT;

// UART status codes
typedef enum {
    UART_RECEIVING,
    UART_FINISHED,
    UART_NO_RESPONSE
} UART_STATUS;

// Globals
extern volatile char UART_data[UART_BUFFER];

/**
 * Initialize NB-IoT functions. Must be called first!
 */
void NB_IoT_Initialize();

/**
 * Send a message to one or both UART outputs.
 * @param output UART output to send to
 * @param text Message to send
 */
void UARTSend(UART_OUTPUT output, char text[]);

/**
 * Send an AT command to the NB-IoT module
 * @param command AT command to send
 * @return Status of the response
 */
UART_STATUS sendCommand(char command[]);

/**
 * Get formatted sensor data.
 * @param buffer Variable to store the sensor data in
 * @return True if successful
 */
bool getSensorData(char buffer[]);

#endif
