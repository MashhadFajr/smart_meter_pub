/*
  ******************************************************************************
	* Programmer				 : Milad Rasouli at Mahhad Fajr Vira Company
	* File Name          : main.c at meter project
  * Description        : this project is about meters which it must reads
	*	thier parameter of meter and you can also contorl meter relays and get
	*	its parameter via modbus protocol.
	*	best of luck. Milad Rasouli 10/20/2021
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "SPI_FlashDriver.h"
#include "modbus.h"

// multi meter defines
#define Toos_4800 				1
#define Toos_9600_small		2
#define Toos_9600 				3
#define Toos_v4						4
#define Behineh 					5

unsigned int Meter_delay_part2 = 1700;
U8 behineh_meter_round[5]={0};
U32 reactive_behin_buffer=0;
char Behineh_time[10],Behineh_date[10];
void removeChar(char *str, char garbage) {

    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}

//			external flash define's
#define external_flash_addres 5000
/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

IWDG_HandleTypeDef hiwdg;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void SystemClock_Config(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART1_UART_Init(U16 baud);
static void MX_USART2_UART_Init(U16 baud);
static void MX_USART3_UART_Init(U16 baud);
static void MX_IWDG_Init(void);

unsigned short make_int(unsigned char  low,unsigned char  high);
void UART3_SendString (unsigned char *s);
void UART2_SendString (unsigned char *s);

void led(char num,char val);
void Led_com(char val);
void Led_pmr(char val);
void Led_os1(char val);
void Led_os2(char val);
void Led_os3(char val);
void Led_os4(char val);

void restor_eram_backup();
void save_eram_backup();


//function temp
int GetTemperature(void);

void  Relay_trig(char Relay_number,char State,char Status);
U16  Read_temperture();

void Read_input();
//void Radio_config(U16 Address,char Chanel,U16 Baud_ttl,U16 Baud_radio);
void Radio_config();

void  Modbus_transmit(char Slave_address,char Func,U16 Start_address,U16 Num_register);

void  Make_serial_number(char Reg_add);


void  Restor_default();
void  Read_config();

void Proces_rcv_string(char Index);

//			boot function
void deinitEverything(void);


U32 chang_value(char *s);
U16 chang_value_16(char *s);

/*******************************************************/
int cmpfunc(const void* a, const void* b)
{
    unsigned int arg1 = *(const unsigned int*)a;
    unsigned int arg2 = *(const unsigned int*)b;
 
//    if (arg1 > arg2) return -1;
//    if (arg1 < arg2) return 1;
//    return 0;
 
    //return (arg1 > arg2) - (arg1 < arg2); // possible shortcut
    return arg2 - arg1; // erroneous shortcut (fails if INT_MIN is present)
}
long avrage( long array[],unsigned char i, unsigned char n){
	long long avrg=0;
	int end = i+n;
	while(i < end)
	{
		avrg += array[i];
		i++;
	}
	return avrg / n;
}
//---------------------------------------------

#define Led_CPU(x)  ((x) ? (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET))  : (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET))) 

#define Relay4_on(x) ((x) ? (HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14,GPIO_PIN_SET))  : (HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14,GPIO_PIN_RESET))) 
#define Relay4_off(x)  ((x) ? (HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET))  : (HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET))) 

#define Relay3_on(x)   ((x) ? (HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET))  : (HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET))) 
#define Relay3_off(x)  ((x) ? (HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET))  : (HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET))) 

#define Relay2_on(x)   ((x) ? (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET))  :  (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_RESET))) 
#define Relay2_off(x)   ((x) ? (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_SET))  : (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_RESET))) 

#define Relay1_on(x)   ((x) ? (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET))  : (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET))) 
#define Relay1_off(x)  ((x) ? (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_SET))  : (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET))) 

#define Radio_m0(x)   ((x) ? (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET))  : (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET))) 
#define Radio_m1(x)   ((x) ? (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET))  : (HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET))) 

#define Buzzer(x)     ((x) ? (HAL_GPIO_WritePin(GPIOC,GPIO_PIN_15,GPIO_PIN_SET))  : (HAL_GPIO_WritePin(GPIOC,GPIO_PIN_15,GPIO_PIN_RESET))) 

//================================VARIABLE'S===============================

//temp var
unsigned int InternalTemperature=0;
unsigned int temp_adc=0;
//----------------current fault variable's
#define Current_fault_time 7000 // 10 Minuts 600,000  70000
unsigned int current_loop;
unsigned char current_fault[4]={0,0,0,0};
unsigned char flag_current_reload_time_fault[4]={0,0,0,0};
unsigned int Current_value[4];
// --------------- Main variable's--------------
U8 Read_temperture_flag;
U8 current_status_flag;
U8 flag_cpu_blink=0;
U16 Temperture_avrage;
U8 Temperture_deb;
U8 Check_error_flag;
U8 Stop_adc=0;
U8 read_meter_flag;
U8 read_meter_flag2;
U8 read_meter_counter;
U8 meter_count;     
U8 reset_meter=0;
char Meter_id[6][50];

U8 Over_current_error_counter[6];
U8 Over_current_error_debounce[6];

U8 Relay_connect_agin_flag[6];
U8 Temp;
U8 Temp1;
char Temp_s[50];

U8 flag_stop_meter_get_voltage=0;
U8 relay_flag;
U8 Read_voltage_flag ;
U32 deb_volt;
unsigned int adc_value=0;
#define ADC_LIMIT_VOLTAGE 150
unsigned int adc_buffer[100];
unsigned int adc_sort_temp;
char loop_asort=0,loop_bsort=0;
unsigned int adc_avrage_above=0;
unsigned int adc_avrage_blow=0;
int adc_channel_voltage[4];
U16 adc_buff[5]={0,0,0,0,0};
int mux[4]={0,0,0,0};//{26,4,35,35};//unsigned long mux[4]={725,806,850,826};{725,806,850,826}
U8 adc_swich=1;
		
// fault vaiable
#define HYSTERIA_VALIUE 8
U8 faul_meter_counter=1;
U8	hysteria_normal[5]={0,0,0,0,0};

U8 Password_reset_timer;

U16 Voltage_value_meter[5];
// -------Inputs Variable's--------
U8 Read_input_flag;
U8 Debin;
U8 Deb;
U8 Door_key_sts;

U8 Door_code_true;
U8 Door_code_1;
U8 Door_code_2;
U8 Door_code_3;
U8 First_key_trig;


// ----- RS485 Variables ------
typedef struct 
{
	U8 buffer[2000];
	U16 byte_count;
	U16 stop_time;
	U8  rcv_flag;
}Serial_RCV_Struct_m;

Serial_RCV_Struct_m Serial2_RCV;
char Meter_recive_data_flag;

U8 Rcv_485;
U8 Rcv_array_485[1501];

U16 Rcv_counter_485;
U8 Num_data_485;
U8 Rcv_complate_flag_485;

char Rcv_splited_array[11];
char Crlf_str[2]; 
//U8 Crlf_U8 = new U8[3]; 
U8 Split_count;

char Rcv_string[31]; 
U8 Rcv_chksum;
U8 My_chksum;

U8 Etx_detect;
U8 My_bcc;

U16 A;
U16 B;
U16 C;

U16 Temperture_margin1;
U16 Temperture_margin2;
U16 Temperture_margin3;

// -------- Radio Variables -------
U8 Rcv_123_cnt;
U8 My_address;

//```````````MODBUS SERIAL VARIABLES````````````
typedef struct 
{
	U8 buffer[100];
	U16 byte_count;
	U16 stop_time;
	U8  rcv_flag;
}Serial_RCV_Struct;

Serial_RCV_Struct Serial1_RCV;
char Recive_data_flag;
U8 Rx_data[2];
U8 Rx_data2[1];

U16 *array_add;
U16 my_crc;
U16 rcv_crc;
U16 b_cnt;
U16 ix;
U16 im;
U16 id1;
U16 id2;
U16 ik1;
U16 ik2;
U16 ik3;
U16 in;

U16 Modbus_buffer[250];

U16 Modbus_rcv_StartAddress;
U16 Modbus_rcv_NumRegister;
U16 Rtu_address;

U8 Temperture_status_level;
U8 Door_status ;
U8 Over_current_error[5];
U8 Power_meter_status[5]; 
U8 Relay_status[5]={0,0,0,0,0};  
U8 Status_pm[5] = {0,0,0,0,0};
U8 General_status = 0;

int Temperture;
//float temperture_buffer=0;
U16 F_version;

U16 Password;
//const 
//const U8 status_for_pm = 8;
const U8 relay_status_for_pm = 12;
const int Door_close = 0;
const int Door_open = 1;
const int Door_alarm = 2;
const int Door_allow = 3;
const int Normal = 1; /* TODO ERROR: Skipped SkippedTokensTrivia *//* TODO ERROR: Skipped SkippedTokensTrivia */

const int Over_voltage_error = 2;
const int Lower_voltage_error = 3;
const int Disconnect = 4;
const int Short_circuit = 5;

const int Current_first_error = 2;
const int Current_second_error = 3;
const int Current_third_error = 4;
const int Current_leakage_error = 5;

const int Relay_off = 1;
const int Relay_on = 2;
const int Relay_error = 3;

const int Relay_off_with_sotware = 1;
const int Relay_off_with_error = 2;
const int Relay_on_in_normal = 3;
const int Relay_on_with_software = 4;

char Relay_1_release;
char Relay_2_release;
char Relay_3_release;
char Relay_4_release;
U8   Relay_Previce_state[5]={1,1,1,1,1};

char Relay_last_state[5];

U16 V_temp;
U16 D_temp;
U16 R_temp;
U16 R_temp2;
U16 Im;
U16 Register_last_value;

// ------------------------------------------------------------------
U8 Spi_buf[21];


//````````SYSTICK TIMER VARIABLES```````````
U16 i;
U16 j;
U16 l;
U16 k;
U16 d;
//U8 m;
U16 t;
U16 v;

U16 b;
U16 c;
U16 s1=0;
U16 m[5];
U16 r1[5];	
U8 flag_s=0;
char led_command[2];

char temp;
char temp1 ;
char temp_str[25];

char rcv_string[15][40];
int ii = 0;
char *token ;
char str_temp[150];
U8 obisCodeLen;
char str_temp2[30];

//these vaiables are for trig relays for times unit
U8 relay_order_again_flag=0;
U8 relay_order_flag=0;
U8 relay_order_state=0;
U8 relay_order_number=0;
U8 Relay_order_times=0;

int main(void)
{

  /* USER CODE BEGIN 1 */
	My_address = 2;
  /* USER CODE END 1 */
	int adc_loop=0;
	unsigned int adc_avrage;
	int adc_channel_buffer;
	int loop0=0;
  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  SystemClock_Config();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init(19200);
  //MX_USART2_UART_Init();
  MX_USART3_UART_Init(300);
  MX_IWDG_Init();

  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart1,Rx_data,1); 
	
	//HAL_UART_Receive_IT(&huart2,Rx_data2,1);
	
	HAL_UART_Receive_IT(&huart3,Rx_data2,1); 

	SysTick_Config(SystemCoreClock/1000); 
	

	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_RESET);	//En485 Master
	
	
	Password = 123 ;
	Num_data_485 = 2;

  Led_CPU(0);
	Led_com(0);
	Led_pmr(0);
	Led_os1(0);
	Led_os2(0);
	Led_os3(0);
	Led_os4(0);

	meter_count = 4;	
	
	read_meter_flag = 1;
					
	F_version=716;

	Radio_config();
	//Restor_default();
	Read_config();
	
	for(loop0=0;loop0<4;loop0++)
	{
		Relay_last_state[loop0+1]=3;
		if(Modbus_buffer[124+(5*loop0)]) // relay be on
		{
			Relay_trig(loop0+1,1,Relay_status[loop0+1]);
		}
		else // relay be off
		{
			Relay_trig(loop0+1,0,Relay_status[loop0+1]);
		}
		HAL_Delay(1);
	}
	
	// watch dog setting 
//	MX_IWDG_Init(); //IWDG inti
  // Start the IWDG  
