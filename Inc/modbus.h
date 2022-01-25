#include "stm32f1xx_hal.h" 
//#include <RTL.h>

void Modbus_Read_Register_responce(U8 slav_address,U8 function,U16 start_address,U16 num_register);
void Modbus_Write_Register_responce(U8 slav_address,U8 function,U16 start_address,U16 num_register);


U16 clac_crc(U8 *array_address,U8 data_len);
unsigned char get_low_byte(unsigned short int d);
unsigned char get_high_byte(unsigned short int d);
//U16 make_int(U8 low,U8 high);
void Delay (uint32_t Time);
	
//typedef struct 
//{
//	U8 buffer[100];
//	U16 byte_count;
//	U16 stop_time;
//	U8  rcv_flag;
////	U8  request_address;
////	U16 num_register;
////	U16 start_address;
//	
//}Serial_RCV_Struct;

