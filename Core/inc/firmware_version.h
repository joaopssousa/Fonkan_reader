#ifndef FIRMWARE_VERSION_H
#define FIRMWARE_VERSION_H


#define MAJOR_FIRMWARE_VERSION 0x01
#define MINOR_FIRMWARE_VERSION 0x00
#define PATCH_FIRMWARE_VERSION 0x07

#ifdef TEST
#define PREFIX 0xFF
#else
#define PREFIX 0x00
#endif

/*
* Device Type List
* WeatherStation 0x01
* Curral 0x02
* Portal 0x03
* Eletrificador 0x04
*/
#define CURRAL 0x02

#define DEVICE_TYPE CURRAL

#endif