//  HAL_IWDG_Start(&hiwdg);
	
  /* Infinite loop */
  while (1)
  {
		//active_multi_meter();

//	HAL_IWDG_Refresh(&hiwdg);//reload IWDG
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//gozashtani zarayeb tashheh dakhel moubus buffer
	Modbus_buffer[160] = mux[0];
	Modbus_buffer[161] = mux[1];
	Modbus_buffer[162] = mux[2];
	Modbus_buffer[163] = mux[3];
	if(mux[0]>110) mux[0]=0;
	if(mux[1]>110) mux[1]=0;
	if(mux[2]>110) mux[2]=0;
	if(mux[3]>110) mux[3]=0;
	//error maker unit
	if (Check_error_flag == 1)//independed of ADC Task
	{
		Check_error_flag = 0;
		for(faul_meter_counter=1;faul_meter_counter<5;faul_meter_counter++)
		{
			//mohaseba address modbus
			temp = faul_meter_counter - 1;
			temp = temp * 5;
			temp = temp + 120;
			
			if ((Modbus_buffer[temp] == 1))//@@ 1 Amper Limit Disbla or Enable
			{
				
				if((Modbus_buffer[114+faul_meter_counter] > Modbus_buffer[temp + 2])&&(Modbus_buffer[114+(faul_meter_counter)]>0))/*&& //over voltage*/
				{
					General_status = 2;
					Status_pm[faul_meter_counter] = Over_voltage_error;
					hysteria_normal[faul_meter_counter]=1; // hyster set
				}
				
				else if((Modbus_buffer[114+faul_meter_counter]<Modbus_buffer[temp+3])&&(Modbus_buffer[114+(faul_meter_counter)]>0))/*&&	//Lower voltage*/
				{
					General_status = 2;
					hysteria_normal[faul_meter_counter]=1; // hyster set
					Status_pm[faul_meter_counter] = Lower_voltage_error;
				}
				
				else if((Modbus_buffer[14+(faul_meter_counter*20)] > ADC_LIMIT_VOLTAGE)&&//agar voltage gheraat shode az 150 volt bishtar bood
					 (Modbus_buffer[19+(faul_meter_counter*20)] < ADC_LIMIT_VOLTAGE)&&//agar voltage ADC az 150 kamtar bood
					 (Modbus_buffer[temp+4]==1)&&                                    //agar farmane relay vasl bashad
				   (Relay_status[faul_meter_counter] != Relay_off_with_error))     //va halate ghabl off ba khata nabashad
							//&&(Status_pm[faul_meter_counter] != Disconnect)) 
				{
					General_status = 2;//it means error  
					Status_pm[faul_meter_counter] = Disconnect;
				}		


				else if((Modbus_buffer[14+(faul_meter_counter*20)] > ADC_LIMIT_VOLTAGE)&&//agar voltage gheraat shode az 150 volt bishtar bood
					 (Modbus_buffer[19+(faul_meter_counter*20)] > ADC_LIMIT_VOLTAGE)&&//agar voltage ADC az 150 kamtar bood
					 (Modbus_buffer[temp+4]!=1)&&                                    //agar farmane relay vasl bashad
				   (Relay_status[faul_meter_counter] != Relay_off_with_error))     //va halate ghabl off ba khata nabashad
							//&&(Status_pm[faul_meter_counter] != Disconnect)) 
				{
					General_status = 2;//it means error  
					Status_pm[faul_meter_counter] = Short_circuit;
				}
				
				
				//this condition when be excuit whitch each voltage be OK 
				else if((Modbus_buffer[114+faul_meter_counter] < (Modbus_buffer[temp + 2]))&& // voltage is ok
						(Modbus_buffer[114+faul_meter_counter]> (Modbus_buffer[temp + 3]))&&
				(Modbus_buffer[14+(20*faul_meter_counter)]> ADC_LIMIT_VOLTAGE)&&
				(hysteria_normal[faul_meter_counter]==0)&&(Modbus_buffer[114+(faul_meter_counter)]>0))
				{
					General_status = Normal;
					Status_pm[faul_meter_counter] = Normal;
				}	
				//hyster part
				if((Modbus_buffer[114+faul_meter_counter] <= (Modbus_buffer[temp + 2] - HYSTERIA_VALIUE)) && // normal hyster voltage condition
					 (Modbus_buffer[114+faul_meter_counter] >= (Modbus_buffer[temp+3] + HYSTERIA_VALIUE)))
				{
					hysteria_normal[faul_meter_counter] = 0;
				}
			}
		}
		relay_flag = 1; // active relay unit	
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//@rtu_address = My_address
		Modbus_buffer[1] = My_address;
		Modbus_buffer[170] = My_address;
		Modbus_buffer[19] = read_meter_counter;
		Modbus_buffer[18] = F_version;

		Modbus_buffer[13] = Relay_status[1];
		Modbus_buffer[14] = Relay_status[2];
		Modbus_buffer[15] = Relay_status[3];
		Modbus_buffer[16] = Relay_status[4];
		
		Modbus_buffer[9] = Status_pm[1];
		Modbus_buffer[10] = Status_pm[2];
		Modbus_buffer[11] = Status_pm[3];
		Modbus_buffer[12] = Status_pm[4]; 
		Modbus_buffer[2] = General_status;
		// Buzzer become ON in here when Door been open 
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1))
		{
			Modbus_buffer[4] = 1;
			Buzzer(1);
		}
		else
		{
			//Modbus_buffer[4] = 0;
			Buzzer(0);
		}
	}
	
	//Relay unit
	if(relay_flag == 1)//independed of error maker unit it will be active in that
	{
		relay_flag = 0;
		for (int Temp1 = 1; Temp1 < 5; Temp1++)
		{
			temp = Temp1 - 1;
			temp = temp * 5;
			temp = temp + 120;
			
			if(Temperture_status_level == 4) //agar dama bish az hade tanzimi bashad kole rele
			{
				Relay_trig(Temp1,0,Relay_off_with_error);		
			}
			
			else //agar dama dar hade mojaz bashad angah sharaite dighe test shavad
			{	
				if(Modbus_buffer[temp]==1)// when Amper limit be active
				{	
					if((Relay_last_state[Temp1] == 1)&&(current_fault[Temp1-1] == 0)) // it means previuse state was on
					{
						if(Status_pm[Temp1] == Over_voltage_error)
						{
							Relay_trig(Temp1,0,Relay_off_with_error);
							Modbus_buffer[temp+4]=2;
							Modbus_buffer[14+(20*Temp1)] = 0; // make it 0 meter voltage parameter
							flag_stop_meter_get_voltage = 1;
							mux[Temp1-1] = 0; // clear mux
							Voltage_value_meter[Temp1] = 0; //previuse voltage meter				
							Modbus_buffer[114+Temp1] = 0; // make it 0 voltage modbus buffer
						}
						else if(Status_pm[Temp1] == Lower_voltage_error)
						{
							Relay_trig(Temp1,0,Relay_off_with_error);
							Modbus_buffer[temp+4]=2;//write on 124 to 139 modbus address
							Modbus_buffer[14+(20*Temp1)] = 0; // make it 0 meter voltage parameter
							mux[Temp1-1] = 0; // clear mux 
							Voltage_value_meter[Temp1] = 0;
							flag_stop_meter_get_voltage = 1;
							Modbus_buffer[114+Temp1] = 0; // make it 0 voltage modbus buffer
						}
						else if((Modbus_buffer[temp+4]==2)/*&&(Status_pm[Temp1] == Normal)*/)
						{
							Relay_trig(Temp1,0,Relay_off_with_sotware);
							Modbus_buffer[14+(20*Temp1)] = 0; // make it 0 meter voltage parameter
							mux[Temp1-1] = 0; // clear mux 
							Voltage_value_meter[Temp1] = 0;
							flag_stop_meter_get_voltage = 1;
							Modbus_buffer[114+Temp1] = 0; // make it 0 voltage modbus buffer 
							
							//@
							Spi_Flash_write_byte(204+(Temp1-1),0); // write current value in flash ic
						}
					}
					else if((Relay_last_state[Temp1] == 0)&&(current_fault[Temp1-1] == 0)) // it means preivuse state was off
					{
						if(((Status_pm[Temp1]==Normal))&& (Modbus_buffer[14+(20*Temp1)] != 0) &&
							(Relay_status[Temp1] == Relay_off_with_error))
						{
							Relay_trig(Temp1,1,Relay_on_in_normal);
							//Modbus_buffer[temp+4]=1;
						}
						else if((Modbus_buffer[temp+4]==1)&&(Relay_status[Temp1] != Relay_on_in_normal)&&
									((Status_pm[Temp1] == Normal)||(Status_pm[Temp1]== Disconnect)))
						{
							Relay_trig(Temp1,1,Relay_on_with_software);
							//@
							Spi_Flash_write_byte(204+(Temp1-1),0); // write current value in flash ic
						}
					}
					
					if((Relay_last_state[Temp1] == 1)&&(current_fault[Temp1-1] == 1))
					{
						current_fault[Temp1-1] = 2;	//go to the next condition				
						Relay_trig(Temp1,0,Relay_off_with_error);
						
						Modbus_buffer[temp+4]=2;
						Spi_Flash_write_byte(204+(Temp1-1),2); // write current value in flash ic
						Modbus_buffer[14+(20*Temp1)] = 0; // make it 0 meter voltage parameter
						
						flag_stop_meter_get_voltage = 1;
						
						mux[Temp1-1] = 0; // clear mux
						
						Voltage_value_meter[Temp1] = 0; //previuse voltage meter				
						
						Modbus_buffer[114+Temp1] = 0; // make it 0 voltage modbus buffer
					}
					else if((Relay_last_state[Temp1] == 0)&&(current_fault[Temp1-1] == 2))
					{
						if((Modbus_buffer[temp+4]==1)&&(Status_pm[Temp1] == Normal))
						{
							current_fault[Temp1-1] = 0; // clear current fault
							
							Modbus_buffer[Temp1+4] = 0; // clear 
							
							Spi_Flash_write_byte(204+(Temp1-1),0); // write current value in flash ic
							Spi_Flash_write_byte(208+(Temp1-1),0); // write modebus current status in flash ic

							Relay_trig(Temp1,1,Relay_on_with_software);
						}
					}
				}			
				else if(!Modbus_buffer[temp]) //// when Amper limit be Deactive
				{
					if(Relay_last_state[Temp1] == 1)
					{
						if(Modbus_buffer[temp+4]==2)
						{
							Relay_trig(Temp1,0,Relay_off_with_sotware);
						}
					}
					else if(Relay_last_state[Temp1] == 0)
					{
						if(Modbus_buffer[temp+4]==1)
						{
							Relay_trig(Temp1,1,Relay_on_with_software);
						}
					}
				}
			}	
		}		
	}
	//ADC unit
	if((Read_voltage_flag == 1)&&(Stop_adc==0)) // every 2ms
	{		
		Read_voltage_flag = 0;
		temp=0;
		//```````````
		hadc2.Init.NbrOfConversion= adc_swich;
		HAL_ADC_Init(&hadc2);
		HAL_ADC_Start(&hadc2);
		if(HAL_ADC_PollForConversion(&hadc2,10)==HAL_OK)
		{
			adc_value = HAL_ADC_GetValue(&hadc2);
		}
		HAL_ADC_Stop(&hadc2);		
		adc_buffer[deb_volt] = (unsigned int)adc_value;
		deb_volt +=1U;
		
		
		if(deb_volt>=100U)
		{
			if((adc_buffer[0]>=50)||(adc_buffer[99]>=50))
			{
				//qsort (&adc_buffer, 100, sizeof(unsigned int), cmpfunc);//adc_buffer_sort
				//Sort the array in ascending order    
				for (loop_asort=0;loop_asort<100;loop_asort++) {     
						for (loop_bsort=0; loop_bsort < 100; loop_bsort++) {     
							 if(adc_buffer[loop_asort] > adc_buffer[loop_bsort]) {    
									 adc_sort_temp = adc_buffer[loop_asort];    
									 adc_buffer[loop_asort] = adc_buffer[loop_bsort];    
									 adc_buffer[loop_bsort] = adc_sort_temp;    
							 }     
						}     
				}
				adc_avrage=0;
				adc_avrage_above=0;
				
				for(adc_loop=0;adc_loop<7;adc_loop++)
				{
						adc_avrage = adc_avrage + adc_buffer[adc_loop];
				}
				adc_avrage_above = adc_avrage >> 3;/*((unsigned int)adc_avrage) / 4U;*/
				adc_avrage=0;
				
				adc_avrage_blow = 0;
				
				for(adc_loop=92;adc_loop<99;adc_loop++)
				{
					adc_avrage = adc_avrage + adc_buffer[adc_loop]; 
				}
				adc_avrage_blow = adc_avrage >> 3;/*((unsigned int)adc_avrage) / 4U;*/
				//adc_avrage=0;
				//adc_avrage_above = avrage(adc_buffer,0,5);
				//adc_avrage_blow  = avrage(adc_buffer,95,5);
			
				adc_channel_voltage[adc_swich-1] = (adc_avrage_above-adc_avrage_blow)/5.5;	
				adc_channel_buffer = adc_channel_voltage[adc_swich-1] - mux[adc_swich-1];
				
				if(adc_channel_buffer >= ADC_LIMIT_VOLTAGE) //agar voltage gheraat shode az 150 volt bishtar bood
				{
					adc_buff[adc_swich] = adc_channel_voltage[adc_swich-1] - mux[adc_swich-1] ;//voltage dar adc_buf gharar migirad
				}
				else
				{
					adc_buff[adc_swich] = 0;
				}		//4	9	14	19
			
				// write voltages in 115 to 118 address
				if((Modbus_buffer[119U+(adc_swich*5U)]==1U) && (mux[adc_swich-1U]>0U) &&(current_fault[adc_swich-1U]!=2)) //agar meter voltage dasht va relay farmane vasl dasht
				{
					Modbus_buffer[114+adc_swich] = adc_buff[adc_swich]; 
				}
				else
				{
					Modbus_buffer[114+adc_swich] = Voltage_value_meter[adc_swich]; 
				}
				
				//write voltages in 39 69 89 99
				Modbus_buffer[19+(adc_swich*20)] = adc_buff[adc_swich]; //channel voltage
				Check_error_flag=1;//active error task
			}
			
			deb_volt=0;
			adc_swich++;
			if(adc_swich>=5)
			{
				adc_swich=1;
			}
			
		}	
	}
	//CPU LED BLINK , every 200 ms 
	if(flag_cpu_blink==1)
	{
		flag_cpu_blink=0;		
		//@Toggle Led_cup
		if(d==0)// reset_time == 0)
		{
			d=1;
			Led_CPU(d);			
		}
		else
		{
			d=0;
			Led_CPU(d);
		}
	}
	//Read current and tamperture 3000ms
	if( Read_temperture_flag == 1)
  {
		Read_temperture_flag = 0;
		Temperture = GetTemperature();//at this line we take tamperture value from function 
		Modbus_buffer[17]=Temperture; //this line put tamperture value on the modbus buffer address 17
		Modbus_buffer[3] = 1;   // temperture status address which "1" means normal
		
		
		if(Temperture > Modbus_buffer[103])
			Temperture_status_level = 4;
		
		else if((Temperture > Modbus_buffer[102])  && (Temperture_status_level != 4))
			Temperture_status_level = 3;
		
		else if((Temperture > Modbus_buffer[101])  && (Temperture_status_level != 4))
			Temperture_status_level = 2;
		
		else if((Temperture < Modbus_buffer[101]) && (Temperture_status_level != 4))
			Temperture_status_level = 1;
		
		
		Modbus_buffer[3] = Temperture_status_level;		
	
		
	}
	if(current_status_flag == 1)
  {	
		for(current_loop=0;current_loop < 4;current_loop++)
		{
			//Amper limit be active modbus address[120]
			if(Modbus_buffer[120+(current_loop*5)]==1)
			{
				// Modbus[35] > modbus[121] -> correct current > limitation current && Reload Time passed
				if((Current_value[current_loop] > Modbus_buffer[121+(current_loop*5)])&&
						(!current_fault[current_loop]))
				{
					Current_value[current_loop] = 0;
					
					Modbus_buffer[5+current_loop] += 1;
					
					flag_current_reload_time_fault[current_loop] = 1;
					
					if(Modbus_buffer[5+current_loop]>=3) //current error address in modbus
					{
						// [204] to [207] is for current_fault
						// [208] to [211] is for modbus[5] to modbus[8]
						current_fault[current_loop] = 1;
						Spi_Flash_write_byte(204+current_loop,1); // write current value in flash ic
						Spi_Flash_write_byte(208+current_loop,3); // write modebus current status in flash ic
					}
				} // modbus[34]< modbus[121] -> meter current < limitation value 
				else if(Modbus_buffer[15+((current_loop+1)*20)] <= Modbus_buffer[121+(current_loop*5)]&&
								(Modbus_buffer[5+current_loop])&&
								(!current_fault[current_loop]))
				{
					Modbus_buffer[5+current_loop] = 0;
				}
			}
		}
	}
 
	
	//
	
	if(read_meter_flag == 1)
	{
		read_meter_flag = 0;
		meter_count = 4;	
		read_meter_counter +=1;
		if(read_meter_counter > meter_count){read_meter_counter = 1;}
		
		switch(Modbus_buffer[164+read_meter_counter])
		{
			case Toos_4800:

				MX_USART3_UART_Init(4800);//set usart budrate
				UART3_SendString(Meter_id[read_meter_counter]);
				read_meter_flag2=2;// active read_meter flag2 after passed some time
				
				Meter_delay_part2 = 1700; // make a delay for going to next part

			break;
			
			case Toos_9600:
			case Toos_9600_small:
		
				MX_USART3_UART_Init(9600);//set usart budrate
				UART3_SendString(Meter_id[read_meter_counter]);
				Num_data_485 = 2;
				My_bcc = 0;
				Rcv_counter_485 =0;		
				Led_pmr(1);

			break;
			case Toos_v4:
				
				//read_meter_counter=0;///@
				MX_USART3_UART_Init(300);//set usart budrate
				UART3_SendString(Meter_id[read_meter_counter]);
				read_meter_flag2=2; // active read_meter flag2 after passed some time
				Meter_delay_part2 = 1700; // make a delay for going to next part
				
			break;
		
			case Behineh:
		
				MX_USART3_UART_Init(300);//set usart budrate
				UART3_SendString(Meter_id[read_meter_counter]);
				read_meter_flag2=2; // active read_meter flag2 after passed some time
				Meter_delay_part2 = 1400; // make a delay for going to next part
			
			break;
		}
	}
	
	if(read_meter_flag2==1)
	{
		read_meter_flag2=0;
		switch(Modbus_buffer[164+read_meter_counter])
		{
			case Toos_4800:

				temp_str[0]= 0x06 ; //ACK
				temp_str[1]= 0x30 ; //'0'
				temp_str[2]= 0x34 ; 
				temp_str[3]= 0x30 ; //'0'
				temp_str[4]= 0x0d ; //CR
				temp_str[5]= 0x0a ; //LF
				temp_str[6]= '\0' ; //null
				
				UART3_SendString(temp_str);
				
				Num_data_485 = 2;
				My_bcc = 0;
				Rcv_counter_485 =0;		
				Led_pmr(1);
			
			break;
			case Toos_v4:
		
				temp_str[0]= 0x06 ; //ACK
				temp_str[1]= 0x30 ; //'0'
				temp_str[2]= 0x35 ; //'5'
				temp_str[3]= 0x30 ; //'0'
				temp_str[4]= 0x0d ; //CR
				temp_str[5]= 0x0a ; //LF
				temp_str[6]= '\0' ; //null
				
				UART3_SendString(temp_str);
				MX_USART3_UART_Init(9600);
				
				Num_data_485 = 2;
				My_bcc = 0;
				Rcv_counter_485 =0;		
				Led_pmr(1);
			
			break;
			case Behineh:
				if(!behineh_meter_round[read_meter_counter]) // this condition when will be active which we be in round 0
				{
					temp_str[0]= 0x06 ; //ACK
					temp_str[1]= 0x30 ; //'0'
					temp_str[2]= 0x34 ; //'5'
					temp_str[3]= 0x30 ; //'0'
					temp_str[4]= 0x0d ; //CR
					temp_str[5]= 0x0a ; //LF
					temp_str[6]= '\0' ; //null
				}			
				else // this condition when will be excute which it be in round 1
				{	   // in here we want to get other behineh's parameter's  and after sending that we are going to budrade 4800bps
					temp_str[0]= 0x06 ; //ACK
					temp_str[1]= 0x30 ; //'0'
					temp_str[2]= 0x34 ; //'5'
					temp_str[3]= 0x31 ; //'0'
					temp_str[4]= 0x0d ; //CR
					temp_str[5]= 0x0a ; //LF
					temp_str[6]= '\0' ; //null
				}			
				UART3_SendString(temp_str);
				
				MX_USART3_UART_Init(4800);
				
				if(behineh_meter_round[read_meter_counter]==1)
				{//01 50 31 02 28 30 30 30 31 30 30 31 31 31 31 31 31 29 03 60 
					
					HAL_Delay(611); //this delay is requied for reciving
					// send password message to meter
					temp_str[0] = 0x01;	temp_str[1] = 0x50;
					temp_str[2] = 0x31;	temp_str[3] = 0x02;
					temp_str[4] = 0x28;	temp_str[5] = 0x30;
					temp_str[6] = 0x30;	temp_str[7] = 0x30;
					temp_str[8] = 0x31;	temp_str[9] = 0x30;
					temp_str[10]= 0x30;	temp_str[11]= 0x31;
					temp_str[12]= 0x31;	temp_str[13]= 0x31;
					temp_str[14]= 0x31;	temp_str[15]= 0x31;
					temp_str[16]= 0x31;	temp_str[17]= 0x29;
					temp_str[18]= 0x03;	temp_str[19]= 0x60;
					temp_str[20]= '\0';//null
					UART3_SendString(temp_str);
					// send reicive message
					//01 52 32 02 39 30 33 46 28 29 03 1C
					HAL_Delay(330);
					temp_str[0] = 0x01;	temp_str[1] = 0x52;
					temp_str[2] = 0x32;	temp_str[3] = 0x02;
					temp_str[4] = 0x39;	temp_str[5] = 0x30;
					temp_str[6] = 0x33;	temp_str[7] = 0x46;
					temp_str[8] = 0x28;	temp_str[9] = 0x29;
					temp_str[10]= 0x03;	temp_str[11]= 0x1c;
					temp_str[12]= '\0';//null
					UART3_SendString(temp_str);
					
					behineh_meter_round[read_meter_counter] = 2; // go to the next unit
					HAL_Delay(50);
				}
				
				Num_data_485 = 2;
				My_bcc = 0;
				Rcv_counter_485 =0;		
				Led_pmr(1);
				
			break;
		}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~			
	if (Rcv_complate_flag_485 == 1)
	{
		Rcv_complate_flag_485 = 0;
		
		if(1)//(Rcv_chksum == My_bcc)
		{
			Led_pmr(0);
			ii=0;
			token = strtok(Rcv_array_485,"\r\n");
			strcpy(str_temp,token);
			B=0;
			while(token!=0)				
			{
				strcpy(str_temp,token);
				for(A=0;A<30;A++){str_temp2[A]=0;} // make it null , all of it
				
				switch(Modbus_buffer[164+read_meter_counter])
				{
					case Toos_4800:
						
						if(strstr(str_temp,".00.00") != NULL) //serial number  .00.00.00(13537443)
						{					  
							strncpy(str_temp2,str_temp+10,8);
							strcpy(rcv_string[1] , str_temp2);				  
						}						
						else if(strstr(str_temp,"01.08.01") != NULL) //KW 1   .01.08.01(000052.77*kWh)
						{					  
							strncpy(str_temp2,str_temp+10,9);
							strcpy(rcv_string[2] , str_temp2);				  
						}						
						else if(strstr(str_temp,"01.08.02") != NULL) //KW 2    .01.08.02(000016.42*kWh)
						{					  
							strncpy(str_temp2,str_temp+9,9);
							strcpy(rcv_string[3] , str_temp2);				  
						}					
						else if(strstr(str_temp,"01.08.03") != NULL) //KW 3  01.08.03(000039.41*kWh)
						{					  
							strncpy(str_temp2,str_temp+9,9);
							strcpy(rcv_string[4] , str_temp2);				  
						}				
						else if(strstr(str_temp,"03.08.00") != NULL) //KVAR    03.08.00(000008.95*kvarh)
						{					  
							strncpy(str_temp2,str_temp+9,9);
							strcpy(rcv_string[5] , str_temp2);				  
						}				
						else if(strstr(str_temp,"13.07.00") != NULL) //COSFI  .13.07.00(1.00)
						{					  
							strncpy(str_temp2,str_temp+9,4);
							strcpy(rcv_string[6] , str_temp2);				  
						}		
						else if(strstr(str_temp,"32.07.00") != NULL) //VOLTAGE   .32.07.00(230.32*V)
						{					  
							strncpy(str_temp2,str_temp+9,6);
							strcpy(rcv_string[7] , str_temp2);				  
						}
						else if(strstr(str_temp,"31.07.00") != NULL) //CURRNET   .31.07.00(000.00*A)
						{					  
							strncpy(str_temp2,str_temp+9,6);
							strcpy(rcv_string[8] , str_temp2);				  
						}						
						else if(strstr(str_temp,"00.09.02") != NULL) //DATE  00.09.02(13980818)
						{					  
							strncpy(str_temp2,str_temp+9,8);
							strcpy(rcv_string[9] , str_temp2);				  
						}					
						else if(strstr(str_temp,"00.09.01") != NULL) //TIME  00.09.01(1217)
						{					  
							strncpy(str_temp2,str_temp+9,4);
							strcpy(rcv_string[10] , str_temp2);				  
						}
						
					break;
					case Toos_9600_small:
						
						if(strstr(str_temp,"0.0.0.255")!= NULL)
						{
							strncpy(str_temp2,str_temp+14,14);
							strcpy(rcv_string[1] , str_temp2);
						}
						else if(strstr(str_temp,"1-0:1.8.1.255")!=NULL)
						{
							strncpy(str_temp2,str_temp+15,9);
							strcpy(rcv_string[2] , str_temp2);
						}
						else if(strstr(str_temp,"1-0:1.8.2.255")!=NULL)
						{
							strncpy(str_temp2,str_temp+14,9);
							strcpy(rcv_string[3] , str_temp2);
						}
						else if(strstr(str_temp,"1-0:1.8.3.255")!=NULL)
						{
							strncpy(str_temp2,str_temp+14,9);
							strcpy(rcv_string[4] , str_temp2);
						}
						else if(strstr(str_temp,"1-0:3.8.0.255")!=NULL)
						{
							strncpy(str_temp2,str_temp+14,9);
							strcpy(rcv_string[5] , str_temp2);
						}
						else if(strstr(str_temp,"1-0:13.7.0.255")!=NULL)
						{
							strncpy(str_temp2,str_temp+15,4);
							strcpy(rcv_string[6] , str_temp2);
						}
						else if(strstr(str_temp,"1-0:32.7.0.255")!=NULL)
						{
							strncpy(str_temp2,str_temp+15,6);
							strcpy(rcv_string[7] , str_temp2);
						}
						else if(strstr(str_temp,"1-0:31.7.0.255")!=NULL)
						{
							strncpy(str_temp2,str_temp+15,6);
							strcpy(rcv_string[8] , str_temp2);
						}
						else if(strstr(str_temp,"0-0:1.0.0.255")!=NULL)
						{
							strncpy(str_temp2,str_temp+14,14);
							strcpy(rcv_string[9] , str_temp2);
						}
						
						
					break;
					case Toos_9600:
					case Toos_v4:
						
						if(strstr(str_temp, "0.0.0.255") != NULL) //serial number // 1-0:0.0.0.255(18079921011800){0D}{0A}
						{					  
							strncpy(str_temp2,str_temp+14,14);
							strcpy(rcv_string[1] , str_temp2);					  
						}
						else if((strstr(str_temp, "1-0:15.8.1.255") != NULL)||(strstr(str_temp, "1-0:1.8.1.255") != NULL)) //KW1 //1-0:15.8.1.255(000000.00*kWh){0D}{0A}
						{																								
							strncpy(str_temp2,str_temp+15,9);
							strcpy(rcv_string[2] , str_temp2);					
						}
						else if((strstr(str_temp, "1-0:15.8.2.255") != NULL)||(strstr(str_temp, "1-0:1.8.2.255") != NULL)) //KW2 //1-0:15.8.2.255(000000.00*kWh){0D}{0A}
						{																								
							strncpy(str_temp2,str_temp+15,9);
							strcpy(rcv_string[3] , str_temp2);					
						}
						else if((strstr(str_temp, "1-0:15.8.3.255") != NULL)||(strstr(str_temp, "1-0:1.8.3.255") != NULL)) //KW3 //1-0:15.8.3.255(000000.00*kWh){0D}{0A}
						{																								
							strncpy(str_temp2,str_temp+15,9);
							strcpy(rcv_string[4] , str_temp2);					
						}
						else if(strstr(str_temp, "1-0:3.8.0.255") != NULL) //KVAR //1-0:3.8.0.255(000000.00*kvarh)!
						{																								
							strncpy(str_temp2,str_temp+14,9);
							strcpy(rcv_string[5] , str_temp2);					
						}	
						else if(strstr(str_temp, "1-0:13.7.0.255") != NULL) //KVAR //1-0:3.8.0.255(000000.00*kvarh)!
						{																								
							strncpy(str_temp2,str_temp+15,4);
							strcpy(rcv_string[6] , str_temp2);					
						}	
						else if(strstr(str_temp, "1-0:32.7.0.255") != NULL) //Voltage //1-0:32.7.0.255(226.30V)
						{																								
							strncpy(str_temp2,str_temp+15,6) ;//7); //(226.30V)adade 7 baraye Check Charecter Validity
							strcpy(rcv_string[7] , str_temp2);						
						}				
						else if(strstr(str_temp, "1-0:31.7.0.255") != NULL) //Current //1-0:31.7.0.255(000.00A)
						{																								
							strncpy(str_temp2,str_temp+15,6);
							strcpy(rcv_string[8] , str_temp2);						
						}		
						else if(strstr(str_temp, "0-0:1.0.0.255") != NULL) //DATE TIME//0-0:1.0.0.255(14000126181151)
						{																								
							strncpy(str_temp2,str_temp+14,14);
							strcpy(rcv_string[9] , str_temp2);						
						}	
						
					break;
					case Behineh:
						
						if(strstr(str_temp, "0.0.0") != NULL) //'serial number  .00.00.00(13537443)
						{																								
							strncpy(str_temp2,str_temp+6,16);
							strcpy(rcv_string[1] , str_temp2);						
						}
						else if(strstr(str_temp, "1.8.1") != NULL) //'KW 1   1.8.1(000009.68*kWh)
						{																								
							strncpy(str_temp2,str_temp+7,9);
							strcpy(rcv_string[2] , str_temp2);						
						}
						else if(strstr(str_temp, "1.8.2") != NULL) //'KW 2    .1.8.2(000004.64*kWh)
						{																								
							strncpy(str_temp2,str_temp+6,9);
							strcpy(rcv_string[3] , str_temp2);						
						}
						else if(strstr(str_temp, "1.8.3") != NULL) //'KW 3  .1.8.3(000002.84*kWh)
						{																								
							strncpy(str_temp2,str_temp+6,9);
							strcpy(rcv_string[4] , str_temp2);						
						}
						else if(strstr(str_temp, "13.07.00") != NULL) //'COSFI  .13.07.00(1.00)
						{																								
							strncpy(str_temp2,str_temp+9,14);
							strcpy(rcv_string[6] , str_temp2);						
						}
						else if(strstr(str_temp, "32.7") != NULL) //'VOLTAGE   .32.7(0235.77*V)
						{																								
							strncpy(str_temp2,str_temp+5,7);
							strcpy(rcv_string[7] , str_temp2);						
						}
						else if(strstr(str_temp, "31.7") != NULL) //'CURRNET   .31.7(0000.00*A)
						{																								
							strncpy(str_temp2,str_temp+5,7);
							strcpy(rcv_string[8] , str_temp2);						
						}
						else if(strstr(str_temp, "0.9.1") != NULL) //TIME	0.9.1(22:14:17)<\r><\n>
						{																								
							strncpy(str_temp2,str_temp+6,8);
							strcpy(Behineh_time , str_temp2);						
						}	
						else if(strstr(str_temp, "0.9.2") != NULL) //DATE 	0.9.2(78-03-18)<\r><\n>
						{					//char Behineh_time[15],Behineh_date[15];		rcv_string[9]			(14000126181151)	
							strncpy(str_temp2,str_temp+6,8);
							strcpy(Behineh_date, str_temp2);	
							removeChar(Behineh_time, ':');
							removeChar(Behineh_date, '-');
							strcat(rcv_string[9],"14");
							strcat(rcv_string[9],Behineh_date);
							strcat(rcv_string[9],Behineh_time);
							
							//sprintf(rcv_string[9],"00%s%s", Behineh_date , Behineh_time);
						}	
						//-----------------------------
						if(!behineh_meter_round[read_meter_counter])
						{								
							behineh_meter_round[read_meter_counter] = 1;
						}
						else if(behineh_meter_round[read_meter_counter] == 2)//here we get reactive power of meter
						{//01 42 30 03 71
							// here we are in round 1														
							strncpy(str_temp2,str_temp+2,8); //'KVAR    03.08.00(000008.95*kvarh)
							strcpy(rcv_string[5] , str_temp2);						
							reactive_behin_buffer = chang_value(rcv_string[5]);
							Modbus_buffer[(20*read_meter_counter)+11] = reactive_behin_buffer >> 16 ;       // Tarif1_high
							Modbus_buffer[(20*read_meter_counter)+12] = reactive_behin_buffer & 0x0000ffff;       // Tarif1_low
							HAL_Delay(200);
							//here we send a break message
							temp_str[0] = 0x01;	temp_str[1] = 0x42;
							temp_str[2] = 0x30;	temp_str[3] = 0x03;
							temp_str[4] = 0x71;	temp_str[5] = '\0';
							UART3_SendString(temp_str);
							behineh_meter_round[read_meter_counter] = 3;
							//continue;
						}
					break;
				}
				token = strtok(NULL,"\r\n"); 
				B +=1 ;
			}
			if(behineh_meter_round[read_meter_counter]==3)
			{
				behineh_meter_round[read_meter_counter]=0;
			}
			else
			{
				if(rcv_string[7][0])//this variable is for voltage which we check for exsist value
					Proces_rcv_string(read_meter_counter);
			}
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~		
	if(Recive_data_flag == 1) // modbus 
	{
			Recive_data_flag = 0;
			b_cnt = Serial1_RCV.byte_count;
			Serial1_RCV.byte_count =0;
			my_crc = clac_crc(& Serial1_RCV.buffer[0] , b_cnt-2);
			rcv_crc = make_int(Serial1_RCV.buffer[b_cnt-2] ,Serial1_RCV.buffer[b_cnt-1]);
			if(rcv_crc == my_crc)
			{
				if(Serial1_RCV.buffer[0] == My_address)
				{
						Led_com(1);							
					
						if(Serial1_RCV.buffer[1] == 3 || Serial1_RCV.buffer[1] == 4)
						{	
								
							Modbus_rcv_StartAddress = make_int(Serial1_RCV.buffer[3] ,Serial1_RCV.buffer[2]);
							Modbus_rcv_NumRegister = make_int(Serial1_RCV.buffer[5] ,Serial1_RCV.buffer[4]);
							Modbus_Read_Register_responce(My_address,Serial1_RCV.buffer[1],Modbus_rcv_StartAddress,Modbus_rcv_NumRegister);
																					
						}

						if(Serial1_RCV.buffer[1] == 6 )
						{	
								
								Modbus_rcv_StartAddress = make_int(Serial1_RCV.buffer[3] ,Serial1_RCV.buffer[2]);
								Modbus_rcv_NumRegister  = make_int(Serial1_RCV.buffer[5] ,Serial1_RCV.buffer[4]);	
							
								D_temp = make_int(Serial1_RCV.buffer[5] ,Serial1_RCV.buffer[4]);														
							
								Register_last_value = Modbus_buffer[Modbus_rcv_StartAddress];
							
								if(Modbus_rcv_StartAddress == 100)
								{
									Modbus_buffer[Modbus_rcv_StartAddress] = D_temp;
									Password_reset_timer = 1;
								}
								//``````````````````````````````````````````
								if(Modbus_rcv_StartAddress >=101)
								{
									if(Modbus_buffer[100] == Password)
									{
											Modbus_buffer[171] = 0; //Password Error Log
											Modbus_buffer[Modbus_rcv_StartAddress] = D_temp ;
											
											R_temp = Modbus_rcv_StartAddress - 100 ;
											R_temp = R_temp * 2 ;
											R_temp = R_temp - 1 ;
										
											Spi_Flash_write_word(R_temp,D_temp);								
								
											 if (Modbus_rcv_StartAddress == 144 || 
														Modbus_rcv_StartAddress == 149 ||
														Modbus_rcv_StartAddress == 154 ||
														Modbus_rcv_StartAddress == 159)
											{
													Make_serial_number(Modbus_rcv_StartAddress);
											}

											if (Modbus_rcv_StartAddress == 173)
											{
													Modbus_buffer[Modbus_rcv_StartAddress] = D_temp;       // Reset Chip
											}			
											
										 if(Modbus_buffer[176] == 100)
										 {
											 Restor_default();
											 //Read_config();
											 Modbus_buffer[176]=0;
										 }

										 //run boot application
										 if(Modbus_buffer[175] == 10)
										 {
												// set flag_program_status and go to program internal flash 
												Spi_Flash_write_byte(external_flash_addres-1,0xf0);	
												HAL_Delay(100);
												// deinint
												deinitEverything();
												// reset MCU
												HAL_NVIC_SystemReset();
										 }				
										 
										 //rest application
										 if(Modbus_buffer[173] == 10)
										 {												
												// reset MCU
												HAL_NVIC_SystemReset();
										 }	
										 
										 
										//IWD TEST										 
										if(Modbus_buffer[164] == 10) // just for watch dog test
										{												
											// reset MCU
											while(1);
										}											 
										if(Modbus_buffer[105] > 0) // for Amp Limit
										 {
											 
											 Modbus_buffer[120] = Modbus_buffer[105];
											 Modbus_buffer[125] = Modbus_buffer[105];
											 Modbus_buffer[130] = Modbus_buffer[105];
											 Modbus_buffer[135] = Modbus_buffer[105];
											 
											Spi_Flash_write_word(39,Modbus_buffer[105]);
											Spi_Flash_write_word(49,Modbus_buffer[105]);
											Spi_Flash_write_word(59,Modbus_buffer[105]);
											Spi_Flash_write_word(69,Modbus_buffer[105]);
											 
											 Modbus_buffer[105] = 0;
										 }	
										 if(Modbus_buffer[106] > 0) // for Amp SET for all 
										 {
											 Modbus_buffer[121] = Modbus_buffer[106];
											 Modbus_buffer[126] = Modbus_buffer[106];
											 Modbus_buffer[131] = Modbus_buffer[106];
											 Modbus_buffer[136] = Modbus_buffer[106];
	
												Spi_Flash_write_word(41,Modbus_buffer[106]);
												Spi_Flash_write_word(51,Modbus_buffer[106]);
												Spi_Flash_write_word(61,Modbus_buffer[106]);
												Spi_Flash_write_word(71,Modbus_buffer[106]);
											 
											 Modbus_buffer[106] = 0;
										 }
										 if(Modbus_buffer[107] > 0) // for High voltage SET for all 
										 {
											 Modbus_buffer[122] = Modbus_buffer[107];
											 Modbus_buffer[127] = Modbus_buffer[107];
											 Modbus_buffer[132] = Modbus_buffer[107];
											 Modbus_buffer[137] = Modbus_buffer[107];
							
												Spi_Flash_write_word(43,Modbus_buffer[107]);
												Spi_Flash_write_word(53,Modbus_buffer[107]);
												Spi_Flash_write_word(63,Modbus_buffer[107]);
												Spi_Flash_write_word(73,Modbus_buffer[107]);
												
											 Modbus_buffer[107] = 0;
										 }
										 if(Modbus_buffer[108] > 0) // for Low voltage SET for all 
										 {
											 Modbus_buffer[123] = Modbus_buffer[108];
											 Modbus_buffer[128] = Modbus_buffer[108];
											 Modbus_buffer[133] = Modbus_buffer[108];
											 Modbus_buffer[138] = Modbus_buffer[108];

												Spi_Flash_write_word(45,Modbus_buffer[108]);
												Spi_Flash_write_word(55,Modbus_buffer[108]);
												Spi_Flash_write_word(65,Modbus_buffer[108]);
												Spi_Flash_write_word(75,Modbus_buffer[108]);
												
											 Modbus_buffer[108] = 0;
										 }
									}
									else //Password Incorrect
									{
										 Modbus_buffer[Modbus_rcv_StartAddress] = Register_last_value;
										 Modbus_buffer[174] = Modbus_buffer[174] + 1;       //Password Error Log
									}
									
								}
								
				 //void Modbus_Write_Register_responce(slav_address,function           ,start_address          ,num_register)	
								Modbus_Write_Register_responce(My_address,Serial1_RCV.buffer[1],Modbus_rcv_StartAddress,Modbus_rcv_NumRegister);					
						
						}
						if( Serial1_RCV.buffer[1] == 16 )
						{
							Modbus_rcv_StartAddress = make_int(Serial1_RCV.buffer[3] ,Serial1_RCV.buffer[2]);
							Modbus_rcv_NumRegister =  make_int(Serial1_RCV.buffer[5] ,Serial1_RCV.buffer[4]);								


							V_temp = Modbus_rcv_StartAddress + Modbus_rcv_NumRegister;
							V_temp = V_temp - 1;
							R_temp = 8;
							if (Modbus_rcv_StartAddress >= 101)
							{
									if (Modbus_buffer[100] == Password)
									{
											for (Im = Modbus_rcv_StartAddress;Im <= V_temp; Im++)
											{
													//D_temp = make_int(rcv_array_radio(r_temp + 1), Rcv_array_radio(r_temp));
													D_temp = make_int(Serial1_RCV.buffer[R_temp+1] ,Serial1_RCV.buffer[R_temp]);	
													Modbus_buffer[Im] = D_temp;
													
													R_temp2 = Modbus_rcv_StartAddress - 100 ;
													R_temp2 = R_temp2 * 2 ;
													R_temp2 = R_temp2 - 1 ;
													Spi_Flash_write_word(R_temp2,D_temp);

													R_temp = R_temp + 2;
											}

											if (Modbus_rcv_StartAddress == 140 ||
													Modbus_rcv_StartAddress == 145 ||
													Modbus_rcv_StartAddress == 150 || 
													Modbus_rcv_StartAddress == 155)
											{
													Modbus_rcv_StartAddress = Modbus_rcv_StartAddress + 4;
													Make_serial_number(Modbus_rcv_StartAddress);
											}
									}
							}
							for(in=0; in < Modbus_rcv_NumRegister ; in++)
							{
								Modbus_buffer[Modbus_rcv_StartAddress+in] =  make_int(Serial1_RCV.buffer[(in*2)+8] ,Serial1_RCV.buffer[(in*2)+7]);		
							}
								
				 //void Modbus_Write_Register_responce(slav_address,function           ,start_address          ,num_register)	
							Modbus_Write_Register_responce(My_address,Serial1_RCV.buffer[1],Modbus_rcv_StartAddress,Modbus_rcv_NumRegister);								
						}
						Led_com(0);	
						
					 }
				}	
			}
  }
}
//============================END MAIN==========================

//==============================================================

void SysTick_Handler(void) //EVERY 1mSec
{
	static unsigned int meter2_counter=0;
	static int adc_stop_timer=0;
	static int counter_stop_meter_volt=0;
	static int counter_cpu_blink=0;
	static unsigned long counter_current_fault[4]={0,0,0,0};
	static unsigned char sys_current_loop=0;
	static unsigned int ic=0;  
	static unsigned int relay_order_again_coun = 0;
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
	
//	if(relay_order_flag==1)
//	{
//		relay_order_again_coun ++;
//		if(relay_order_again_coun > 500)
//		{
//			relay_order_again_coun = 0;
//			relay_order_again_flag=1;
//		}
//	}
	counter_cpu_blink++;
	if(counter_cpu_blink>200)
	{
		counter_cpu_blink=0;
		flag_cpu_blink=1;
	}
	if(flag_stop_meter_get_voltage==1)
	{
		adc_stop_timer++;
		if(adc_stop_timer>8000)
		{
			adc_stop_timer=0;
			flag_stop_meter_get_voltage=0;
		}
	}
	
	j +=1 ;
	if(j >17000)//  17000)
	{
		j = 0;
		read_meter_flag = 1;
	}
	
	if(read_meter_flag2==2)
	{
		meter2_counter++;
		if(meter2_counter>1700)
		{
			meter2_counter=0;
			read_meter_flag2=1;
		}
	}
	if(Stop_adc==1)
	{
		adc_stop_timer++;
		if(adc_stop_timer>1000)
		{
			adc_stop_timer=0;
			Stop_adc=0;
		}
	}
//	s1++;
//	if(s1>=5000)
//	{
//		s1=0;
//		//@flag_s=1;
//	}
	
	//``````````````````````
//	c += 1;
//	if ( c >= 300)
//	{
//		c = 0;
//		relay_flag = 1;
//	}
	
	//``````````````````````
//	l += 1;
//	if(l >= 200)
//	{
//		l = 0;
//		Check_error_flag = 1;			
//	}
		
	v +=1 ;
	if(v >= 2)
	{
		v = 0;
		Read_voltage_flag = 1;		 
	}	
	
	
	//```````````````````
	i +=1;
	if(i >= 3000)
	{
		i = 0;
		Read_temperture_flag = 1;
	}
	ic++;
	if(ic>3000)
	{
		ic=0;
		current_status_flag=1;
	}

	
	//````````````````````````````````
	if(Serial1_RCV.rcv_flag == 1)
	{
		Serial1_RCV.stop_time += 1;
		if(Serial1_RCV.stop_time >= 50)
		{
			Serial1_RCV.rcv_flag = 0;				
			Serial1_RCV.stop_time = 0;
			Recive_data_flag = 1;				
		}		
	}	
	//```````````````````
	k +=1;
	if(k >=2)
	{
		k=0;
		Read_input_flag=1;
	}
			
	//``````````````````````
	//````````````````````````		
	if(Relay_1_release == 1)
	{
		r1[1] += 1;
		if(r1[1] > 250 )
		{
			r1[1] = 0;
			Relay_1_release = 0;
			
			Relay1_off(0);
			Relay1_on(0);
		}
	}
	//````````````````````````		
	if(Relay_2_release == 1)
	{
		r1[2] += 1;
		if(r1[2] > 250 )
		{
			r1[2] = 0;
			Relay_2_release = 0;
			
			Relay2_off(0);
			Relay2_on(0);
		}
	}	
	//````````````````````````		
	if(Relay_3_release == 1)
	{
		r1[3] += 1;
		if(r1[3] > 250 )
		{
			r1[3] = 0;
			Relay_3_release = 0;
			
			Relay3_off(0);
			Relay3_on(0);
		}
	}	
	//````````````````````````		
	if(Relay_4_release == 1)
	{
		r1[4] += 1;
		if(r1[4] > 250 )
		{
			r1[4] = 0;
			Relay_4_release = 0;
			
			Relay4_off(0);
			Relay4_on(0);
		}
	}		
		
}
 // get Temprture from adc
int GetTemperature(void)
{	
	const float         AVG_SLOPE   = 4.3E-03;      // slope (gradient) of temperature line function  [V/°C]
	const float         V25         = 1.43;         // sensor's voltage at 25°C [V]
	const float         ADC_TO_VOLT = 3.3 / 4096;   // conversion coefficient of digital value to voltage [V] 
								
															// when using 3.3V ref. voltage at 12-bit resolution (2^12 = 4096
	uint16_t            adcValue;                   // digital value of sensor
	float               vSense;                     // sensor's output voltage [V]
	float               temp;                       // sensor's temperature [°C]
	int temp_buffer=0;
	HAL_ADC_Start(&hadc1);                                      // start analog to digital conversion
	while(HAL_ADC_PollForConversion(&hadc1, 1000000) != HAL_OK);// wait for completing the conversion
	adcValue = HAL_ADC_GetValue(&hadc1);                        // read sensor's digital value
	vSense = adcValue * ADC_TO_VOLT;                            // convert sensor's digital value to voltage [V]
	/*
	* STM32F103xx Reference Manual:
	* 11.10 Temperature sensor
	* Reading the temperature, Page 235
	* Temperature (in °C) = {(V25 - Vsense) / Avg_Slope} + 25
	*/
	temp = (V25 - vSense) / AVG_SLOPE + 25.0f;                  // convert sensor's output voltage to temperature [°C]

//	hadc1.Init.NbrOfConversion=1;
//	HAL_ADC_Init(&hadc1);
//	HAL_ADC_Start(&hadc1);
//	
//	if(HAL_ADC_PollForConversion(&hadc1,500)==HAL_OK)
//	{
//		temp_adc=HAL_ADC_GetValue(&hadc1);
//		temp_buffer= (uint16_t)((V25-temp_adc)/Avg_Slope+25);
//		return temp_buffer;
//	
//	}
	return temp - 28;
}



//			bootloader function

void deinitEverything(void)
{
	//-- reset peripherals to guarantee flawless start of user application
	HAL_GPIO_DeInit(GPIOB ,GPIO_PIN_15 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_4 | EN_485_1_Pin|
										EN_485_3_Pin|en_595_Pin|OPTO_RL2_STOP_Pin | OPTO_RL2_RUN_Pin | KEY_Pin);
	
	HAL_GPIO_DeInit(GPIOA ,RADIO_M1_Pin|RADIO_M2_Pin|EN_485_2_Pin|LED_CPU_Pin|OPTO_RL3_RUN_Pin|
										OPTO_RL3_STOP_Pin|OPTO_RL4_RUN_Pin|OPTO_RL4_STOP_Pin);
	
	HAL_GPIO_DeInit(GPIOC,OPTO_RL1_STOP_Pin|OPTO_RL1_RUN_Pin|buzzer_Pin);
	
	HAL_UART_DeInit(&huart1);
//	HAL_UART_DeInit(&huart2);
	HAL_UART_DeInit(&huart3);
	
//	HAL_SPI_DeInit(&hspi1);
	HAL_SPI_DeInit(&hspi2);
	
	HAL_ADC_DeInit(&hadc1);
	HAL_ADC_DeInit(&hadc2);
	
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOA_CLK_DISABLE();
	HAL_RCC_DeInit();
	
	HAL_DeInit();
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;
}

//==============================================================
//===================USART IRQ============================
//void HAL_UART_RxCpltCallback (UART_HandleTypeDef *huart)
//{
void USART1_IRQHandler(void)
{
		U8 bf;
		U8 cnt;

		HAL_UART_IRQHandler(&huart1);	

		cnt = huart1.RxXferCount;

		bf = huart1.Instance->DR ;

		Serial1_RCV.buffer[Serial1_RCV.byte_count] = bf ;
		if(Serial1_RCV.byte_count >=100)
		{
			Serial1_RCV.byte_count = 0;
		}
		Serial1_RCV.byte_count +=1;
		Serial1_RCV.stop_time=0;
		Serial1_RCV.rcv_flag=1;	

		//HAL_UART_Receive_IT(&huart1,Rx_data,1);	
}
//===================END USART IRQ======================
//===================USART3 IRQ============================
/*
/TFC5-MTF200FL01V13
.1-0:31.7.0.255(000.00*A)
.1-0:1.8.2.255(000000.00*kWh)
1-0:1.8.1.255(000000.00*kWh)
1-0:3.8.0.255(000000.00*kVArh)
1-0:13.7.0.255(1.00)
1-0:32.7.0.255(235.14*V)
0-0:1.0.0.255(13990909164308)
1-0:15.6.0.255(00.000*kW0000000000)
1-0:1.8.0.255(000000.00*kWh)
1-0:1.8.4.255(000000.00*kWh)
1-0:2.8.0.255(000000.00*kWh)
.1-0:2.8.1.255(000000.00*kWh)
.1-0:1.8.3.255(000000.00*kWh)
.1-0:2.8.2.255(000000.00*kWh)
.1-0:2.8.3.255(000000.00*kWh)
1-0:0.0.0.255(18119813060875)
1-0:2.8.4.255(000000.00*kWh)
.1-0:3.8.1.255(000000.00*kVArh)
1-0:3.8.3.255(000000.00*kVArh)
1-0:3.8.2.255(000000.00*kVArh)
1-0:3.8.4.255(000000.00*kVArh)
1-0:4.8.2.255(000000.00*kVArh)
0-0:96.1.0.255(13060875)
1-0:15.7.0.255(000.00*kW)
1-0:4.8.1.255(000000.00*kVArh)
1-0:4.8.4.255(000000.00*kVArh)
1-0:1.7.0.255(000.00*kW)
1-0:3.7.0.255(000.00*kVAr)
.1-0:4.8.3.255(000000.00*kVArh)
1-0:14.7.0.255(49.92*Hz)
1-0:15.8.3.255(000000.00*kWh)
0-0:21.0.1.255(2F17141501020304000A1D19181A1B1C44455150FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF)
0-0:0.2.0.255(17120600)
0-0:96.20.6.255(139811291556)
1-0:15.8.2.255(000000.00*kWh)
1-0:15.8.1.255(000000.00*kWh)!
. 
*/
void USART3_IRQHandler(void)
{
		U8 bf;
		U8 cnt;
		U8 Rcv_485;
		U8 sts;
	
	 unsigned int IIR;

		IIR = USART3->SR;
		if(IIR & UART_FLAG_RXNE)
		{
			
			
			Rcv_485 = USART3->DR	; //Rx_data2[0];//
			
			Rcv_485 = Rcv_485 & 0x7f;			
			
			switch (Num_data_485)
			{
					case 1:
							{
								
									if (Rcv_485 == 02) // STX
									{
											Num_data_485 = 2;
										  Rcv_counter_485 = 0;

									}

									My_bcc = 0;
									break;
							}
					case 2:
							{
									My_bcc = My_bcc ^ Rcv_485;
								
									Rcv_array_485[Rcv_counter_485] = Rcv_485;
									Rcv_counter_485 +=1 ;
								
									if (Rcv_485 == 03) // ETX
									{
											Num_data_485 = 3;
									}
									break;
							}
					case 3:
							{

								Rcv_chksum = Rcv_485;
								Rcv_array_485[Rcv_counter_485] = 0;       // add 0 to end string
																													// Toggle Led_os4
								//Waitus[10];
								Rcv_complate_flag_485 = 1;
							
								Num_data_485 = 2;
								Rcv_counter_485 = 0;
								break;
							}
			}
			
		}
		
		
		HAL_UART_IRQHandler(&huart3);	
		//Rcv_485 = USART2->DR	;	
		
		HAL_UART_Receive_IT(&huart3,Rx_data2,1);	

}
//===================END USART3 IRQ======================
void UART3_SendString (unsigned char *s) 
{
	uint8_t c;

	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);
	//Delay(500);
	HAL_Delay(40);
	while (*s != 0) 
	{
		c=*s++ ;	
		HAL_UART_Transmit(&huart3,&c,1,HAL_MAX_DELAY);
	//Delay(5);

	}
	//Delay(500);
	HAL_Delay(40);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);


	//UART2_SendByte(*s++);
	//DELAY_us(1000);
}

