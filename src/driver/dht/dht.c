#include "dht.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"


//--------------------------------------------------------
// PRIVATE FUNCTIONS
void sendSignalToDHTSensor()
{
  /* Set DHT Pin pull-up resistor disabled */
  nrf_gpio_cfg_input(DHT_PIN, NRF_GPIO_PIN_NOPULL);
  nrf_delay_ms(1);

  /* MCU sends out start signal and pulls down voltage for at least 18ms to lest DHT detect the signal */
  nrf_gpio_cfg_output(DHT_PIN);
  nrf_gpio_pin_set(DHT_PIN);
  nrf_gpio_pin_clear(DHT_PIN);
  nrf_delay_ms(18);
}

DHTErrorCode waitForDHTResponse()
{
  /* MCU waits for DHT response */
  nrf_gpio_cfg_input(DHT_PIN, NRF_GPIO_PIN_NOPULL);
  nrf_delay_us(50);

  if(nrf_gpio_pin_read(DHT_PIN)!=0) 
  {
      printk("no ACK\n");
      return DHT_FAIL;
  }

  int cntr = 18;

  /* DHT sends out response signal and keeps it for 80us */
  while(nrf_gpio_pin_read(DHT_PIN)==0) 
  {  
	  nrf_delay_us(5);
    if (--cntr==0) 
    {
      printk("no ACK2\n"); 
      return DHT_FAIL;
    }
  }
  cntr = 18;

  /* wait until signal goes down */
  while(nrf_gpio_pin_read(DHT_PIN)!=0) 
  { 
	  nrf_delay_us(5);
    if (--cntr==0) 
    {
      printk("no ACK3\n"); /* signal should be up for the ACK here */
      return DHT_FAIL;
    }
  }
  return DHT_SUCCESS;
}

DHTErrorCode readingDataFromDHTSensor(uint8_t *buffer)
{
  int i = 0;
  int data = 0;
  int loopBits = 40;
  do 
  {
    int cntr = 11; /* wait max 55 us */
    while(nrf_gpio_pin_read(DHT_PIN)==0) 
    {
      nrf_delay_us(5);
      if (--cntr==0) 
      {
        printk("DHT11_NO_DATA_0\n");
        return DHT_FAIL;
      }
    }
    cntr = 15; /* wait max 75 us */
    while(nrf_gpio_pin_read(DHT_PIN)!=0) 
    {
      nrf_delay_us(5);
      if (--cntr==0) 
      {
        printk("DHT11_NO_DATA_1\n");
        return DHT_FAIL;
      }
    }
    data <<= 1; /* next data bit */
    if (cntr<10) /* data signal high > 30 us ==> data bit 1 */
    { 
      data |= 1;
    }
    if ((loopBits&0x7)==1) /* next byte */
    { 
      buffer[i] = data;
      i++;
      data = 0;
    }
  } while(--loopBits!=0);

  return DHT_SUCCESS;
}
  
DHTErrorCode verifyReturnedBuffer(uint8_t *buffer)
{
  if ((uint8_t)(buffer[0]+buffer[1]+buffer[2]+buffer[3])!=buffer[4]) 
  {
      printk("DHT11_BAD_CRC\n");
      return DHT_FAIL;
  }
  return DHT_SUCCESS;
}
  
//--------------------------------------------------------

// INTERFACE

DHTErrorCode dhtRead(struct DHTReadings* dht)
{
  // BUFFER TO RECEIVE
  uint8_t buffer[5];
  int i;

  /* init buffer */
  for(i=0;i<sizeof(buffer); i++) 
  {
      buffer[i] = 0;
  }

  sendSignalToDHTSensor();

  if (waitForDHTResponse() == DHT_FAIL)
  {
    return DHT_FAIL;
  }
  if (readingDataFromDHTSensor(buffer) == DHT_FAIL)
  {
    return DHT_FAIL;
  }
  if (verifyReturnedBuffer(buffer) == DHT_FAIL)
  {
    return DHT_FAIL;
  }
  
  if (dht->dhtModel == DHT22)
  {
    int f = (buffer[0]) << 8 | buffer[1];
    dht->humidityIntPart = f / 10;
    dht->humidityDecimalPart = f % 10;

    f = (buffer[2]) << 8 | buffer[3];
    dht->temperatureIntPart = f / 10; 
    dht->temperatureDecimalPart = f % 10;
  }
  else
  {
    dht->humidityIntPart = buffer[0];
    dht->humidityDecimalPart = buffer[1];

    dht->temperatureIntPart = buffer[2];
    dht->temperatureDecimalPart = buffer[3];
  }

  return DHT_SUCCESS;
}

float getTemperature(struct DHTReadings* dht)
{
  float temperature = (float) (dht->temperatureIntPart + (dht->temperatureDecimalPart / 10.0));
  return temperature;
}

float getHumidity(struct DHTReadings* dht)
{
  float humidity = (float) (dht->humidityIntPart + (dht->humidityDecimalPart / 10.0));
  return humidity;
}
  