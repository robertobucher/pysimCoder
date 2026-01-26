#ifndef DSPICFUN_H
#define DSPICFUN_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum{
  ADC_1 =1,
  ADC_2,
  ADC_3,
  ADC_4,
  ADC_5
}adc_n_t;

typedef enum{
  PWM_1 =1,
  PWM_2,
  PWM_3
}pwm_n_t;

typedef enum{
  QEI_1 =1,
  QEI_2,
  QEI_3
}quei_n_t;

typedef enum {
    GPIO_PORT_A = 0,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
    GPIO_PORT_E,
    GPIO_PORT_F,
    GPIO_PORT_G,
    GPIO_PORT_H
} gpio_port_t;

typedef enum {
    GPIO_INPUT,
    GPIO_OUTPUT
} gpio_dir_t;

typedef enum {
    GPIO_DIGITAL,
    GPIO_ANALOG
} gpio_mode_t;

void gpio_config(gpio_port_t port,
                 uint8_t pin,
                 gpio_dir_t dir,
                 gpio_mode_t mode);

void gpio_write(gpio_port_t port, uint8_t pin, uint8_t value);

uint8_t gpio_read(gpio_port_t port, uint8_t pin);

#endif
 
