//#include "main.h"
#include "stm32f1xx_hal.h"                     
#include "modbus.h"

//extern void DELAY_us(unsigned int count);
extern U16 Modbus_buffer[250];
extern UART_HandleTypeDef huart1;

void Modbus_Read_Register_responce(U8 slav_address,U8 function,U16 start_address,U16 num_register)
{
	U8 t_buf[500];
	U16 crc;
	
	U16 b_cnt;
	U16 i;
	
	if (num_register < 110)
	{	
			t_buf[0] = slav_address;
			t_buf[1] = function ;
			t_buf[2] = num_register * 2 ;
			
			for(i=0 ; i<= num_register ; i++)
			{
				t_buf[(i*2)+3] = get_high_byte(Modbus_buffer[start_address+i]);
				t_buf[(i*2)+4] = get_low_byte(Modbus_buffer[start_address+i]);
			}
				
			b_cnt = (num_register * 2) + 3 ;
			
			crc = clac_crc( &t_buf[0] ,b_cnt);
			
			t_buf[b_cnt] =   get_low_byte(crc);
			t_buf[b_cnt+1] = get_high_byte(crc);
			
			b_cnt = b_cnt + 2;
			
			
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_SET);
			HAL_UART_Transmit(&huart1,t_buf,b_cnt,HAL_MAX_DELAY);


			Delay(10);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_RESET);
			
	}
}

//``````````````````````````````````````````````
void Modbus_Write_Register_responce(U8 slav_address,U8 function,U16 start_address,U16 num_register)	
{
	U8 t_buf[60];
	U16 crc;
	
	U16 b_cnt;
//	char i;
	
	t_buf[0] = slav_address;
	t_buf[1] = function ;
	t_buf[2] = get_high_byte(start_address);
	t_buf[3] = get_low_byte(start_address);
	t_buf[4] = get_high_byte(num_register);
	t_buf[5] = get_low_byte(num_register);	
	
	crc = clac_crc( &t_buf[0] ,6);
	
	t_buf[6] = get_low_byte(crc);
	t_buf[7] = get_high_byte(crc);
	b_cnt = 8;
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_SET);
	
	HAL_UART_Transmit(&huart1,t_buf,b_cnt,HAL_MAX_DELAY);

	Delay(10);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_RESET);
	
}
//`````````````````````````````````````````````````
U16 clac_crc(U8 *array_address,U8 data_len)
{
	U16 ii;
	U16 jj;
	U8 *p;
	U16 s_crc;
	U16 s_crcb;
	U8 temp;
	
	p = array_address;
	s_crc = 0xFFFF;
	for(ii=0;ii<data_len;ii++)
	{
		
	  s_crcb = *p;
		s_crc = s_crc ^ s_crcb;
		for(jj=0;jj<8;jj++)
				{
					temp = s_crc & 0x1;
					if(temp == 1)
					{
							s_crc = s_crc / 2;
							s_crc = s_crc ^ 0xA001;
					}
					else
					{			
							s_crc = s_crc / 2;
					}
		  	}
				p += 1;
	   }
	return s_crc;
	}

	
	
unsigned char get_high_byte(unsigned short int d)
{
	return (d>>8);
}


unsigned char get_low_byte(unsigned short int d)
{
	return (d & 0x00ff);
}

//unsigned short make_int(unsigned char  low,unsigned char  high)
//{
//	unsigned short _d;
//	_d = (high <<8);
//	_d = _d | low;
//}

//U16 make_int(U8 low,U8 high)
//{
//	U16 _d;
//	_d = (high <<8);
//	_d = _d | low;
//}

void Delay (uint32_t Time)
{
    uint32_t ii;
    
    ii = 0;
    while (Time--) 
		{
        for (ii = 0; ii < 5000; ii++);
    }
}