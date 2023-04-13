# Smart-Plant-Monitor
USC ECE Undergraduate capstone Project

Hardware Component:

ATmega328p Microcontroller

20x4 characters, parallel, RGB backlight      			 Adafruit 499—fully controlled with 6 digital lines 

Light sensor, ALS-PT19, analog output        			 Adafruit 2748—analog voltage on the OUTPUT pin

Temperature sensor, TMP36, analog, TO-92     		 Adafruit 165—read voltage on the Vout pin and convert it to temperature 

Soil moisture sensor, resistive, analog             		 Sparkfun SEN-13322—obtain a output from SIG pin which depends on the amount of water in the soil 

2 * Pump, submersible, horizontal mounting          		 Adafruit 4546—powered with 3V and draws 100mA

2 * Hose, clear plastic, 6mm ID, for Adafruit 4546 pumps         Adafruit 454 

Heating pad, 5x10 cm                              			 Sparkfun COM-11288—5V DC powered 

3 * Pushbutton, SPST, PC-board mount, white,  red, black   Digikey 401-1963

White LED, SMD, 3W                  					 Amazon B01DBZICDC—3-3.4V DC powered input 7

2 * 3.3v Relay module

5v Relay module

Which Pins to Use:

PC0 for the temperature sensor

PC1 for the moisture sensor

PC2 for the light sensor

PC3 - PC5 for the buttons

PD0 - PD5 for the LCD

PD6 - PD7 for pumps

PB1 for the LED

PB2 for the Heater
