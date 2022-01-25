#include "SPI_FlashDriver.h"
#include "stm32f1xx_hal.h" 
#include "stdbool.h"
#include <math.h>

extern SPI_HandleTypeDef hspi1;
/* SSPxSR - bit definitions. */
#define TFE     0x01
#define TNF     0x02
#define RNE     0x04
#define RFF     0x08
#define BSY     0x10
#define PAGE_SZ         528             /* Page size                   */

/* SPI Data Flash Commands */
#define SPI_READ_DATA           0xE8
#define SPI_READ_STATUS         0xD7
#define SPI_BLOCK_ERASE         0x50
#define SPI_BUF1_WRITE          0x84
#define SPI_PAGE_PROGRAM        0x83
#define SPI_PAGE_READ           0x53

#define SPI_TIME_OUT 10


typedef signed char     S8;
typedef unsigned char   U8;
typedef short           S16;
typedef unsigned short  U16;
typedef int             S32;
typedef unsigned int    U32;
typedef long long       S64;
typedef unsigned long long U64;
typedef unsigned char   BIT;
typedef unsigned int    BOOL;


//SPI_HandleTypeDef hspi1;

#define CS(x)   ((x) ? (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET)): (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET))) 
//#define RST(x)  ((x) ? (HAL_GPIO_WritePin(GPIOB,SPI2_RESET_Pin,GPIO_PIN_SET)): (HAL_GPIO_WritePin(GPIOB,SPI2_RESET_Pin,GPIO_PIN_RESET)))

//#define SCK(x)  ((x) ? (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET)): (HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET))) 
//#define MOSI(x) ((x) ? (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET)): (HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET)))
//#define MISO (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2))


//#define mdelay() __nop(); __nop(); __nop(); __nop(); __nop();__nop(); __nop(); __nop(); __nop(); __nop();__nop(); __nop(); __nop(); __nop(); __nop();__nop(); __nop(); __nop(); __nop(); __nop();




BOOL Spi_Flash_Init (void) {

	
//	RST(1);
//	CS(1);
	//HAL_GPIO_WritePin(GPIOB,SPI2_RESET_Pin,GPIO_PIN_SET);
	
 return (__TRUE);
  
}
/////////////////////////

////////////////////////
char Spi_Flash_id(char param)
{
	 unsigned char buf[5]; 	
	return buf[1];
}
void Spi_Flash_write_buffer(U32 address,U16 sz,char *buf)
{
	   /* Program Page in Flash Memory. */ 
  U32 cnt;
  
  while (sz) {
    /* Read Flash Page to Buf 1. */
    spi_write (SPI_PAGE_READ, address, NULL, 0);

    cnt = PAGE_SZ - (address % PAGE_SZ);
    if (cnt > sz) cnt = sz;
    spi_write (SPI_BUF1_WRITE, address,(U8 *) buf, cnt);
    spi_write (SPI_PAGE_PROGRAM, address, NULL, 0);

    address += cnt;
    buf += cnt;
    sz  -= cnt;
  }
	
}
//////////////////////////////////////////////////////////////////////
void Spi_Flash_read_buffer(U32 address,U16 sz,char *buf)
{	
	U8  cbuf[8];
		cbuf[0]=0;cbuf[1]=0;cbuf[2]=0;cbuf[3]=0;
		cbuf[4]=0;cbuf[5]=0;cbuf[6]=0;cbuf[7]=0;
	
	  cbuf[0] = SPI_READ_DATA;
		set_adr (&cbuf[1], address);
				
		SetSS (0);
		SendBuf (cbuf, 8);               /* 4 bytes dont care */
		RecBuf ((U8 *)buf, sz);
		SetSS (1);

}
//////////////////////////////////
void Spi_Flash_write_word(U32 address,U16 val)
{
	   /* Program Page in Flash Memory. */ 
	U16 sz;
  U32 cnt;
  U8  buff[4];
	U8 *buf;
	

  buff[1]= val >> 8; 
	buff[0]= val & 0x00ff;

  buf = buff;	
	sz=2;
  while (sz) { /* Read Flash Page to Buf 1. */
    spi_write (SPI_PAGE_READ, address, NULL, 0);

    cnt = PAGE_SZ - (address % PAGE_SZ);
    if (cnt > sz) cnt = sz;
    spi_write (SPI_BUF1_WRITE, address, buf, cnt);
    spi_write (SPI_PAGE_PROGRAM, address, NULL, 0);

    address += cnt;
    buf += cnt;
    sz  -= cnt;
  }
	
}
//////////////////////////////////////////////////////////////////////
U16 Spi_Flash_read_word(U32 address)
{	
	  U16 w;
  	U8  cbuf[8];
	  U8  buff[2];
	  cbuf[0]=0;cbuf[1]=0;cbuf[2]=0;cbuf[3]=0;
	  cbuf[4]=0;cbuf[5]=0;cbuf[6]=0;cbuf[7]=0;
	
	  cbuf[0] = SPI_READ_DATA;
		set_adr (&cbuf[1], address);
				
		SetSS (0);
		SendBuf (cbuf, 8);               /* 4 bytes dont care */
		RecBuf (buff, 2);
		SetSS (1);
	
	  w = buff[1] << 8;
	  w = w | buff[0];	
	
		return w;
}

