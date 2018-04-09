/*
 * radio_lvl1.h
 *
 *  Created on: Nov 17, 2013
 *      Author: kreyl
 */

#pragma once

#include "kl_lib.h"
#include "ch.h"
#include "cc2500.h"
#include "shell.h"

#if 1 // =========================== Pkt_t =====================================
struct rPkt_t  {
    int16_t DtctrID;
    int16_t FrflyID;
} __packed;

#define RPKT_LEN    sizeof(rPkt_t)

#define TEST_WORD   0xCa115ea1  // Call Seal
#endif

#if 1 // ======================= Channels & cycles =============================
#define RCHNL_SRV       0
#define ID2RCHNL(ID)    (RCHNL_MIN + ID)
#define RCHNL           7

#define ID_FRFLY_MIN    10
#define ID_FRFLY_MAX    40

#endif

#if 1 // =========================== Timings ===================================
#define RX_T_MS                 180      // pkt duration at 10k is around 12 ms
#define RX_SLEEP_T_MS           810
#define MIN_SLEEP_DURATION_MS   18
#define RETRY_CNT               4

#endif

#if 1 // ============================= RX Table ================================
#define RXTABLE_SZ              (ID_FRFLY_MAX - ID_FRFLY_MIN)
struct Firefly_t {
    int32_t ID;
    int32_t Rssi;
    int32_t Cnt;
    Firefly_t& operator = (const Firefly_t &Right) {
        ID = Right.ID;
        Rssi = Right.Rssi;
        Cnt = Right.Cnt;
        return *this;
    }
    void CalcAverage() { Rssi = Rssi / Cnt; }
} __packed;

class RxTable_t {
private:
public:
    Firefly_t Firefly[RXTABLE_SZ];
    void Add(uint8_t ID, int8_t Rssi) {
        if(ID < ID_FRFLY_MIN or ID > ID_FRFLY_MAX) return;
        int indx = ID - ID_FRFLY_MIN;
        Firefly[indx].Rssi += Rssi;
        Firefly[indx].Cnt++;
//        Printf("%u; %d\r", ID, Rssi);
    }

    void Clear() {
        for(int i = 0; i<RXTABLE_SZ; i++) {
            Firefly[i].ID = i + ID_FRFLY_MIN;
            Firefly[i].Rssi = 0;
            Firefly[i].Cnt = 0;
        }
    }

//    void Print() {
//        for(int i=0; i<RXTABLE_SZ; i++) {
//            Printf("ID: %u; Rssi: %d\r", i+ID_FRFLY_MIN, RssiBuf[i]);
//        }
//    }
};
#endif

class rLevel1_t {
private:
public:
    RxTable_t RxTable;
    uint8_t Init();
    rPkt_t Pkt;
    // Inner use
    void ITask();
};

extern rLevel1_t Radio;
