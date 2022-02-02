#include "stm32h7xx_hal.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

static unsigned short PA_Pins = 0x0000;
static unsigned short PB_Pins = 0x0000;
static unsigned short PC_Pins = 0x0000;
static unsigned short PD_Pins = 0x0000;
static unsigned short PE_Pins = 0x0000;
static unsigned short PF_Pins = 0x0000;
static unsigned short PG_Pins = 0x0000;

int set_GPIO_outPort(int p, unsigned short ch)
{
  int port; 
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  switch(p){
  case 0:
    __HAL_RCC_GPIOA_CLK_ENABLE();
    port = (int) GPIOA;
    PA_Pins = PA_Pins | ch;
    HAL_GPIO_WritePin(GPIOA, PA_Pins, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = PA_Pins;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);   
    break;
  case 1:
    __HAL_RCC_GPIOB_CLK_ENABLE();
    port = (int) GPIOB;
    PB_Pins = PB_Pins | ch;
    HAL_GPIO_WritePin(GPIOB, PB_Pins, GPIO_PIN_RESET);
     GPIO_InitStruct.Pin = PB_Pins;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);   
    break;
  case 2:
    __HAL_RCC_GPIOC_CLK_ENABLE();
    port = (int) GPIOC;
    PC_Pins = PC_Pins | ch;
    HAL_GPIO_WritePin(GPIOC, PC_Pins, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = PC_Pins;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);   
    break;
  case 3:
    __HAL_RCC_GPIOD_CLK_ENABLE();
    port = (int) GPIOD;
    PD_Pins = PD_Pins | ch;
    HAL_GPIO_WritePin(GPIOD, PD_Pins, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = PD_Pins;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);   
    break;
  case 4:
    __HAL_RCC_GPIOE_CLK_ENABLE();
    port = (int) GPIOE;
    PE_Pins = PE_Pins | ch;
    HAL_GPIO_WritePin(GPIOE, PE_Pins, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = PE_Pins;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);   
    break;
  case 5:
    __HAL_RCC_GPIOF_CLK_ENABLE();
    port = (int) GPIOF;
    PF_Pins = PF_Pins | ch;
    HAL_GPIO_WritePin(GPIOF, PF_Pins, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = PF_Pins;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);   
    break;
  case 6:
    __HAL_RCC_GPIOG_CLK_ENABLE();
    port = (int) GPIOG;
    PG_Pins = PG_Pins | ch;
    HAL_GPIO_WritePin(GPIOG, PG_Pins, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = PG_Pins;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);   
    break;    
  default:
    break;
  }
  return port;
}

int set_GPIO_inPort(int p, unsigned short ch)
{
  int port;
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  switch(p){
  case 0:
    __HAL_RCC_GPIOA_CLK_ENABLE();
    port = (int) GPIOA;
    PA_Pins = PA_Pins | ch;
    GPIO_InitStruct.Pin = PA_Pins;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);   
    break;
  case 1:
    __HAL_RCC_GPIOB_CLK_ENABLE();
    port = (int) GPIOB;
    PB_Pins = PB_Pins | ch;
    GPIO_InitStruct.Pin = PB_Pins;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);   
    break;
  case 2:
    __HAL_RCC_GPIOC_CLK_ENABLE();
    port = (int) GPIOC;
    PC_Pins = PC_Pins | ch;
    GPIO_InitStruct.Pin = PC_Pins;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);   
    break;
  case 3:
    __HAL_RCC_GPIOD_CLK_ENABLE();
    port = (int) GPIOD;
    PD_Pins = PD_Pins | ch;
    GPIO_InitStruct.Pin = PD_Pins;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);   
    break;
  case 4:
    __HAL_RCC_GPIOE_CLK_ENABLE();
    port = (int) GPIOE;
    PE_Pins = PE_Pins | ch;
    GPIO_InitStruct.Pin = PE_Pins;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);   
    break;
  case 5:
    __HAL_RCC_GPIOF_CLK_ENABLE();
    port = (int) GPIOF;
    PF_Pins = PF_Pins | ch;
    GPIO_InitStruct.Pin = PF_Pins;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);   
    break;
  case 6:
    __HAL_RCC_GPIOG_CLK_ENABLE();
    port = (int) GPIOG;
    PG_Pins = PG_Pins | ch;
    GPIO_InitStruct.Pin = PG_Pins;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);   
    break;
  default:
    break;
  }
  return port;
}

void start_OTG()
{
  static int counter = 0;
  if (counter == 0){
    MX_USB_DEVICE_Init();
    HAL_PWREx_EnableUSBVoltageDetector();
    counter += 1;
  }
}

