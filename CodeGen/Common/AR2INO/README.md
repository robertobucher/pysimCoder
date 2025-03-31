This is a new protocllo to use Arduino I/Os from an application generated in RT by pysimCoder.

I my opinion, the Firmata protocol is to heavy to be used to implement controllers with a usable sampling rate,
This new protocol is lighter, simpler and more usable.

It implements 5 I/O system:

  * Analog input
  * PWM output
  * Digital input
  * Digital output
  * 1 encoders
Encoders are associated to fixed pins:

Arduino Leonardo:  pin 2 and 3
Arduino UNO R4 MINIMA: pin 4 and 5

These pins can't be used for other I/O's!

The "Setup" (INIT) block must be inserted into the pysimCoder Block Diagram in order to have the Arduino working!

The program for arduino is available <a href="https://github.com/robertobucher/ar2ino" title="Title Text">here</a>.