//////////////////////////////////
void Spi_Flash_write_byte(U32 address,U8 val)
{
	   /* Program Page in Flash Memory. */ 
  U32 cnt;
	U8  buff;
   buff=val; 
	
    /* Read Flash Page to Buf 1. */
    spi_write (SPI_PAGE_READ, address, NULL, 0);

    cnt = PAGE_SZ - (address % PAGE_SZ);
    if (cnt > 1) cnt = 1;
    spi_write (SPI_BUF1_WRITE, address, &buff, cnt);
    spi_write (SPI_PAGE_PROGRAM, address, NULL, 0);
	
		
}
/////////////////////////////////////////
U8 Spi_Flash_read_byte(U32 address)
{	
  	U8  cbuf[8];
	  U8  buff;
	
	   cbuf[0]=0;cbuf[1]=0;cbuf[2]=0;cbuf[3]=0;
	   cbuf[4]=0;cbuf[5]=0;cbuf[6]=0;cbuf[7]=0;


	  cbuf[0] = SPI_READ_DATA;
		set_adr (&cbuf[1], address);
				
		SetSS (0);
		SendBuf (cbuf, 8);               /* 4 bytes dont care */
		RecBuf (&buff, 1);
		SetSS (1);
	

	
	  return buff;				
}
//////////////////////////////////
void Spi_Flash_write_bool(U32 address,BOOL val)
{
	   /* Program Page in Flash Memory. */ 
  U32 cnt;
	U8  buff;
	
  if(val == __TRUE)
			buff = 1;
		else if(val == __FALSE)
			buff = 0;
		
  
    /* Read Flash Page to Buf 1. */
    spi_write (SPI_PAGE_READ, address, NULL, 0);

    cnt = PAGE_SZ - (address % PAGE_SZ);
    if (cnt > 1) cnt = 1;
    spi_write (SPI_BUF1_WRITE, address, &buff, cnt);
    spi_write (SPI_PAGE_PROGRAM, address, NULL, 0);

		
}
/////////////////////////////////////////
BOOL Spi_Flash_read_bool(U32 address)
{	
  	U8  cbuf[8];
	  U8  buff;

	  cbuf[0] = SPI_READ_DATA;
		set_adr (&cbuf[1], address);
				
		SetSS (0);
		SendBuf (cbuf, 8);               /* 4 bytes dont care */
		RecBuf (&buff, 1);
		SetSS (1);
	
	  if(buff ==1)
			return __TRUE;
		else if(buff == 0)
		  return	__FALSE;		
}

/////////////////////////////////////////