//========================================================

void UART2_SendString (unsigned char *s) 
{
	uint8_t c;
	
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
	 Delay(500);
  	while (*s != 0) 
	 {
		 c=*s++ ;	
		 HAL_UART_Transmit(&huart2,&c,1,HAL_MAX_DELAY);
		 //Delay(5);
	 }
		Delay(500);	
		 HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);

		
   	//UART2_SendByte(*s++);
		//DELAY_us(1000);
}

unsigned short make_int(unsigned char  low,unsigned char  high)
{
	unsigned short _d;
	_d = (high <<8);
	_d = _d | low;
	return _d;
}

//===================================================================
void Relay_trig(char relay_number,char State,char Status)
{
    char Led_sts;
    Led_sts = State;
    Relay_status[relay_number] = Status;
	
	// below code is for trig relaies ro three times
//	relay_order_number = relay_number;
//	relay_order_state = State;
//	relay_order_flag = 1;
		Stop_adc =1; // this vriable cause adc be stop
    switch (relay_number)
    {
        case 1:
            { // flash address 200 , 201 , 202 , 203 -> ralys in order 1 , 2 , 3 , 4
								Led_os1(Led_sts);
                if ((Relay_last_state[relay_number] != State))
                {
										Spi_Flash_write_byte(200,Status); // write relay status 1 in ic Flash
                    Relay_last_state[relay_number] = State;
										Modbus_buffer[124] = State;
									  Spi_Flash_write_word(47,State);
									
                    if (State == 0)//1
                    {
                        Relay1_off(0);
                        Relay1_on(1);	
											
												HAL_Delay(200);
  
                        Relay1_off(0);
                        Relay1_on(1);	
											
  											HAL_Delay(200);
  
                        Relay1_off(0);
                        Relay1_on(1);	
	                  }
                    else
                    {
                        Relay1_off(1);
                        Relay1_on(0);
											
												HAL_Delay(200);
											
                        Relay1_off(1);
                        Relay1_on(0);
											
												HAL_Delay(200);
											
                        Relay1_off(1);
                        Relay1_on(0);
											
												HAL_Delay(200);
											
                        Relay1_off(1);
                        Relay1_on(0);
                    }
                }

                Relay_1_release =1;
                break;
            }

        case 2:
            {
							Led_os2(Led_sts);
                if ((Relay_last_state[relay_number] != State))
                {
										Spi_Flash_write_byte(201,Status); // write relay status 2 in ic Flash
                    Relay_last_state[relay_number] = State;
									  Spi_Flash_write_word(57,State);
										Modbus_buffer[129] = State;
									
                    if (State == 0)//1
                    {
                        Relay2_off(0);
                        Relay2_on (1);
											
												HAL_Delay(200);
											
                        Relay2_off(0);
                        Relay2_on (1);
											
												HAL_Delay(200);
											
                        Relay2_off(0);
                        Relay2_on (1);	
											
												HAL_Delay(200);
											
                        Relay2_off(0);
                        Relay2_on (1);	
										}
                    else
                    {
                        Relay2_off (1);
                        Relay2_on (0);	
											
												HAL_Delay(200);
											
                        Relay2_off (1);
                        Relay2_on (0);	
											
												HAL_Delay(200);
											
                        Relay2_off (1);
                        Relay2_on (0);
											
												HAL_Delay(200);
											
                        Relay2_off (1);
                        Relay2_on (0);	
                    }
                }

                Relay_2_release=1;
                break;
            }

        case 3:
            {
                Led_os3(Led_sts);
                if ((Relay_last_state[relay_number] != State))
                {
										Spi_Flash_write_byte(202,Status); // write relay status 2 in ic Flash
                    Relay_last_state[relay_number] = State;
									  Spi_Flash_write_word(67,State);
										Modbus_buffer[134] = State;
									
                    if (State == 0)//1
                    {
                        Relay3_off(0);
                        Relay3_on(1);												
											
												HAL_Delay(200);
											
                        Relay3_off(0);
                        Relay3_on(1);		
											
												HAL_Delay(200);
											
                        Relay3_off(0);
                        Relay3_on(1);		
											
												HAL_Delay(200);
											
                        Relay3_off(0);
                        Relay3_on(1);		
                    }
                    else
                    {
                        Relay3_off (1);
                        Relay3_on (0);	
											
												HAL_Delay(200);
											
                        Relay3_off (1);
                        Relay3_on (0);	
											
												HAL_Delay(200);
											
                        Relay3_off (1);
                        Relay3_on (0);												
											
												HAL_Delay(200);
											
                        Relay3_off (1);
                        Relay3_on (0);	
                    }
                }

                Relay_3_release =1;
                break;
            }

        case 4:
            {
                Led_os4(Led_sts);
                if ((Relay_last_state[relay_number] != State))
                {
									Spi_Flash_write_byte(203,Status); // write relay status 3 in ic Flash
									flag_s=0;
									
                    Relay_last_state[relay_number] = State;
									  Spi_Flash_write_word(77,State);
									  Modbus_buffer[139] = State;
									
                    if (State == 0)//1
                    {
                        Relay4_off (0);
                        Relay4_on (1);	
											
												HAL_Delay(200);
											
                        Relay4_off (0);
                        Relay4_on (1);	
											
												HAL_Delay(200);
											
                        Relay4_off (0);
                        Relay4_on (1);	
											
												HAL_Delay(200);
											
                        Relay4_off (0);
                        Relay4_on (1);	
                    }
                    else
                    {
                        Relay4_off (1);
                        Relay4_on (0);	
											
												HAL_Delay(200);
											
                        Relay4_off (1);
                        Relay4_on (0);											
											
												HAL_Delay(200);
											
                        Relay4_off (1);
                        Relay4_on (0);	
											
												HAL_Delay(200);
											
                        Relay4_off (1);
                        Relay4_on (0);	
										}
                }

                Relay_4_release =1;
                break;
            }
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void led(char num,char val)
{
	U8 ls;
	char lc[1];

	ls= pow(2,num);  	
	led_command[0]= led_command[0] ^ ls;
  lc[0] = ~ led_command[0];
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET) ;
	HAL_SPI_Transmit(&hspi2,(uint8_t*)&lc,1,10);
}

void Led_com(char val)
{
	U8 ls;
	char lc[1];

	if(val==1)
		led_command[0] |= (1<<2);
	else
		led_command[0] &= ~(1<<2);
	
	lc[0] = ~ led_command[0];
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET) ;
	HAL_SPI_Transmit(&hspi2,(uint8_t*)&lc,1,10);
}

