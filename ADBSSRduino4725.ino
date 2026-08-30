// ADSRduino
//
// a simple ADSR for the Arduino
// m0xpd
// Feb 2017
//
// see http://m0xpd.blogspot.co.uk/2017/02/signal-processing-on-arduino.html
//
// 4725 mod and BS mod by Anton Yurchenko aka Ogonblik http://ogonblik.tilda.ws
//
// uses a 4725 DAC on digita pins A4 and A5 (SDA and SCL)
// receives gate inputs on digital pin 2 (remember to protect e.g. with a 200R resistor and a 5V1 Zener diode)
// and a loop mode input on digital pin 3 (pulling to 0V selects loop mode)
//
// Voltages between 0 and 5V (e.g. from potentiometers) on analog pins A0:A3 A6:A7 control Attack, Decay, Sustain, Release, Breakpoint and Slope 

#include <Adafruit_MCP4725.h>
Adafruit_MCP4725 dac;

#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }

// Pin definitions...
const int gatePin = 2;
const int modePin = 3;

float alpha=0.7;   // this is the pole location ('time constant') used for the first-order difference equation
double alpha1=0.9;  // initial value for attack
double alpha2=0.9;  // initial value for decay
double alpha7=0.9;   // initial value for slope
double alpha3=0.95; // initial value for release

float envelope = 0.0;  // initialise the envelope
                       // result of reads from potentiometers (yes - it will only be an int, but helps with the casting!)
float CV0 = 0.0;       // attack     
float CV1 = 0.0;       // decay
int CV6 = 0;           // breakpoint
float CV7 = 0.0;       // slope
int CV2 = 0;           // sustain
float CV3 = 0.0;       // release

int drive = 0;
int sustain_Level = 0;
int breakpoint_Level = 0;
int scan = 0;
boolean note_active = false;
boolean loop_mode=false;
boolean trigger = false;
boolean decay = false;
boolean slope = false;
boolean release_done = true;



void setup() {
  
  dac.begin(0x60);
  pinMode(gatePin, INPUT);  
  pinMode(modePin, INPUT);     
  digitalWrite(gatePin,HIGH);   
  digitalWrite(modePin,HIGH);  
  
}

void loop() {
    boolean gate=digitalRead(gatePin);        // read the gate input every time through the loop
    update_params(scan);                      // scan only one of the other inputs each pass 
    
    boolean trigger=gate||(loop_mode&&release_done);  // trigger an ADSR even if there's a gate OR if we're in loop mode
    while(trigger){  
    if(note_active==false){                   // if a note isn't active and we're triggered, then start one!
    decay = false;
    drive=4096;                               // drive toward full value
    alpha=alpha1;                             // set 'time constant' alpha1 for attack phase 
    note_active=true;                         // set the note_active flag
  } 
   if((decay==false)&&(envelope>4000)&&(drive==4096)){    // if we've reached envelope >4000 with drive= 4096, we must be at the end of attack phase
                                                          // so switch to decay...
    decay = true;                                         // set decay flag
    drive=breakpoint_Level;                                  // drive toward breakpoint level
    alpha=alpha2;                                         // and set 'time constant' alpha2 for decay phase
  } 
   if((decay==true)&&(envelope<(breakpoint_Level+96))&&(drive==breakpoint_Level)){

    slope = true;
    drive=sustain_Level;                                    // drive toward sustain level
    alpha=alpha7;                                         
    decay = false; 
  }
  
    envelope=((1.0-alpha)*drive+alpha*envelope);     // implement difference equation: y(k) = (1 - alpha) * x(k) + alpha * y(k-1)
    dac.setVoltage(round(envelope),false);                   // and output the envelope to the DAC

     if((loop_mode==true)&&(slope==true) && (envelope<(float)(sustain_Level+1.0))){ // in loop mode, break out at the end of the slope
      slope = false;
      break;
    }
    gate=digitalRead(gatePin);                      // read the gate pin (remember we're in the while loop)
    trigger=gate||(loop_mode&&release_done);        // and re-evaluate the trigger function
    }
    
    if(note_active==true){                // this is the start of the release phase
    drive=0;                              // drive towards zero
    alpha=alpha3;                         // set 'time comnstant' alpha3 for release phase
    note_active=false;                    // turn off note_active flag
    release_done=false;                   // and set release_flag done false
  }    
  
    envelope=((1.0-alpha3)*drive+alpha3*envelope);   // implement the difference equation again (outside the while loop)
    dac.setVoltage(round(envelope),false);                   // and output envelope
    gate=digitalRead(gatePin);                       // watch out for a new note
    scan+=1;                                         // prepare to look at a new parameter input
    if(envelope<4){                                  // is the release phase ended?
      release_done=true;                             // yes - so flag it
    }
    if(scan==7){                                     // increment the scan pointer
      scan=0;
    }
}

void update_params(int scan){             // read the input parameters
  switch (scan){
  case 0:
  CV0=analogRead(0);                      // get the attack pole location
  alpha1=0.999*cos((1023-CV0)/795);
  alpha1=sqrt(alpha1);  
  break;
  case 1:
  CV1=analogRead(1);                      // get the decay pole location
  alpha2=0.999*cos((1023-CV1)/795);
  alpha2=sqrt(alpha2);   
  break; 
  case 2:                                // get the integer breakpoint level
  CV6=analogRead(6);
  breakpoint_Level=CV6<<2;
  break;
  case 3:                                // get the slope pole location
  CV7=analogRead(7);
  alpha7=0.999*cos((1023-CV7)/795);
  alpha7=sqrt(alpha7);   
  break;
  case 4:
  CV2=analogRead(2);                     // get the (integer) sustain level
  sustain_Level=CV2<<2;
  break;
  case 5:
  CV3=analogRead(3);                     // get the release pole location (potentially closer to 1.0)
  alpha3=0.99999*cos((1023-CV3)/795);
  alpha3=sqrt(alpha3);
  break;  
  case 6:                                // read the loop mode input
  loop_mode=!digitalRead(modePin);
  break;  
  
  }
  
}
