#ifndef _RPI_GPCLK_H
#define _RPI_GPCLK_H

#define RPI_GPCLK_PLLD_500_MHZ  0  /*500 MHz*/
#define RPI_GPCLK_OSC_19_MHZ_2  1  /*19.2 MHz*/
#define RPI_GPCLK_HDMI_216_MHZ  2  /*216 MHz*/
#define RPI_GPCLK_PLLC_1000_MHZ 3  /*1000 MHz, changes with overclock*/

int rpi_gpclk_setup(int chan, int source, int div_int, int div_frac);

#endif /*_RPI_GPCLK_H*/