void Led_pmr(char val)
{
	U8 ls;
	char lc[1];

	if(val==1)
		led_command[0] |= (1<<1);
	else
		led_command[0] &= ~(1<<1);
	
	lc[0] = ~ led_command[0];
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET) ;
	HAL_SPI_Transmit(&hspi2,(uint8_t*)&lc,1,10);
}

void Led_os1(char val)
{
	U8 ls;
	char lc[1];

	if(val==1)
		led_command[0] |= (1<<3);
	else
		led_command[0] &= ~(1<<3);
	
	lc[0] = ~ led_command[0];
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET) ;
	HAL_SPI_Transmit(&hspi2,(uint8_t*)&lc,1,10);
}

void Led_os2(char val)
{
	U8 ls;
	char lc[1];

	if(val==1)
		led_command[0] |= (1<<4);
	else
		led_command[0] &= ~(1<<4);
	
	lc[0] = ~ led_command[0];
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET) ;
	HAL_SPI_Transmit(&hspi2,(uint8_t*)&lc,1,10);
}

void Led_os3(char val)
{
	U8 ls;
	char lc[1];

	if(val==1)
		led_command[0] |= (1<<5);
	else
		led_command[0] &= ~(1<<5);
	
	lc[0] = ~ led_command[0];
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET) ;
	HAL_SPI_Transmit(&hspi2,(uint8_t*)&lc,1,10);
}