//////////////////////////////////
void spi_out(U8 data)
{
//	SetSS(0);
	HAL_SPI_Transmit(&hspi1,&data,1,SPI_TIME_OUT);
//	SetSS(1);
}
/////////////////////////////////////////////
U8 spi_in(void)
{
	U8 data;
//	SetSS(0);
	HAL_SPI_Receive(&hspi1,&data,1,SPI_TIME_OUT);
//	SetSS(1);
	return data;

}
//////////////////////////////




/*--------------------------- Send ------------------------------------------*/
U8 Send (U8 outb) {
  /* Write and Read a byte on SPI interface. */
//	SetSS(0);
	U8 data;
	HAL_SPI_TransmitReceive(&hspi1,&outb,&data,1,SPI_TIME_OUT);
//	SetSS(1);
	return data;
	
}
/*--------------------------- SendBuf ---------------------------------------*/

BOOL SendBuf (U8 *buf, U32 sz) {
  /* Send buffer to SPI interface. */
  U32 i;
	   
	
  for (i = 0; i < sz; i++) {
    spi_out(buf[i]);		
  }
		
  return (__TRUE);
}


/*--------------------------- RecBuf ----------------------------------------*/

BOOL RecBuf (U8 *buf, U32 sz) {
  /* Receive SPI data to buffer. */
  U32 i;

	for (i = 0 ; i < sz ; i++)
	{
		 buf[i] = spi_in();
	}
	
  return (__TRUE);
}

/*--------------------------- SetSS -----------------------------------------*/

BOOL SetSS (U32 ss) {
  /* Enable/Disable SPI Chip Select (drive it high or low). */

  if (ss) {
    /* SSEL is GPIO, output set to high. */
    CS(1);
  } 
  else {
    /* SSEL is GPIO, output set to low. */
    CS(0);
  }
  return (__TRUE);
}

/*--------------------------- spi_write -------------------------------------*/

void spi_write (U8 cmd, U32 adr, U8 *buf, U32 sz) {
  /* Send SPI Command to Data Flash. */
  U8  sr,cbuf[4];

  cbuf[0] = cmd;
  set_adr (&cbuf[1], adr);
  SetSS (0);
  SendBuf (cbuf, 4);
  if (cmd == SPI_BUF1_WRITE) {
    SendBuf (buf, sz);
  }
  SetSS (1);  

  /* Wait while device is Busy. */
  SetSS (0);
  Send (SPI_READ_STATUS);
  Send (0xFF);                /* dummy byte */
  do {
    sr = Send (0xFF);
  } while ((sr & 0x80) == 0);
  SetSS (1);
}


/*--------------------------- set_adr ---------------------------------------*/

void set_adr (U8 *cbuf, U32 adr) {
  /* Convert linear address to segmented Data Flash address */
  U32 bl_adr, pg_adr;

  bl_adr = adr / PAGE_SZ;
  pg_adr = adr % PAGE_SZ;

  adr = (bl_adr << 10) | pg_adr;
  cbuf[0] = (U8)(adr >> 16);
  cbuf[1] = (U8)(adr >> 8 );
  cbuf[2] = (U8)(adr >> 0 );
}

void dly (uint32_t Time)
{
    uint32_t ii;
    
    ii = 0;
    while (Time--) 
		{
			  for (ii = 0 ; ii < 5 ; ii++);
    }
}


/*---------------------ERASE BLOCK-----------------------------------*/
void Spi_Flash_eraseBlock(U32 address)
{	
  	U8  cbuf[8];
	
	   cbuf[0]=0xff;cbuf[1]=0xff;cbuf[2]=0xff;cbuf[3]=0xff;
	   cbuf[4]=0xff;cbuf[5]=0xff;cbuf[6]=0xff;cbuf[7]=0xff;


	  cbuf[0] = SPI_BLOCK_ERASE;
		set_adr (&cbuf[1], address);
				
		SetSS (0);
		SendBuf (cbuf, 8);               /* 4 bytes dont care */
		SetSS (1);
}






