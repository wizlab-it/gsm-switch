/**
 * @package Caldaia
 * @desc a6lib.c
 * @author WizLab.it
 * @version 20220924.001
 * @processor: PIC16F1825
 */

#ifndef A6LIB_H
#define	A6LIB_H

#include "defs.h"

#define A6_NEXT_COMMAND_WAIT_TIME       200

#define	A6_NETWORK_STATUS_IDLE          0
#define	A6_NETWORK_STATUS_REGISTERED    1
#define	A6_NETWORK_STATUS_SEARCHING     2
#define	A6_NETWORK_STATUS_DENIED        3
#define	A6_NETWORK_STATUS_UNKNOWN       4
#define	A6_NETWORK_STATUS_ROAMING       5

#define A6_SIM_A6_ERROR     0
#define A6_SIM_UNKNOWN      1
#define A6_SIM_MISSING      2
#define A6_SIM_READY        3
#define A6_SIM_PIN          4

const uint32_t A6_BAUDRATES[] = { 9600, 57600, 115200 };
uint32_t A6_LAST_COMMAND_MILLISECONDS = 0;

uint8_t A6_Init(const uint32_t baudRate);
void A6_Command(const uint8_t *command, int16_t timeout, uint8_t *response, uint8_t responseLen);
uint8_t A6_ReadLine(uint8_t *line, uint8_t len, int16_t timeout);
uint8_t A6_IsAlive(void);
void A6_Process_Random_Comms(void);

uint32_t A6_BaudRate_Get(void);
uint32_t A6_BaudRate_Set(const uint32_t baudRate);
uint32_t A6_BaudRate_AutoDetect(void);

uint8_t A6_Network_Status(void);
uint8_t A6_Network_IsConnected(void);

#endif