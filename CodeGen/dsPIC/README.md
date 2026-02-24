# Files for dsPIC33AK512MPS512

The files for the projects have been mostly generated with MCC.

The Clock is configured in this way:

```
   /*  
        System Clock Source                             :  PLL1 Out output
        System/Generator 1 frequency (Fosc)             :  100 MHz
        
        Clock Generator 2 frequency                     : 8 MHz
        Clock Generator 3 frequency                     : 8 MHz
        Clock Generator 6 frequency                     : 80 MHz
        
        PLL 1 frequency                                 : 350 MHz
        PLL 1 VCO Out frequency                         : 87.5 MHz
        PLL 2 frequency                                 : 200 MHz
        PLL 2 VCO Out frequency                         : 166.666667 MHz

    */
```

to get a frequency used to calculate the sampling time at **50MHz**.

The python script *mcc2pysim.py* can be used to get, modify and transfer the genrated MCC files into the CodeGen/dsPIC folder.

## Important files

- **extDevice.mc3**: contains the MCC configuration used to generate the devices adc5, qei1, qei2 qei3, sccp1, sccp2, sccp3
- **include/dspicConf.h**: contains the configuration used in pysimCoder to generalize the drivers.

In particular, the **dspicConf**.h file contains the structures with the registers used by the devices, like *ADC*, *PWM* and *ENC* in this form (example for encoders):

```
typedef struct {
  volatile uint32_t *datareg;
  volatile uint32_t *enable;
  volatile gpio_port_t portA;
  volatile uint8_t pinA;
  volatile gpio_port_t portB;
  volatile uint8_t pinB;
} qei_hw_t;


static const qei_hw_t qei[] = {
  [QEI_1] =   {&POS1CNT, &QEI1CON, GPIO_PORT_A, 0, GPIO_PORT_A, 2},
  [QEI_2] =   {&POS2CNT, &QEI2CON, GPIO_PORT_B, 0, GPIO_PORT_B, 1},
  [QEI_3] =   {&POS3CNT, &QEI3CON, GPIO_PORT_B, 2, GPIO_PORT_B, 3}
};
```

Please, modify the first lines of the *mcc2pysim.py* script

```
source = '/home/bucher/sviluppo/MPLABXProjects/pysimC.X/mcc_generated_files'
```

with the right path which contains the mcc_generated_files.