void Led_os4(char val)
{
	U8 ls;
	char lc[1];

	if(val==1)
		led_command[0] |= (1<<6);
	else
		led_command[0] &= ~(1<<6);
	
	lc[0] = ~ led_command[0];
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET) ;
	HAL_SPI_Transmit(&hspi2,(uint8_t*)&lc,1,10);
}


void Radio_config()
{
	U8 send_buf[10];
	MX_USART1_UART_Init(9600);
	
	Radio_m0(1);
	Radio_m1(1);
	
	HAL_Delay(500);
	send_buf[0]=0xc0;
	send_buf[1]= 0;
	send_buf[2]= 0;
	send_buf[3]= 0x25;
	send_buf[4]= 0x03; //<-------- BAND
	send_buf[5]= 0x47;
	
	HAL_UART_Transmit(&huart1,send_buf,6,HAL_MAX_DELAY);
	
	HAL_Delay(100);
	
	MX_USART1_UART_Init(19200);

	Radio_m0(0);
	Radio_m1(0);
	
	Modbus_buffer[171]=send_buf[4];
}




void Proces_rcv_string(char Index)
{
	
   // 1- 18119713956891
   // 2- 000000.00*kWh
   // 3- 000000.00*kWh
   // 4- 000000.00*kWh
   // 5- 000000.00*kVArh
   // 6- 1.00
   // 7- 226.76*V
   // 8- 000.00*A
   // 9- 13981224012936
   // 10- 00.000 * Kw0000000000
   // Read Response Variables
	
   char T_string[20]; 
	 char T_str[20];
   float T_single;
   U32 T_dword;
   U16 T_word;
   U8 I1;
   U8 J1;
   U8 B1;
   U8 B2;
   U8 B3;
   U8 B4;
   float Voltage_value;
  // float Current_value;

   U16 W1[6];
   char S1[3];
   char S2[3]; 

    // ```````Check Charecter Validity```````````
//		strcpy(T_string,"");
//		//strcpy(T_str , rcv_string[7]);
//		strncpy(T_string,rcv_string[7] +6,1)	;	
//		
//		if(strstr(T_string, "V") == NULL)
//       return;
    
		// ````````````````````````````````````````````
		// for behin meter i got it up  only this seg
//    // ```````KVAR````````````````````````
////		B1 = Index - 1;
////		B1 = 20 * B1;
////		B1 = B1 + 20;
//		T_dword = chang_value(rcv_string[5]);
//    Modbus_buffer[B1 +11] = T_dword >> 16 ;       // Tarif1_high
//    Modbus_buffer[B1 +12] = T_dword & 0x0000ffff;       // Tarif1_low
		
//		if(behineh_meter_round[read_meter_counter]==3){
//			behineh_meter_round[read_meter_counter]=0;
//			return;
//		}
		
		J1 = 0;    
		if(Modbus_buffer[164+read_meter_counter]==Behineh)
			sprintf(T_string,"0000%s",rcv_string[1]);
		else
			sprintf(T_string,"000000%s",rcv_string[1]);
		
    for (I1 = 0; I1 <= 20; I1 += 4)
    {       
				strncpy(S1,T_string+I1,2);
        B1 = I1 + 2;      
        strncpy(S2,T_string+B1,2);
        B1 = atoi(S1);
        B2 = atoi(S2);
			  J1 += 1;
				W1[J1] = (B1 <<8);
				W1[J1] |= B2;
    }
    B1 = Index - 1;
    B1 = 20 * B1;
    B1 = B1 + 20;

    Modbus_buffer[B1] = W1[1];           // serial number 1,2 - 3,4
    Modbus_buffer[B1 + 1] = W1[2];       // serial number 5,6 - 7,8
    Modbus_buffer[B1 + 2] = W1[3];       // serial number 9,10 - 11,12
    Modbus_buffer[B1 + 3] = W1[4];       // serial number 13,14 - 15,16
    Modbus_buffer[B1 + 4] = W1[5];       // serial number 17,18 - 19-20

    // ``````Trif 1````````````````````````
		T_dword = chang_value(rcv_string[2]);

    Modbus_buffer[B1 + 5] = T_dword >> 16 ;       // Tarif1_high
    Modbus_buffer[B1 + 6] = T_dword & 0x0000ffff;       // Tarif1_low

    //```````Tarif 2````````````````````````
		T_dword = chang_value(rcv_string[3]);
    Modbus_buffer[B1 + 7] = T_dword >> 16 ;       // Tarif1_high
    Modbus_buffer[B1 + 8] = T_dword & 0x0000ffff;       // Tarif1_low

    // ```````Tarif 3````````````````````````
		T_dword = chang_value(rcv_string[4]);
    Modbus_buffer[B1 + 9] = T_dword >> 16 ;       // Tarif1_high
    Modbus_buffer[B1 + 10] = T_dword & 0x0000ffff;       // Tarif1_low
    // ```````KVAR````````````````````````
		if(Modbus_buffer[164+read_meter_counter]!=Behineh)
		{
			T_dword = chang_value(rcv_string[5]);
			Modbus_buffer[B1 +11] = T_dword >> 16 ;       // Tarif1_high
			Modbus_buffer[B1 +12] = T_dword & 0x0000ffff;       // Tarif1_low
		}
		// ```````COS Fi````````````````````````
		T_word = chang_value_16(rcv_string[6]);
    Modbus_buffer[B1 +13] = T_word;     
  
   // ```````Voltage ````````````````````````
//		for(I1=0;I1<20;I1++)
//			T_string[I1]=0;
//			
//		
//		strncpy(T_string , rcv_string[7],6);
//		T_word = chang_value(T_string);

		HAL_Delay(1);
		if(flag_stop_meter_get_voltage==0)
		{
			T_word = chang_value_16(rcv_string[7]);
			Modbus_buffer[B1 + 14] = T_word/100 ;
			Voltage_value_meter[Index] = T_word/100 ;
		
			if((adc_buff[Index]>=ADC_LIMIT_VOLTAGE)) // mux init 
			{
				mux[Index-1] = adc_channel_voltage[Index-1] - Voltage_value_meter[Index];
			}
			if(mux[Index-1] < 0) // doesnt let it which mux be lower then zero
			{
				mux[Index-1] = 0;
			}
		}
		//---------------[calibration voltage]------
//		mux[Index-1] = (float)((float)channels_adc[Index-1]*100)/Voltage_value_meter[Index];		
    
		// ```````Current ````````````````````````
		
		T_word = chang_value_16(rcv_string[8]);
    Modbus_buffer[B1 +15] = T_word  ;  
		
		Current_value[Index-1] = T_word  ; 
		
		//previce_current[Index-1] = T_word;
    // ``````````Year```````````
//    1400 01 26 18 11 51

			strcpy(T_string,"");
			for(I1=0;I1<20;I1++)
				T_string[I1]=0;
			
			strncpy(T_string,rcv_string[9],4);
			T_word = atoi(T_string);
			Modbus_buffer[B1 + 16] = T_word;
			
    // ``````````Mounth And Day```````````
			strcpy(S1,"");
			strcpy(S2,"");
			//strncpy(S1,rcv_string[9]+4,2);
			S1[0] = rcv_string[9][4];
			S1[1] = rcv_string[9][5];
			//strncpy(S2,(rcv_string[9]+6),2);
			S2[0] = rcv_string[9][6];
			S2[1] = rcv_string[9][7];
			B3 = atoi(S1); // months values
			B4 = atoi(S2); // days values
			T_word = B3 << 8 ;
			T_word |= B4;
      Modbus_buffer[B1 + 17] = T_word;
    // ````````Houre and Miniute```````````

			strcpy(S1,"");
			strcpy(S2,"");
			//strncpy(S1,rcv_string[9]+9,2);
			S1[0] = rcv_string[9][8];
			S1[1] = rcv_string[9][9];
			//strncpy(S2,rcv_string[9]+11,2);
			S2[0] = rcv_string[9][10];
			S2[1] = rcv_string[9][11];
			B3 = atoi(S1);
			B4 = atoi(S2);
			T_word = B3 << 8 ;
			T_word |= B4;
			Modbus_buffer[B1 + 18] = T_word;
    // ````````````````````````````````````````
		for(I1=0;I1<40;I1++) // make buffer to null
		{
			rcv_string[0][I1]= '\0';	rcv_string[1][I1]= '\0';
			rcv_string[2][I1]= '\0';	rcv_string[3][I1]= '\0';
			rcv_string[4][I1]= '\0';	rcv_string[5][I1]= '\0';
			rcv_string[6][I1]= '\0';	rcv_string[7][I1]= '\0';
			rcv_string[8][I1]= '\0';	rcv_string[9][I1]= '\0';
			rcv_string[10][I1]= '\0';	rcv_string[11][I1]= '\0';
			rcv_string[12][I1]= '\0';	rcv_string[13][I1]= '\0';
			rcv_string[14][I1]= '\0';	rcv_string[15][I1]= '\0';
		}
    // ```````````CHECK VALUE`````````````````
//    B1 = Index - 1;
//    B1 = 5 * B1;
//    B1 = B1 + 120;

    // 1 = Normal , 2=Over Voltage , 3=Lower Voltage , 4= P.M Disconnect
    Power_meter_status[Index] = Normal;

}

