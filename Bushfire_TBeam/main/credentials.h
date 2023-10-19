/*

Credentials file

*/

#pragma once

// Only one of these settings must be defined
#define USE_ABP
//#define USE_OTAA

#ifdef USE_ABP

    // LoRaWAN NwkSKey, network session key
    static const u1_t PROGMEM NWKSKEY[16] = { 0x58, 0xD9, 0xA4, 0x4F, 0x0B, 0x27, 0x13, 0x4A, 0x8A, 0x0F, 0xC8, 0x5B, 0x3E, 0xEE, 0x42, 0xF9 };   
    // LoRaWAN AppSKey, application session key
    static const u1_t PROGMEM APPSKEY[16] = { 0x80, 0xEE, 0x80, 0x8A, 0x40, 0x9B, 0xB4, 0x01, 0xBC, 0x22, 0xC0, 0x22, 0xBA, 0x9C, 0x00, 0x7D };
    // LoRaWAN end-device address (DevAddr)
    // This has to be unique for every node
    static const u4_t DEVADDR = 0x260DE20A;

#endif

