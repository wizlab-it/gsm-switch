/**
 * @package Caldaia
 * @desc a6lib.c
 * @author WizLab.it
 * @version 20220924.001
 * @processor: PIC16F1825
 */

#include "A6Lib.h"

uint8_t A6_Init(const uint32_t baudRate) {
    A6_BaudRate_AutoDetect();
    A6_BaudRate_Set(baudRate);
    A6_Command("AT+COPS=3,0\r\n", 0, NULL, 0);
    return A6_IsAlive();
}

void A6_Command(const uint8_t *command, int16_t timeout, uint8_t *response, uint8_t responseLen) {
    A6_LAST_COMMAND_MILLISECONDS += A6_NEXT_COMMAND_WAIT_TIME;
    while(MILLISECONDS < A6_LAST_COMMAND_MILLISECONDS);
    A6_LAST_COMMAND_MILLISECONDS = MILLISECONDS;

    EUSART_Flush();
    EUSART_SendByteArray(command, strlen(command));
    if(responseLen != 0) {
        A6_ReadLine(response, responseLen, timeout);
        EUSART_Trim(response);
    }
    return;
}

uint8_t A6_ReadLine(uint8_t *line, uint8_t len, int16_t timeout) {
    int16_t c;
    line[0] = 0x00;

    //Skip initial CRLF
    while(1) {
        c = EUSART_PeekByte(timeout);
        if(c == EUSART_RX_NODATA) return 0;
        if((c == '\r') || (c == '\n')) {
            EUSART_ReadByte(timeout);
        } else {
            break;
        }
    }

    //Read line
    return EUSART_ReadLine(line, len, timeout);
}

uint8_t A6_IsAlive(void) {
    char response[32];
    A6_Command("AT\r\n", EUSART_USE_DEFAULT_TIMEOUT, response, 32);
    if(strcmp(response, "OK") == 0) {
        return 1;
    }
    return 0;
}

void A6_Process_Random_Comms(void) {
    //Nothing on bus
    if(EUSART_Available(EUSART_USE_NO_TIMEOUT) == 0) return;

    //Get data
    char response[32];
    uint8_t cnt = A6_ReadLine(response, 32, 0);
    if(cnt == 0) return;
    EUSART_Trim(response);

    //Nothing serious
    if(response[0] == 0x00) return;

    //Intercept generic OK
    if(strcmp(response, "OK") == 0) return;

    //Intercept rings    
    if(strcmp(response, "RING") == 0) {
        GSMStatus.ringLastMilliseconds = MILLISECONDS;
        if(GSMStatus.isRinging == 0) {
            GSMStatus.ringFirstMilliseconds = MILLISECONDS;
        }
        GSMStatus.isRinging = 1;
        return;
    }
}

uint32_t A6_BaudRate_Get(void) {
    uint32_t baudRate;
    char *baudRateToken;
    char response[32];

    A6_Command("AT+IPR?\r\n", 0, response, 32);
    if(strstr(response, "+IPR: ") != NULL) {
        baudRateToken = strtok(response, " ");
        baudRateToken = strtok(NULL, " ");
        if(baudRateToken != NULL) {
            baudRate = atol(baudRateToken);
            for(uint8_t j=0; j<3; j++) {
                if(baudRate == A6_BAUDRATES[j]) {
                    return baudRate;
                }
            }
        }
    }

    return 0;
}

uint32_t A6_BaudRate_Set(const uint32_t baudRate) {
    char request[20];
    char response[32];
    uint32_t currentBaudRate = A6_BaudRate_Get();

    //If unable to detect current baud rate then exit
    if(currentBaudRate == 0) return 0;

    //Send IPR Command
    sprintf(request, "AT+IPR=%lu\r\n", baudRate);
    A6_Command(request, 0, response, 32);
    sleepMS(250);
    if(strcmp(response, "OK") == 0) {
        //Set new baud rate on EUSART
        EUSART_BaudRateSet(baudRate);
        sleepMS(150);
        if(A6_IsAlive() == 1) {
            return baudRate;
        }
    }

    //If here, new baud rate has not been set, restore EUSART original baud rate and return error
    EUSART_BaudRateSet(currentBaudRate);
    return 0;
}

uint32_t A6_BaudRate_AutoDetect(void) {
    for(uint8_t i=0; i<3; i++) {
        EUSART_BaudRateSet(A6_BAUDRATES[i]);
        if(A6_IsAlive() == 1) {
            return A6_BAUDRATES[i];
        }
        sleepMS(100);
    }

    return 0;
}

uint8_t A6_Network_Status(void) {
    char *networkStatusToken;
    char response[32];

    A6_Command("AT+CREG?\r\n", 0, response, 32);
    if(strstr(response, "+CREG: ") != NULL) {
        networkStatusToken = strtok(response, " ");
        networkStatusToken = strtok(NULL, " ");
        if(networkStatusToken != NULL) {
            networkStatusToken = strtok(networkStatusToken, ",");
            networkStatusToken = strtok(NULL, ",");
            return atoi(networkStatusToken);
        }
    }

    return A6_NETWORK_STATUS_UNKNOWN;
}

uint8_t A6_Network_IsConnected(void) {
    uint8_t ns = A6_Network_Status();
    if((ns == 1) || (ns == 5)) return 1;
    return 0;
}