U32 chang_value(char *s)
{
	char a;
	char l ;
	char i_n[10]={0,0,0,0,0,0,0,0,0,0};
	char d_n[10]={0,0,0,0,0,0,0,0,0,0};
	U32 b1;
	U32 b2;
//	l= strlen(s);
//	for(a=0 ; a< l ; a++)
//	{
//		strncpy(b, s+a , 1);
//		if( b[0] == '.')
//		{
//			p=a;
//			break;
//		}
//	}
//	strncpy(i_n , s , p);
//	strncpy(d_n , s + p + 1 , l);

	for(a=0 ;(*(s+a)!='\0')&&(a!=10); a++)
	{
		if((*(s+a))== '.')
			break;
		i_n[a] = *(s+a);
	}
	b1=atoi(i_n);
	for(l=++a;(*(s+a)!='\0')&&(a!=10);a++)
	{
		d_n[a-l] = *(s+a);
		b1 *= 10;
	}
	b2=atoi(d_n);
	b1 += b2;
	HAL_Delay(1);
	return b1;	
}


//=============================
U16 chang_value_16(char *s)
{
	char a;
	U16 b1;
	U16 b2;
	char l ;
	char b[1];
	char p;
	
	char i_n[10]={0,0,0,0,0,0,0,0,0,0};
	char d_n[10]={0,0,0,0,0,0,0,0,0,0};
	
	
	l= strlen(s);
	p=0;
	for(a=0 ; a< l ; a++)
	{
		strncpy(b, s+a , 1);
		if( b[0] == '.')
		{
			p=a;
			break;
		}
	}
	strncpy(i_n , s , p);
	strncpy(d_n , s + p + 1 , l);
	b1=atoi(i_n);
	b2=atoi(d_n);
	
	b1=(b1*100) + b2 ;
	
	return b1;	
}

