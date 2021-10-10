//www.elegoo.com
//2016.12.8

// Define Pins
#define RED 6
#define YELLOW 8
#define GREEN 10
#define BUZZER 11
#define BUTTON 12
#define ONBOARDLED 13

#define data 2   //74HC595  pin 8 DS
#define latch 3  //74HC595  pin 9 STCP
#define clock 4  //74HC595  pin 10 SHCP


// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int count = 20;
int state = 0;
int tens = 2;
int ones = 0;

//storage variables
boolean toggle1 = 0;

//this code will enable one arduino timer interrupt
//timer1 will interrupt at 1Hz


unsigned char table[]=
{0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c
,0x39,0x5e,0x79,0x71,0x00};


void setup()
{
  pinMode(BUTTON, INPUT);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(BUZZER,OUTPUT);//initialize the buzzer pin as an output  
  pinMode(7, OUTPUT); //digit 3 enable
  pinMode(9, OUTPUT); //digit 4 enable
  pinMode(ONBOARDLED, OUTPUT);
  
  pinMode(latch,OUTPUT);
  pinMode(clock,OUTPUT);
  pinMode(data,OUTPUT);
 
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, LOW);



  cli();//stop interrupts

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  sei();//allow interrupts
  
}//end setup

// Timer1's interrupt service routing (ISR)
// The code in ISR will be executed every time timer1 interrupt occurs
// That is, the code will be called once every second
// TODO
//   you need to set a flag to trigger an evaluation of the conditions
//   in your state machine, then potentially transit to next state
//

ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  if (toggle1){
    digitalWrite(13,HIGH);
    toggle1 = 0;
    if (state >= 1 )
      {
        count = count - 1; 
      }
  }
  else{
    digitalWrite(13,LOW);
    toggle1 = 1;
    if (state >= 1 )
      {
        count = count - 1; 
      }
  }
}


void Display(unsigned char num)
{

  digitalWrite(latch,LOW);
  shiftOut(data,clock,MSBFIRST,table[num]);
  digitalWrite(latch,HIGH);
  
}

// main loop
void loop()
{
  //Buzzer control
  buttonState = digitalRead(BUTTON);
  if (count <= 3) {
    digitalWrite(BUZZER,HIGH);
  }
  else {
    digitalWrite(BUZZER, LOW);
  }  


        //seven segment control
  if (state >= 1) {
      tens = count/10;
      ones = count%10;
      digitalWrite(7, HIGH);
      digitalWrite(9, LOW);
      Display(tens);
      delay(5);
      
      digitalWrite(9, HIGH);
      digitalWrite(7, LOW);
      Display(ones);
      delay(5);
  }

  
  //finite state machine
  switch (state)
  {
     case 0: //initial state, red flashes until button is pressed
            if (buttonState == HIGH) {
               state = 1;
               count = 20;
               digitalWrite(RED,HIGH);
               break;
            }

            else {
                Display(16);
                if (toggle1 == 1) {
                  digitalWrite(RED, LOW);
                }
                else {
                  digitalWrite(RED, HIGH);
                } 
                break;
            }
     case 1: //red
            if (count == 0) {
               state = 2;
               count = 20;
               digitalWrite(RED, LOW);
               digitalWrite(GREEN, HIGH);
               break;
            }
     case 2: //green
            if (count == 0) {
               state = 3;
               count = 3;
               digitalWrite(GREEN, LOW);
               digitalWrite(YELLOW, HIGH);
               
               break;
            }    
     case 3://yellow
            if (count == 0) {
               state = 1;
               count = 20;
               digitalWrite(YELLOW, LOW);
               digitalWrite(RED, HIGH);
               break;
            }
  }


}
