//BootSetting.h
#ifndef __BootSetting_h
#define __BootSetting_h
/*
when below flag was 0 it means bootloader doesnt work and opposite.

As you can see in below , BOOT_FLASH causes to change this parameter.

in stm32f103xb.h
#define FLASH_BASE            ((uint32_t)0x08000000) to ((uint32_t)0x08002800)!< FLASH base address in the alias region 

system_stm32f1xx.c
#define VECT_TAB_OFFSET  0x2000 to 0x2800 !< Vector Table base offset field. 
                                  This value must be a multiple of 0x200. 
*/

#define BOOT_FLAG 0


/*
moreover for active bootloader app you have to change IROM1 parameter to 0x8002800   0xD800
when you dont wanna use bootloader app you have to change those 				to 0x8000000   0x10000
*/
#endif