void Read_config()
{
//	Modbus_buffer[111] = Spi_Flash_read_word(11); //meter Count
//	meter_count = Modbus_buffer[111];
	
	Modbus_buffer[120] = Spi_Flash_read_word(39);		//Amper Limit Disbla or Enable
	Modbus_buffer[121] = Spi_Flash_read_word(41);		//Amper Set for P.m #1
	Modbus_buffer[122] = Spi_Flash_read_word(43);		//High Volt Set for P.m #1
	Modbus_buffer[123] = Spi_Flash_read_word(45);		//Low Volt set for P.m#1
	Modbus_buffer[124] = Spi_Flash_read_word(47);		//Relay #1 -> 1=ON , 2-> off
	if(Modbus_buffer[124]==2)
		Relay_last_state[1]=1;
	else
		Relay_last_state[1]=0;
	
	Modbus_buffer[125] = Spi_Flash_read_word(49);		//Amper Limit Disbla or Enable
	Modbus_buffer[126] = Spi_Flash_read_word(51);		//Amper Set for P.m #1
	Modbus_buffer[127] = Spi_Flash_read_word(53);		//High Volt Set for P.m #1
	Modbus_buffer[128] = Spi_Flash_read_word(55);		//Low Volt set for P.m#1
	Modbus_buffer[129] = Spi_Flash_read_word(57);		//Relay #1 -> 1=ON , 2-> off	
	if(Modbus_buffer[129]==2)
		Relay_last_state[2]=1;
	else
		Relay_last_state[2]=0;

	
	Modbus_buffer[130] = Spi_Flash_read_word(59);		//Amper Limit Disbla or Enable		
	Modbus_buffer[131] = Spi_Flash_read_word(61);		//Amper Set for P.m #1
	Modbus_buffer[132] = Spi_Flash_read_word(63);		//High Volt Set for P.m #1
	Modbus_buffer[133] = Spi_Flash_read_word(65);		//Low Volt set for P.m#1
	Modbus_buffer[134] = Spi_Flash_read_word(67);		//Relay #1 -> 1=ON , 2-> off
	if(Modbus_buffer[134]==2)
		Relay_last_state[3]=1;
	else
		Relay_last_state[3]=0;

	
	Modbus_buffer[135] = Spi_Flash_read_word(69);		//Amper Limit Disbla or Enable		
	Modbus_buffer[136] = Spi_Flash_read_word(71);		//Amper Set for P.m #1
	Modbus_buffer[137] = Spi_Flash_read_word(73);		//High Volt Set for P.m #1
	Modbus_buffer[138] = Spi_Flash_read_word(75);		//Low Volt set for P.m#1
	Modbus_buffer[139] = Spi_Flash_read_word(77);		//Relay #1 -> 1=ON , 2-> off
	if(Modbus_buffer[139]==2)
		Relay_last_state[4]=1;
	else
		Relay_last_state[4]=0;
	
	//put meter type on modbus buffer
	Modbus_buffer[165] = Spi_Flash_read_word(129);
	Modbus_buffer[166] = Spi_Flash_read_word(131);
	Modbus_buffer[167] = Spi_Flash_read_word(133);
	Modbus_buffer[168] = Spi_Flash_read_word(135);
	
	Modbus_buffer[140] = Spi_Flash_read_word(79);		//Serial1 number 1 -contor 1
	Modbus_buffer[141] = Spi_Flash_read_word(81);		//Serial1 number 1 -contor 1
	Modbus_buffer[142] = Spi_Flash_read_word(83);		//Serial1 number 1 -contor 1
	Modbus_buffer[143] = Spi_Flash_read_word(85);		//Serial1 number 1 -contor 1
	Modbus_buffer[144] = Spi_Flash_read_word(87);		//Serial1 number 1 -contor 1
	
	Modbus_buffer[145] = Spi_Flash_read_word(89);		//Serial1 number 2 -contor 2
	Modbus_buffer[146] = Spi_Flash_read_word(91);		//Serial1 number 2 -contor 2
	Modbus_buffer[147] = Spi_Flash_read_word(93);		//Serial1 number 2 -contor 2
	Modbus_buffer[148] = Spi_Flash_read_word(95);		//Serial1 number 2 -contor 2
	Modbus_buffer[149] = Spi_Flash_read_word(97);		//Serial1 number 2 -contor 2

	Modbus_buffer[150] = Spi_Flash_read_word(99);		//Serial1 number 3 -contor 3
	Modbus_buffer[151] = Spi_Flash_read_word(101);		//Serial1 number 3 -contor 3
	Modbus_buffer[152] = Spi_Flash_read_word(103);		//Serial1 number 3 -contor 3
	Modbus_buffer[153] = Spi_Flash_read_word(105);		//Serial1 number 3 -contor 3
	Modbus_buffer[154] = Spi_Flash_read_word(107);		//Serial1 number 3 -contor 3
	
	Modbus_buffer[155] = Spi_Flash_read_word(109);		//Serial1 number 4 -contor 4
	Modbus_buffer[156] = Spi_Flash_read_word(111);		//Serial1 number 4 -contor 4
	Modbus_buffer[157] = Spi_Flash_read_word(113);		//Serial1 number 4 -contor 4
	Modbus_buffer[158] = Spi_Flash_read_word(115);		//Serial1 number 4 -contor 4
	Modbus_buffer[159] = Spi_Flash_read_word(117);		//Serial1 number 4 -contor 4	
	
	Modbus_buffer[101] = Spi_Flash_read_word(1);		//Temperture Level 1
	Modbus_buffer[102] = Spi_Flash_read_word(3);		//Temperture Level 2
	Modbus_buffer[103] = Spi_Flash_read_word(5);		//Temperture Level 3

//	Modbus_buffer[160] = Spi_Flash_read_word(119);
//	Modbus_buffer[161] = Spi_Flash_read_word(121);
//	Modbus_buffer[162] = Spi_Flash_read_word(123);
//	Modbus_buffer[163] = Spi_Flash_read_word(125);	
	
	Password = 123 ;

	Make_serial_number(144);
	Make_serial_number(149);
	Make_serial_number(154);
	Make_serial_number(159);
	
	// read realy_status from flash ic it causes reaturn relay [200] to [203]
	Relay_status[1] = Spi_Flash_read_byte(200);
	Relay_status[2] = Spi_Flash_read_byte(201);
	Relay_status[3] = Spi_Flash_read_byte(202);
	Relay_status[4] = Spi_Flash_read_byte(203);
	// read current_falut from flash ic . address alocated [204] to [207]
	current_fault[0] = Spi_Flash_read_byte(204);
	current_fault[1] = Spi_Flash_read_byte(205);
	current_fault[2] = Spi_Flash_read_byte(206);
	current_fault[3] = Spi_Flash_read_byte(207);
	//read current over status 3 times from flash ic . address alocated [208] to [211]
	Modbus_buffer[5] = Spi_Flash_read_byte(208);
	Modbus_buffer[6] = Spi_Flash_read_byte(209);
	Modbus_buffer[7] = Spi_Flash_read_byte(210);
	Modbus_buffer[8] = Spi_Flash_read_byte(211);
}

