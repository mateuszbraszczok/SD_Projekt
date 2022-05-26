#ifndef __DHT_H__
#define __DHT_H__

#define DHT_PIN   NRF_GPIO_PIN_MAP(1,11)

#include <stdint.h>

    typedef enum DHTErrorCode 
    {
        DHT_SUCCESS,
        DHT_FAIL,
    } DHTErrorCode;

    typedef enum DHTModel 
    {
        DHT11,
        DHT22,
    } DHTModel;

    struct DHTReadings
    {
        uint8_t temperatureIntPart;
        uint8_t temperatureDecimalPart;
        uint8_t humidityIntPart;
        uint8_t humidityDecimalPart;
        DHTModel dhtModel;
    };

    DHTErrorCode dhtRead(struct DHTReadings* dht);
    float getTemperature(struct DHTReadings* dht);
    float getHumidity(struct DHTReadings* dht);

#endif