#include <xc.h>
#include <dsPICfun.h>

static const gpio_port_hw_t gpio_ports[] = {
    [GPIO_PORT_A] = { &LATA, &TRISA, &PORTA, &ANSELA },
    [GPIO_PORT_B] = { &LATB, &TRISB, &PORTB, &ANSELB },
    [GPIO_PORT_C] = { &LATC, &TRISC, &PORTC, &ANSELC },
    [GPIO_PORT_D] = { &LATD, &TRISD, &PORTD, &ANSELD },
    [GPIO_PORT_E] = { &LATE, &TRISE, &PORTE, &ANSELE },
    [GPIO_PORT_F] = { &LATF, &TRISF, &PORTF, &ANSELF },
    [GPIO_PORT_G] = { &LATG, &TRISG, &PORTG, NULL     }, // spesso no ANSEL
    [GPIO_PORT_H] = { &LATH, &TRISH, &PORTH, NULL     }
};

void gpio_config(gpio_port_t port,
                 uint8_t pin,
                 gpio_dir_t dir,
                 gpio_mode_t mode)
{
    const gpio_port_hw_t *p = &gpio_ports[port];

    /* Direzione */
    if (dir == GPIO_OUTPUT)
        *(p->tris) &= ~(1U << pin);
    else
        *(p->tris) |=  (1U << pin);

    /* Analogico / Digitale */
    if (p->ansel != NULL) {
        if (mode == GPIO_ANALOG)
            *(p->ansel) |=  (1U << pin);
        else
            *(p->ansel) &= ~(1U << pin);
    }
}

void gpio_write(gpio_port_t port, uint8_t pin, uint8_t value)
{
    if (value)
        *(gpio_ports[port].lat) |=  (1U << pin);
    else
        *(gpio_ports[port].lat) &= ~(1U << pin);
}

uint8_t gpio_read(gpio_port_t port, uint8_t pin)
{
    return ( (*(gpio_ports[port].port) >> pin) & 1U );
}
