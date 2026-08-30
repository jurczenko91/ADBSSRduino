# ADBSSRduino
 **Poly 800 style envelope generator on Arduino**

<img src="https://github.com/jurczenko91/ADBSSRduino/blob/main/ADBSSRduino_module.jpg" alt="DIY module" width="500"/>
 
 I made an extended version of the ADSRduino envelope generator by [m0xpd](https://github.com/m0xpd). You can find original version here: https://github.com/m0xpd/ADSRduino.

 I added Break point and Slope phases to make the envelope a bit more complex. The same envelope style were used in Korg Poly 800 synth. Also I changed the DAC for MCP4725, as I've got plenty of them :) 

 ## What is the difference?

<img src="https://github.com/jurczenko91/ADBSSRduino/blob/main/adsr.jpg" alt="Standard ADSR" width="500"/>

 **ADSR** stands for *Attack*, *Decay*, *Sustain*, *Release*. *Attack*, *Decay* and *Release* are time (duration) parameters, while *Sustain* represents a voltage level. When a key is pressed, the envelope generator starts to raise the voltage from 0 volts to its maximum (5 volts in this case) with the duration defined by *Attack* parameter. Then the voltage follows to the *Sustain* voltage level over the duration defined by *Decay* parameter. The voltage then holds its *Sustain* value for as long as the key is pressed. The moment the key is released, the voltage starts to drop to 0 volts with the duration defined by *Release* parameter.<br><br><br><br>

<img src="https://github.com/jurczenko91/ADBSSRduino/blob/main/adbssr.jpg" alt="ADBSSR" width="500"/>

 **ADBSSR**, on the other hand, stands for *Attack*, *Decay*, *Break point*, *Slope*, *Sustain*, *Release*. There are two additional parameters: *Break point*, which defines a voltage level, and *Slope*, which is another duration parameter. The operating principle is the same as in **ADSR**, except after reaching the maximum, the voltage decays to the *Break point* level (instead of *Sustain*) over the duration defined by the *Decay* parameter. After that it continues to the *Sustain* level over the duration defined by the *Slope* parameter. The rest of the process is identical to **ADSR**.
 
## Schematics

 ![Schematics](https://github.com/jurczenko91/ADBSSRduino/blob/main/ADBSSR_generator_schematic.png)

There's also an ability to switch on the loop mode connecting digital pin 3 to ground. Also, there is the recommendation to add a zener diode from the input to ground to protect the Arduino. I omitted those.<br>
I highly recommend to read **m0xpd**'s article on the Blogspot: https://m0xpd.blogspot.com/2017/02/signal-processing-on-arduino.html. 
 
