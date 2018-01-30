/*
 * File:   main.cpp
 * Author: Kreyl
 *
 * Created on May 27, 2011, 6:37 PM
 */

#include "stm32f10x.h"
#include "delay_util.h"
#include "kl_util.h"
#include "cc2500.h"
#include "beep.h"
#include "lcd110x.h"
#include "images.h"
#include "main.h"
#include "bcklt.h"
#include "adc.h"

Signal_t Signal;

// Sounds
BeepSnd_t IdleBeep = {
        2,
        {
            {880, 9, 100},
            {0,   0, 1503},
        }
};
BeepSnd_t AlienBeep = {
        6,
        {
            {1800,  7,  150},   // On
            {0,     0,   35},   // Off
            {1800, 18,  170},   // On
            {0,     0,   35},   // Off
            {1800,  7,  114},   // On
            {0,     0,  500},   // Off
        }
};


#define CC_ADDRESS 207

int main(void) {
    GeneralInit();

    while (1) {
        Lcd.Task();
        CC.Task();
        Beep.Task();
        Signal.Task();
        Battery.Task();
    } // while 1
}

void GeneralInit(void) {
    Delay.Init();
    Delay.ms(63);
    UART_Init();

    Bcklt.Init();
    Bcklt.On(100);

    Lcd.Init();
    Lcd.DrawImage(0, 0, icon_WYTiny);
    Lcd.DrawImage(32, 0, icon_motiontrack);

    Battery.Init();
    Battery.State = bsFull;
    EVENT_NewBatteryState();
    Battery.EvtNewState = EVENT_NewBatteryState;

    CC.Init();
    CC.SetChannel(CC_CHNL);
    CC.SetAddress(CC_ADDRESS);
    // TX pkt
    CC.TX_Pkt.From = CC_ADDRESS;

    Beep.Init();
    Beep.SetSound(&IdleBeep);

    Signal.Init();

    klPrintf("\rDetector\r");
}

// ================================= CC task ===================================
/*
 * Both TX and RX are interrupt-driven, so IRQ enabled at init and commented out in EnterRX.
 */
#define RX_WAIT_TIME    2
enum SearchState_t {IsCalling, IsWaiting} SearchState;
uint8_t PktCounter=0;

void CC_t::Task(void) {
    // Proceed with state processing
    GetState();
    switch (State) {
        case CC_STB_RX_OVF:
            klPrintf("RX ovf\r");
            FlushRxFIFO();
            break;
        case CC_STB_TX_UNDF:
            klPrintf("TX undf\r");
            FlushTxFIFO();
            break;

        case CC_STB_IDLE:
            if (SearchState == IsCalling) { // Call alien
                WriteTX();
                //klPrintf("TX: %u\r", TX_Pkt.To);
                EnterTX();
            }
            else {  // Is waiting
                if (Delay.Elapsed(&Timer, RX_WAIT_TIME)) SearchState = IsCalling;
                else EnterRX();
            }
            break;

        case CC_STB_RX:
            if (Delay.Elapsed(&Timer, RX_WAIT_TIME)) {
                SearchState = IsCalling;
                EnterIdle();
            }
            break;

        default: // Just get out in other cases
            //klPrintf("Other: %X\r", State);
            break;
    } //Switch
}

void CC_t::IRQHandler() {
    if (SearchState == IsCalling) { // Packet transmitted, enter RX
        if(++PktCounter == 1) {     // Several packets sent
            PktCounter = 0;
            // Increase packet address
            TX_Pkt.To++;
            if (TX_Pkt.To > MAX_ADDRESS) TX_Pkt.To = MIN_ADDRESS;
            // Switch to waiting state
            SearchState = IsWaiting;
            Delay.Reset(&Timer);
        }
    }
    else { // Will be here if packet received successfully or in case of wrong address
        if (ReadRX()) {
            // Check address
            if(RX_Pkt.To == CC_ADDRESS) {   // This packet is ours
                //klPrintf("From: %u; RSSI: %u\r", RX_Pkt.From, RX_Pkt.RSSI);
                Signal.Remember(RX_Pkt.From, RX_Pkt.RSSI);
            }
            FlushRxFIFO();
        } // if size>0
    } // if is waiting
}

// =============================== Events ======================================
void EVENT_NewBatteryState(void) {
    switch (Battery.State) {
        case bsFull:  Lcd.DrawImage(80, 0, icon_BatteryFull);  break;
        case bsHalf:  Lcd.DrawImage(80, 0, icon_BatteryHalf);  break;
        case bsEmpty: Lcd.DrawImage(80, 0, icon_BatteryEmpty); break;
    }
}

// ============================= Signal_t ======================================
void Signal_t::Task() {
    bool IsIdle = true;
    Alien_t *Al;
    // Check all channels if displaying needed
    for (uint8_t i=0; i<ALIEN_COUNT; i++) {
        Al = &Alien[i];
        if (Al->New) {
            Al->New = false;
            Lcd.DrawPeak(i, Al->RSSI);
            Beep.SetSound(&AlienBeep);
            Al->Exists = true;
        } // if new
        else if (Delay.Elapsed(&Al->Timer, 999)) {  // not new and timeout passed
            Lcd.DrawPeak(i, 0); // Clear peak
            Al->Exists = false;
        }
        // Check if set Idle sound
        if (Al->Exists) IsIdle = false;
    } // for

    // Set idle sound if needed
    if (IsIdle) Beep.SetSound(&IdleBeep);
}

void Signal_t::Remember(uint8_t AAddress, int32_t RawRSSI) {
    // Calculate RSSI
    if (RawRSSI >= 128) RawRSSI -= 256;
    RawRSSI = (RawRSSI / 2) - 69;    // now it is in dBm
    // Scale to display
    RawRSSI += 90;
    if (RawRSSI < 0) RawRSSI = 1;
    if (RawRSSI > 50) RawRSSI = 50;
    // Check if display
    Alien_t *Al = &Alien[AAddress - MIN_ADDRESS];
    //if (RawRSSI > MIN_RSSI_TO_DISPLAY) {
        Al->New = true;
        Delay.Reset(&Al->Timer);
        Al->RSSI = RawRSSI;
    //}
}

void Signal_t::Init() {
    for(uint8_t i=0; i<8; i++) {
        Lcd.DrawPeak(i, 0);
        Alien[i].New = false;
    }
}
