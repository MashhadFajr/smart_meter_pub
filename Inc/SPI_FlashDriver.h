#include "stm32f1xx_hal.h"
//#include <RTL.h>

typedef signed char     S8;
typedef unsigned char   U8;
typedef short           S16;
typedef unsigned short  U16;
typedef int             S32;
typedef unsigned int    U32;
//typedef unsigned long    U32;
typedef long long       S64;
typedef unsigned long long U64;
typedef unsigned char   BIT;
typedef unsigned int    BOOL;
#define __TRUE         1
#define __FALSE        0



BOOL Spi_Flash_Init (void);
BOOL Spi_Flash_Uninit (void);
char Spi_Flash_id(char param);
void spi_out(U8 data);
U8 spi_in(void);
void dly (uint32_t Time);


U8 Send (U8 outb);
BOOL SendBuf (U8 *buf, U32 sz) ;
BOOL RecBuf (U8 *buf, U32 sz);
BOOL SetSS (U32 ss) ;
void spi_write (U8 cmd, U32 adr, U8 *buf, U32 sz) ;
void set_adr (U8 *cbuf, U32 adr);

void Spi_Flash_write_bool(U32 address,BOOL val);
BOOL Spi_Flash_read_bool(U32 address);

void Spi_Flash_write_byte(U32 address,U8 val);
U8 Spi_Flash_read_byte(U32 address);

void Spi_Flash_write_word(U32 address,U16 val);
U16 Spi_Flash_read_word(U32 address);

void Spi_Flash_write_buffer(U32 address,U16 sz,char *buf);
void Spi_Flash_read_buffer(U32 address,U16 sz,char *buf);


void Spi_Flash_eraseBlock(U32 address);

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI2_Init(void);
