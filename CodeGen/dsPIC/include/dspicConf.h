#ifndef DSPICCONF_H
#define DSPICCONF_H

#include <xc.h>
#include <dspicFun.h>

#define ADCMAXVAL 330
#define PWMFACT 20000

typedef struct {
    volatile uint32_t *lat;
    volatile uint32_t *tris;
    volatile uint32_t *port;
    volatile uint32_t *ansel; 
} gpio_port_hw_t;

typedef struct {
    volatile uint32_t *stat;
    volatile uint32_t *trig;
    volatile uint32_t *buf[4];
} adc_hw_t;

typedef struct {
    volatile uint32_t *dutyReg;
} pwm_hw_t;

typedef struct {
  volatile uint32_t *datareg;
  volatile uint32_t *enable;
  volatile gpio_port_t portA;
  volatile uint8_t pinA;
  volatile gpio_port_t portB;
  volatile uint8_t pinB;
} qei_hw_t;

static const gpio_port_hw_t gpio_ports[] = {
  [GPIO_PORT_A] = { &LATA, &TRISA, &PORTA, &ANSELA },
  [GPIO_PORT_B] = { &LATB, &TRISB, &PORTB, &ANSELB },
  [GPIO_PORT_C] = { &LATC, &TRISC, &PORTC, &ANSELC },
  [GPIO_PORT_D] = { &LATD, &TRISD, &PORTD, &ANSELD },
  [GPIO_PORT_E] = { &LATE, &TRISE, &PORTE, &ANSELE },
  [GPIO_PORT_F] = { &LATF, &TRISF, &PORTF, &ANSELF },
  [GPIO_PORT_G] = { &LATG, &TRISG, &PORTG, NULL     },
  [GPIO_PORT_H] = { &LATH, &TRISH, &PORTH, NULL     }
};

static const adc_hw_t adc[] = {
  [ADC_1] = {&AD1STAT,
	   &AD1SWTRG,
	   {&AD1CH0DATA, &AD1CH1DATA, &AD1CH2DATA,
	    &AD1CH3DATA, &AD1CH4DATA}},
  [ADC_2] = {&AD2STAT,
	   &AD2SWTRG,
	   {&AD2CH0DATA, &AD2CH1DATA, &AD2CH2DATA,
	    &AD2CH3DATA}},
  [ADC_5] = {&AD5STAT,
	   &AD5SWTRG,
	   {&AD5CH0DATA, &AD5CH1DATA, &AD5CH2DATA,
	    &AD5CH3DATA, &AD5CH4DATA}}
};

static const pwm_hw_t pwm[] = {
  [PWM_1] =   {&PG1DC},
  [PWM_2] =   {&PG2DC},
  [PWM_3] =   {&PG3DC}
};

static const qei_hw_t qei[] = {
  [QEI_1] =   {&POS1CNT, &QEI1CON, GPIO_PORT_D, 5, GPIO_PORT_D, 6},
  [QEI_2] =   {&POS2CNT, &QEI2CON, GPIO_PORT_D, 7, GPIO_PORT_D, 8},
  [QEI_3] =   {&POS3CNT, &QEI3CON, GPIO_PORT_D, 11, GPIO_PORT_D, 13}
};

#endif
 