//====================================================================
void  Restor_default()
{
	
	Modbus_buffer[101]=70;
	Modbus_buffer[102]=80;
	Modbus_buffer[103]=100;
	
	Modbus_buffer[120] = 1;
	Modbus_buffer[121] = 3500;
	Modbus_buffer[122] = 300;
	Modbus_buffer[123] = 150;
	//Modbus_buffer[124] = 0;
	Modbus_buffer[124] = 1;
	
	Modbus_buffer[125] = 1;
	Modbus_buffer[126] = 3500;
	Modbus_buffer[127] = 300;
	Modbus_buffer[128] = 150;
	//Modbus_buffer[129] = 0;
	Modbus_buffer[129] = 1;

	Modbus_buffer[130] = 1;
	Modbus_buffer[131] = 3500;
	Modbus_buffer[132] = 300;
	Modbus_buffer[133] = 150;
	//Modbus_buffer[134] = 0;
	Modbus_buffer[134] = 1;


	Modbus_buffer[135] = 1;
	Modbus_buffer[136] = 3500;
	Modbus_buffer[137] = 300;
	Modbus_buffer[138] = 150;
	//Modbus_buffer[139] = 0;
	Modbus_buffer[139] = 1;
	
	
//	Modbus_buffer[101] = 330;
//	Modbus_buffer[121] = 510;
//	Modbus_buffer[122] = 8000;
	
  Modbus_buffer[105] = 1;
	Modbus_buffer[106] = 3500;
	Modbus_buffer[107] = 300;
	Modbus_buffer[108] = 150;
	Modbus_buffer[109] = 1;
	
	Modbus_buffer[111] = 4;
	/*
	Modbus_buffer[140] = 0;	
	Modbus_buffer[141] = 0;	
	Modbus_buffer[142] = 0;	
	Modbus_buffer[143] = 0;	

	Modbus_buffer[144] = 0;	
	Modbus_buffer[145] = 0;	
	Modbus_buffer[146] = 0;	
	Modbus_buffer[147] = 0;	

	Modbus_buffer[148] = 0;	
	Modbus_buffer[149] = 0;	
	Modbus_buffer[150] = 0;	
	Modbus_buffer[151] = 0;	

	Modbus_buffer[152] = 0;	
	Modbus_buffer[153] = 0;	
	Modbus_buffer[154] = 0;	
	Modbus_buffer[155] = 0;		
	*/

  Spi_Flash_write_word(39,Modbus_buffer[120]);
	Spi_Flash_write_word(41,Modbus_buffer[121]);
	Spi_Flash_write_word(43,Modbus_buffer[122]);
	Spi_Flash_write_word(45,Modbus_buffer[123]);
	Spi_Flash_write_word(47,Modbus_buffer[124]);
	
  Spi_Flash_write_word(49,Modbus_buffer[125]);
	Spi_Flash_write_word(51,Modbus_buffer[126]);
	Spi_Flash_write_word(53,Modbus_buffer[127]);
	Spi_Flash_write_word(55,Modbus_buffer[128]);
	Spi_Flash_write_word(57,Modbus_buffer[129]);

  Spi_Flash_write_word(59,Modbus_buffer[130]);
	Spi_Flash_write_word(61,Modbus_buffer[131]);
	Spi_Flash_write_word(63,Modbus_buffer[132]);
	Spi_Flash_write_word(65,Modbus_buffer[133]);
	Spi_Flash_write_word(67,Modbus_buffer[134]);

  Spi_Flash_write_word(69,Modbus_buffer[135]);
	Spi_Flash_write_word(71,Modbus_buffer[136]);
	Spi_Flash_write_word(73,Modbus_buffer[137]);
	Spi_Flash_write_word(75,Modbus_buffer[138]);
	Spi_Flash_write_word(77,Modbus_buffer[139]);

  Spi_Flash_write_word(79,Modbus_buffer[140]);
	Spi_Flash_write_word(81,Modbus_buffer[141]);
	Spi_Flash_write_word(83,Modbus_buffer[142]);
	Spi_Flash_write_word(85,Modbus_buffer[143]);
	Spi_Flash_write_word(87,Modbus_buffer[144]);
	
	Spi_Flash_write_word(89,Modbus_buffer[145]);
	Spi_Flash_write_word(91,Modbus_buffer[146]);
	Spi_Flash_write_word(93,Modbus_buffer[147]);
	Spi_Flash_write_word(95,Modbus_buffer[148]);
	Spi_Flash_write_word(97,Modbus_buffer[149]);
	
	Spi_Flash_write_word(99,Modbus_buffer[150]);
	Spi_Flash_write_word(101,Modbus_buffer[151]);
	Spi_Flash_write_word(103,Modbus_buffer[152]);
	Spi_Flash_write_word(105,Modbus_buffer[153]);
	Spi_Flash_write_word(107,Modbus_buffer[154]);
	
	
  Spi_Flash_write_word(109,Modbus_buffer[155]);
	Spi_Flash_write_word(111,Modbus_buffer[156]);
	Spi_Flash_write_word(113,Modbus_buffer[157]);
	Spi_Flash_write_word(115,Modbus_buffer[158]);
	Spi_Flash_write_word(117,Modbus_buffer[159]);
	
  Spi_Flash_write_word(1,Modbus_buffer[101]);
	Spi_Flash_write_word(3,Modbus_buffer[102]);
	Spi_Flash_write_word(5,Modbus_buffer[103]);
	
	Spi_Flash_write_word(9,Modbus_buffer[105]);
	Spi_Flash_write_word(11,Modbus_buffer[106]);
	Spi_Flash_write_word(13,Modbus_buffer[107]);
	Spi_Flash_write_word(15,Modbus_buffer[108]);

	Spi_Flash_write_word(21,Modbus_buffer[111]);
	
	
	Modbus_buffer[160] = 0;
	Spi_Flash_write_word(119,0);

	Modbus_buffer[161] = 0;
	Spi_Flash_write_word(121,0);
	
	Modbus_buffer[162] = 0;
	Spi_Flash_write_word(123,0);
	
	Modbus_buffer[163] = 0;
	Spi_Flash_write_word(125,0);
		
	// read realy_status from flash ic it causes reaturn relay 
	Relay_status[1] = 0;
	Relay_status[2] = 0;
	Relay_status[3] = 0;
	Relay_status[4] = 0;
	// read current_falut from flash ic . address alocated [204] to [207]
	current_fault[0] = 0;
	current_fault[1] = 0;
	current_fault[2] = 0;
	current_fault[3] = 0;
	//read current over status 3 times from flash ic . address alocated [208] to [211]
	Modbus_buffer[5] = 0;
	Modbus_buffer[6] = 0;
	Modbus_buffer[7] = 0;
	Modbus_buffer[8] = 0;
	// goes to zero for statuses
	Spi_Flash_write_word(200,0);
	Spi_Flash_write_word(201,0);
	Spi_Flash_write_word(202,0);
	Spi_Flash_write_word(203,0);
	
	Spi_Flash_write_word(204,0);
	Spi_Flash_write_word(205,0);
	Spi_Flash_write_word(206,0);
	Spi_Flash_write_word(207,0);
	
	Spi_Flash_write_word(208,0);
	Spi_Flash_write_word(209,0);
	Spi_Flash_write_word(210,0);
	Spi_Flash_write_word(211,0);
//		//put meter type on modbus buffer
//	Spi_Flash_write_word(129,0);
//	Spi_Flash_write_word(131,0);
//	Spi_Flash_write_word(133,0);
//	Spi_Flash_write_word(135,0);
//	
//	behineh_meter_round[1]=behineh_meter_round[2]=behineh_meter_round[3]=behineh_meter_round[4]=0;
//	Modbus_buffer[165]=Modbus_buffer[166]=Modbus_buffer[167]=Modbus_buffer[168]=0;
	
	meter_count = 4;
	Password = 123 ;
}
//========================================================================

void  Make_serial_number(char Reg_add)
{
	U8 rg;
	U16 D;
	U16 t1;
	U16 t2;
	char ts[20];
	char s1[3];
	char s2[3];
	char cnt_type;
	
	rg = Reg_add - 4;
	
	strcpy(ts,"/?");
	
	if(rg == 140)
		cnt_type = Modbus_buffer[165];
	else if(rg == 145)
		cnt_type = Modbus_buffer[166];
	else if(rg == 150)
		cnt_type = Modbus_buffer[167];
	else if(rg == 155)
		cnt_type = Modbus_buffer[168];
	//-------------
	switch(cnt_type)
	{
		case Toos_9600_small:
		case Toos_9600:
		case Toos_v4:
			D = Modbus_buffer[rg+1];
			//t1 = D >> 8;
			t2 = D & 0x00ff ;
			//sprintf(s1,"%02d",t1);
			sprintf(s1,"%02d",t2);
			
			//strcat(ts,s1);
			strcat(ts,s1);
			//-----------------
			
			//-------------
			D = Modbus_buffer[rg+2];
			t1 = D >> 8;
			t2 = D & 0x00ff ;
			sprintf(s1,"%02d",t1);
			sprintf(s2,"%02d",t2);
			
			strcat(ts,s1);
			strcat(ts,s2);
			//-----------------
			//-------------
			D = Modbus_buffer[rg+3];
			t1 = D >> 8;
			t2 = D & 0x00ff ;
			sprintf(s1,"%02d",t1);
			sprintf(s2,"%02d",t2);
			
			strcat(ts,s1);
			strcat(ts,s2);
			//-----------------	
			//-------------
			D = Modbus_buffer[rg+4];
			t1 = D >> 8;
			t2 = D & 0x00ff ;
			sprintf(s1,"%02d",t1);
			sprintf(s2,"%02d",t2);
			
			strcat(ts,s1);
			strcat(ts,s2);
		break;
		case Toos_4800:
		
			D = Modbus_buffer[rg+3];
			t1 = D >> 8;
			t2 = D & 0x00ff;
			sprintf(s1,"%02d",t1);
			sprintf(s2,"%02d",t2);
			strcat(ts,s1);
			strcat(ts,s2);
			//---------------
			D = Modbus_buffer[rg+4];
			t1 = D >> 8;
			t2 = D & 0x00ff;
			sprintf(s1,"%02d",t1);
			sprintf(s2,"%02d",t2);
			strcat(ts,s1);
			strcat(ts,s2);
			//---------------
		break;
		case Behineh:
		
			D = Modbus_buffer[rg+1];
			t1 = D >> 8;
			t2 = D & 0x00ff;
			sprintf(s1,"%02d",t1);
			sprintf(s2,"%02d",t2);
			strcat(ts,s1);
			strcat(ts,s2);
			//---------------		
			D = Modbus_buffer[rg+2];
			t1 = D >> 8;
			t2 = D & 0x00ff;
			sprintf(s1,"%02d",t1);
			sprintf(s2,"%02d",t2);
			strcat(ts,s1);
			strcat(ts,s2);
			//---------------		
			D = Modbus_buffer[rg+3];
			t1 = D >> 8;
			t2 = D & 0x00ff;
			sprintf(s1,"%02d",t1);
			sprintf(s2,"%02d",t2);
			strcat(ts,s1);
			strcat(ts,s2);
			//---------------		
			D = Modbus_buffer[rg+4];
			t1 = D >> 8;
			t2 = D & 0x00ff;
			sprintf(s1,"%02d",t1);
			sprintf(s2,"%02d",t2);
			strcat(ts,s1);
			strcat(ts,s2);
			//---------------
		
		break;
		//-----------------	
	}
  strcat(ts,"!\r\n");
	
	if(rg == 140)
		strcpy(Meter_id[1],ts);
	
	else if(rg == 145)
		strcpy(Meter_id[2],ts);

	else if(rg == 150)
		strcpy(Meter_id[3],ts);

	else if(rg == 155)
		strcpy(Meter_id[4],ts);
}


/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
//void MX_ADC1_Init(void)
//{

//  ADC_ChannelConfTypeDef sConfig;

//    /**Common config
//    */
//  hadc1.Instance = ADC1;
//  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
//  hadc1.Init.ContinuousConvMode = ENABLE;
//  hadc1.Init.DiscontinuousConvMode = DISABLE;
//  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
//  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
//  hadc1.Init.NbrOfConversion = 1;
//  HAL_ADC_Init(&hadc1);

//    /**Configure Regular Channel
//    */
//  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
//  sConfig.Rank = 1;
//  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
//  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

//}
/* ADC1 init function */
void MX_ADC1_Init(void) {
    ADC_ChannelConfTypeDef  sConfig;
 
    /**Common config
        */
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    HAL_ADC_Init(&hadc1);
 
    /**Configure Regular Channel
        */
    sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}
/* ADC2 init function */
void MX_ADC2_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Common config 
    */
  hadc2.Instance = ADC2;
  hadc2.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 5;
  HAL_ADC_Init(&hadc2);

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = 2;
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = 3;
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 4;
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);

    /**Configure Regular Channel 
    */
  sConfig.Rank = 5;
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);

}
/*
 //ADC2 init function 
void MX_ADC2_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

  hadc2.Instance = ADC2;
  hadc2.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 4;
  HAL_ADC_Init(&hadc2);

  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);


  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = 2;
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);


  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = 3;
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);

  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 0;//4
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);

//  sConfig.Rank = 5;
//  HAL_ADC_ConfigChannel(&hadc2, &sConfig);
}*/

/* IWDG init function */
void MX_IWDG_Init(void)
{

  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
  hiwdg.Init.Reload = 3195;
  HAL_IWDG_Init(&hiwdg);

}

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLED;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
  hspi1.Init.CRCPolynomial = 10;
  HAL_SPI_Init(&hspi1);

}

/* SPI2 init function */
void MX_SPI2_Init(void)
{

  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_1LINE;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLED;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
  hspi2.Init.CRCPolynomial = 10;
  HAL_SPI_Init(&hspi2);

}

/* USART1 init function */
void MX_USART1_UART_Init(U16 baud)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = baud;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart1);

}

/* USART2 init function */
void MX_USART2_UART_Init(U16 baud)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = baud;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_EVEN;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart2);

}

/* USART3 init function */
void MX_USART3_UART_Init(U16 baud)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = baud;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_EVEN;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart3);

}

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOC_CLK_ENABLE();
  __GPIOD_CLK_ENABLE();
  __GPIOA_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : OPTO_RL1_STOP_Pin OPTO_RL1_RUN_Pin buzzer_Pin */
  GPIO_InitStruct.Pin = OPTO_RL1_STOP_Pin|OPTO_RL1_RUN_Pin|buzzer_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : RADIO_M1_Pin RADIO_M2_Pin EN_485_2_Pin LED_CPU_Pin
                           OPTO_RL3_RUN_Pin OPTO_RL3_STOP_Pin OPTO_RL4_RUN_Pin OPTO_RL4_STOP_Pin
                           CS_SPI1_Pin */
  GPIO_InitStruct.Pin =GPIO_PIN_15| RADIO_M1_Pin|RADIO_M2_Pin|EN_485_2_Pin|LED_CPU_Pin
                          |OPTO_RL3_RUN_Pin|OPTO_RL3_STOP_Pin|OPTO_RL4_RUN_Pin|OPTO_RL4_STOP_Pin
                          |CS_SPI1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : KEY_Pin */
  GPIO_InitStruct.Pin = KEY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : EN_485_1_Pin EN_485_3_Pin en_595_Pin OPTO_RL2_STOP_Pin
                           OPTO_RL2_RUN_Pin */
  GPIO_InitStruct.Pin = EN_485_1_Pin|EN_485_3_Pin|en_595_Pin|OPTO_RL2_STOP_Pin
                          |OPTO_RL2_RUN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */

/**
  * @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
