#pragma once
#pragma message ( "Warning, not linux, spi is being stubbed" )

#include <stdint.h>

 #define SPI_CPHA                0x01
 #define SPI_CPOL                0x02
 
 #define SPI_MODE_0              (0|0)
 #define SPI_MODE_1              (0|SPI_CPHA)
 #define SPI_MODE_2              (SPI_CPOL|0)
 #define SPI_MODE_3              (SPI_CPOL|SPI_CPHA)
 
 #define SPI_CS_HIGH             0x04
 #define SPI_LSB_FIRST           0x08
 #define SPI_3WIRE               0x10
 #define SPI_LOOP                0x20
 #define SPI_NO_CS               0x40
 #define SPI_READY               0x80

#define SPI_IOC_WR_MODE 0
#define SPI_IOC_RD_MODE 1
#define SPI_IOC_WR_BITS_PER_WORD 2
#define SPI_IOC_RD_BITS_PER_WORD 3
#define SPI_IOC_WR_MAX_SPEED_HZ 4
#define SPI_IOC_RD_MAX_SPEED_HZ 5

struct spi_ioc_transfer {
    uint64_t tx_buf;
    uint64_t rx_buf;
    uint32_t len;
    uint32_t speed_hz;
    uint16_t delay_usecs;
    uint8_t  bits_per_word;
    uint8_t  cs_change;
    uint8_t  tx_nbits;
    uint8_t  rx_nbits;
    uint16_t pad;
 };

 #define SPI_IOC_MESSAGE(x) 0
 #define ioctl(x, y, z) -1