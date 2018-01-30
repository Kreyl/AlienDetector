/*
 * main.h
 *
 *  Created on: 28.01.2012
 *      Author: kreyl
 */

#ifndef MAIN_H_
#define MAIN_H_

#define NOSIGNAL_DELAY  999

#define MIN_RSSI_TO_DISPLAY     18  // Set between 0 and 50

#define MIN_ADDRESS     1
#define MAX_ADDRESS     7
#define ALIEN_COUNT     (MAX_ADDRESS - MIN_ADDRESS + 1)

struct Alien_t {
    uint32_t Timer;
    bool New, Exists;
    int32_t RSSI;
};

class Signal_t {
private:
    Alien_t Alien[ALIEN_COUNT];
public:
    void Init(void);
    void Task(void);
    void Remember(uint8_t AAddress, int32_t RawRSSI);
};

void EVENT_NewPacket(void);
void EVENT_NewBatteryState(void);
void GeneralInit(void);

#endif /* MAIN_H_ */
