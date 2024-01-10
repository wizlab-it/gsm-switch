/**
 * @package Caldaia
 * @desc main.h
 * @author WizLab.it
 * @version 20220924.002
 * @processor: PIC16F1825
 */

#include "defs.h"
#include "init.h"

/*==============================================================================
 * Main routine
 *  - Init system
 *  - Loop forever
 *============================================================================*/
void main(void) {
    init();
    EUSART_Init();
    GSMStatus.gsmModuleStatus = A6_Init(115200);
    while(1) loop();
}

/*==============================================================================
 * Loop routine
 *============================================================================*/
void loop(void) {
    //Process EUSART Comms from GSM Module
    A6_Process_Random_Comms();

    //Is there an incoming call?
    if(GSMStatus.isRinging == 1) {
        _LED_RING = 1;
        if((MILLISECONDS - GSMStatus.ringLastMilliseconds) > 3000) {
            GSMStatus.isRinging = 0;
        }
        if((MILLISECONDS - GSMStatus.ringFirstMilliseconds) > 6000) {
            GSMStatus.isRinging = 0;
            A6_Command("ATH\r\n", 0, NULL, 0);
            _LED_CALDAIA = 1;
            __COUNTDOWN_CALDAIA = _CALDAIA_TIMEOUT;
        }
    } else {
        _LED_RING = 0;
    }

    //Cron: 1 second
    if(Flags.timer1sec == 1) {
        Flags.timer1sec = 0;

        //Show network status
        if(A6_Network_IsConnected() == 1) {
            _LED_NETLINK = 1;
        } else {
            _LED_NETLINK = 0;
        }

        //Handle caldaia relay
        if(_LED_CALDAIA == 1) {
            __COUNTDOWN_CALDAIA--;
            if(__COUNTDOWN_CALDAIA == 0) {
                _LED_CALDAIA = 0;
            }
        }
    }
}