#include <nrfx.h>
#include <nrfx_spi.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "spi.h"

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
void spi_deinit()
{
	nrfx_spi_uninit(&spi);
	printk("Denit SPI success\n");
}

void ESP_write_data(float temperature, float humidity, int setPoint, int heaterState)
{
	nrfx_err_t err_com;
    spi_xfer_done = false;
    memset(spi_tx_buf, 0, 32*sizeof(uint8_t));
	memset(spi_rx_buf, 0, 32*sizeof(uint8_t));
    spi_desc.tx_length = 32;
    spi_desc.rx_length = 0;

	char dataToSend[30];
	sprintf(dataToSend, "t_%0.1f_h_%0.1f_s_%0.1d_hS_%d_end", temperature, humidity, setPoint, heaterState);
    spi_tx_buf[0] = 0x02;
	spi_tx_buf[1] = 0x00;
	strcpy(spi_tx_buf + 2, dataToSend);

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