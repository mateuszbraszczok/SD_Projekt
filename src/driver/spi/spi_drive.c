#include <nrfx.h>
#include <nrfx_spi.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "spi_drive.h"

#define SPI_TX_BUFSIZE 32
#define SPI_RX_BUFSIZE 32

#define SPI_SCK_PIN  28
#define SPI_MOSI_PIN 29
#define SPI_MISO_PIN 30
#define SPI_SS_PIN   31

static volatile bool spi_xfer_done;
uint8_t spi_tx_buf[SPI_TX_BUFSIZE];
uint8_t spi_rx_buf[SPI_RX_BUFSIZE];



static const nrfx_spi_t spi = NRFX_SPI_INSTANCE(1);
nrfx_spi_xfer_desc_t spi_desc = {
	.p_tx_buffer = spi_tx_buf,
	.p_rx_buffer = spi_rx_buf,
	.tx_length = SPI_TX_BUFSIZE,
	.rx_length = SPI_TX_BUFSIZE
};

static void spi_event_handler(nrfx_spi_evt_t const * p_event, void * p_context)
{
	spi_xfer_done = true;
}
void spi_init(void)
{
	nrfx_spi_config_t spi_config = NRFX_SPI_DEFAULT_CONFIG(
		SPI_SCK_PIN, SPI_MOSI_PIN,
		SPI_MISO_PIN, SPI_SS_PIN
	);

	nrfx_err_t err_spi_init = nrfx_spi_init(&spi, &spi_config, spi_event_handler, NULL);
	if(err_spi_init == NRFX_SUCCESS)
	{
		printk("Init SPI success\n");
	}
}
void spi_uninit()
{
	nrfx_spi_uninit(&spi);
	printk("Uinit SPI success\n");
}
/* tutaj musialem zmodyfikowac funkcje strcat, ponieważ oryginalna zjadala pierwszy znak w danych.
   Nie jest to wina oryginalnej funkcji tylko tego, że w wysylaniu w buforze na drugim miejcu trzeba dać 0x00 */
char *strcat_MY(char *ZRESTRICT dest, const char *ZRESTRICT src)
{
	strcpy(dest + (strlen(dest)+1), src);
	return dest;
}


/* funkcja testujaca SPI od ESP */
void ESP_write_Hello(void)
{
	printk("writing hello\n");
	nrfx_err_t err_com;
    spi_xfer_done = false;
    memset(spi_tx_buf, 0, 32*sizeof(uint8_t));
	memset(spi_rx_buf, 0, 32*sizeof(uint8_t));
    spi_desc.tx_length = 32;
    spi_desc.rx_length = 0;
    spi_tx_buf[0] = 0x02;
	spi_tx_buf[1] = 0x00;
	strcat_MY(spi_tx_buf, "Hello Slave!");
	err_com = nrfx_spi_xfer(&spi, &spi_desc,0);
	if (err_com == NRFX_SUCCESS)
	{
		printk("Send to ESP success!\n");
	}
	else
	{
		printk("Config Mode No Success!\n");
	}
	while (!spi_xfer_done);
}
/* funkcja testujaca SPI od ESP */
void ESP_write_Alive(void)
{
	nrfx_err_t err_com;
    spi_xfer_done = false;
    memset(spi_tx_buf, 0, 32*sizeof(uint8_t));
	memset(spi_rx_buf, 0, 32*sizeof(uint8_t));
    spi_desc.tx_length = 32;
    spi_desc.rx_length = 0;
    spi_tx_buf[0] = 0x02;
	spi_tx_buf[1] = 0x00;
	strcat_MY(spi_tx_buf, "Are you alive?");
	err_com = nrfx_spi_xfer(&spi, &spi_desc,0);
	if (err_com == NRFX_SUCCESS)
	{
		//printf("Send to ESP success!\n");
	}
	else
	{
		printf("Config Mode No Success!\n");
	}
	while (!spi_xfer_done);
}
/* funkcja wysylajaca dane do ESP */
void ESP_write(uint8_t* data)
{
	nrfx_err_t err_com;
    spi_xfer_done = false;
    memset(spi_tx_buf, 0, 32*sizeof(uint8_t));
	memset(spi_rx_buf, 0, 32*sizeof(uint8_t));
    spi_desc.tx_length = 32;
    spi_desc.rx_length = 0;
    spi_tx_buf[0] = 0x02;
	spi_tx_buf[1] = 0x00;
	strcat_MY(spi_tx_buf,data);
	err_com = nrfx_spi_xfer(&spi, &spi_desc,0);
	if (err_com == NRFX_SUCCESS)
	{
		//printf("Send to ESP success!\n");
	}
	else
	{
		printf("Config Mode No Success!\n");
	}
	while (!spi_xfer_done);
}

void ESP_write_data(float temperature, float humidity, int setPoint, int heaterState)
{
	char str[30];
	sprintf(str, "t_%0.1f_h_%0.1f_s_%0.1d_hS_%d_end", temperature, humidity, setPoint, heaterState);
	//sprintf(str, "hello world");
	nrfx_err_t err_com;
    spi_xfer_done = false;
    memset(spi_tx_buf, 0, 32*sizeof(uint8_t));
	memset(spi_rx_buf, 0, 32*sizeof(uint8_t));
    spi_desc.tx_length = 32;
    spi_desc.rx_length = 0;
    spi_tx_buf[0] = 0x02;
	spi_tx_buf[1] = 0x00;
	strcat_MY(spi_tx_buf, str);
	//strcat_MY(spi_tx_buf, "Are you alive?");
	err_com = nrfx_spi_xfer(&spi, &spi_desc,0);
	if (err_com == NRFX_SUCCESS)
	{
		//printf("Send to ESP success!\n");
	}
	else
	{
		printf("Config Mode No Success!\n");
	}
	while (!spi_xfer_done);
}


/* Funkcja czytająca z ESP, niestety dostaje smieci */
void ESP_read(void)
{
	nrfx_err_t err_com;
    spi_xfer_done = false;
	memset(spi_tx_buf, 0, 32*sizeof(uint8_t));
    memset(spi_rx_buf, 0, 32*sizeof(uint8_t));
    spi_desc.tx_length = 32;
    spi_desc.rx_length = 32;
	spi_tx_buf[0] = 0x03;
	spi_tx_buf[1] = 0x00;
	err_com = nrfx_spi_xfer(&spi, &spi_desc,0);
	if (err_com == NRFX_SUCCESS)
	{
        printf("I got: %s\n", spi_rx_buf);
	}
	else
	{
		printf("Config Mode No Success!\n");
	}
	while (!spi_xfer_done);
}