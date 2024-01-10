/**
 * @package Caldaia
 * @desc defs.h
 * @author WizLab.it
 * @version 20220924.004
 * @processor: PIC16F1825
 */

#ifndef DEFS_H
#define DEFS_H

#include    <xc.h>
#include    <stdint.h>
#include    <string.h>
#include    <stdio.h>
#include    "EUSART.h"
#include    "A6Lib.h"

#define _XTAL_FREQ 32000000      //XTAL Frequency

#define _CLOCK_TICKS_PER_SECOND 126
#define _CALDAIA_TIMEOUT 15 * 60    //15 miunutes

#define OPERATOR_NAME_SIZE              32
#define OPERATOR_NAME_TOP_MARGIN        8
#define OPERATOR_NAME_RIGHT_MARGIN      110
#define OPERATOR_NAME_REFRESH_SHORT     2510
#define OPERATOR_NAME_REFRESH_LONG      37190
#define OPERATOR_RSSI_REFRESH           4740
#define OPERATOR_GPRS_REFRESH           5920

#define _LED_NETLINK    PORTAbits.RA5   //Led NetLink
#define _LED_RING       PORTCbits.RC2   //Led Ring
#define _LED_CALDAIA    PORTCbits.RC1   //Led Caldaia

volatile struct {
    unsigned unused : 7;          //Unused bits
    unsigned timer1sec : 1;       //1 second flag
} Flags;

struct {
    uint8_t gsmModuleStatus;
    uint32_t nextOperatorName;
    uint32_t nextRSSI;
    uint32_t nextGPRSStatus;
    char operatorName[OPERATOR_NAME_SIZE];
    char operatorNameLastFirstChar;
    uint8_t operatorRSSILevel;
    uint8_t operatorGPRSIsConnected;
    uint32_t ringFirstMilliseconds;
    uint32_t ringLastMilliseconds;
    uint8_t isRinging;
} GSMStatus;

extern uint32_t MILLISECONDS;

extern void sleepMS(uint32_t ms);

#endif