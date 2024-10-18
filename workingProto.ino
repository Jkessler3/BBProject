#include <SevSeg.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//PRESSURE SENSOR VARAIBLES
const int pressureSensorPin = A0; //Connect the pressure sensor to the analog pin A0
const int pressureThreshold = 450; //Cu Gap Threshold.
int pressureValue = 0;

//loop counters
int countLLoop = 0;
int countHLoop = 0;
int count = 1;

//BUTON VARAIBLES
const int buttonPin = 10; // Connect the push button to digital pin 10
static unsigned long last_interrupt_time = 0;
unsigned long interrupt_time = 0;
bool isInterrupted = false;

//LED VARAIBLES
byte ccDPins[] = {};
byte segDPins[] = {3, 2, 8, 7, 6, 4, 5, 9}; //Pins that corrispond to the LED segments
SevSeg sevseg; // Declares seven seg object

void countIncrease();

void setup(){

  sevseg.begin(COMMON_CATHODE, 1, ccDPins, segDPins, true); //Initialize the seven segment display 
  sevseg.setBrightness(75); //Set the brightness of the display

  //PCIFR |= (1 << PCIF0);    // Sets flag for Interrupt
  PCICR |= (1 << PCIE0);     // set PCIE0 to enable PCMSK0 scan
  PCMSK0 |= (1 << PCINT2);   // set PCINT2 to trigger an interrupt on state change 
  sei();                    // Enable global interrupts
}

void loop() {
  sei();
  isInterrupted = false;
  //Sets the display to show count
  sevseg.setNumber(count);
  sevseg.refreshDisplay();

  pressureValue = analogRead(pressureSensorPin); //Takes first reading of pressureValue to start loops.

  //sets loop counters to zero
  countLLoop = 0;
  countHLoop = 0;

  while((pressureValue > pressureThreshold) && (countHLoop < 6) && !isInterrupted)
  { 
    //sevseg.setChars("P");
    //sevseg.refreshDisplay();
    pressureValue = analogRead(pressureSensorPin); //polls the presure sensor
    delay(250);
    sevseg.setNumber(count);
    sevseg.refreshDisplay();
    delay(250);
    countHLoop++;
    }
      
//LOOP FOR RELEASED SENSOR
  while((countHLoop >= 6) && (countLLoop < 6) && !isInterrupted)
  {
    pressureValue = analogRead(pressureSensorPin);//check pressurValue
    if((pressureValue > pressureThreshold) && !isInterrupted){
      //sevseg.setChars("P"); //Sets the display to U for unoad. Indicates that pressure has been removed.
      //sevseg.refreshDisplay();
      countLLoop = 0;
    }
    else
    {
      //sevseg.setChars("U");
      //sevseg.refreshDisplay();
    }
  
    delay(250); 
    sevseg.setNumber(count); //Sets the display back to count to indicate the current prayer step.
    sevseg.refreshDisplay();
    delay(250); // Delay to keep track of time. Set to 0.5 seconds. If 6 sucessful polls of non-pressure then 3 seconds of non-pressure pressure is detected.
    countLLoop++; //Increments the Low count loop. This indicates the length of time the pressure has been LOW. 
  }

//SETS THE COUNT VALUE IF BOTH HIGH AND LOW CONDITIONS HAVE BEEN MET
  if(countHLoop >= 6 && countLLoop >= 6 && !isInterrupted)
  {
    countIncrease();
  }
}

ISR(PCINT0_vect)
{
  interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) 
  {
    countIncrease();
    isInterrupted = true;
  }
  last_interrupt_time = interrupt_time;
}

void countIncrease(){
  if(count < 4)
    {
      count++;
    }
    else{
      count = 1;
      }
      countHLoop = 0;
      countLLoop = 0; 
}