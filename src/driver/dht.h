#ifndef __DHT_H__
#define __DHT_H__

#define DHT_PIN   NRF_GPIO_PIN_MAP(1,10)

#include <stdint.h>

    typedef enum DHTErrorCode 
    {
        DHT_SUCCESS,
        DHT_FAIL,
    } DHT22ErrorCode;

    typedef enum DHTModel 
    {
        DHT11,
        DHT22,
    } DHTModel;

    struct DHTReadings
    {
        int temperatureIntPart;
        int temperatureDecimalPart;
        int humidityIntPart;
        int humidityDecimalPart;
        DHTModel dhtModel;
    };

    void dhtRead(struct DHTReadings* dht);

#endif