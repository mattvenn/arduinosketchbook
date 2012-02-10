int ledPinR = 3;    // LED connected to digital pin 3
int ledPinG = 5;    // LED connected to digital pin 5
int ledPinB = 9;    // LED connected to digital pin 9
int statusLED = 7;  // LED for status flashing

int defaultThresh = 15;
int minThresh = 5;
int thresh = defaultThresh; //when we assume the knob has been moved (rather than noise)
int knobTimer = 0;

int R,G,B;
int oldR,oldG,oldB;
int knobR, knobG, knobB;

void setup()
{ 
  pinMode( statusLED, OUTPUT );
  pinMode( ledPinG, OUTPUT );
  pinMode( ledPinB, OUTPUT );
  pinMode( ledPinR, OUTPUT );
  pinMode( 0, INPUT );

  flashStatus(); 

  //we're on a 12MHz clock so scale up (should be 115000)
  Serial.begin(153333);

  R = oldR = 255;
  G = oldG = 255;
  B = oldB = 255;

  updateLED();
} 

void loop()
{ 
  //we expect RGB chars plus a new line
  if( Serial.available() > 3 )
  {
    flashStatus();

    R = 255 - Serial.read();
    G = 255 - Serial.read();
    B = 255 - Serial.read();

    //throw away any other chars up to newline
    while( Serial.available() )
    {
      char next = Serial.read();
      if( next == '\n' )
        break;
    } 

    updateLED();
  }

  //or read the analog inputs
  knobR = map( analogRead( 2 ), 0, 1023, 255, 0 );
  knobG = map( analogRead( 0 ), 0, 1023, 255, 0 );
  knobB = map( analogRead( 1 ), 0, 1023, 255, 0 );

  if( knobTimer > 0 )
  {
    knobTimer --;
    thresh = minThresh;
  }
  else
  {  
    thresh = defaultThresh;
  }
  if( abs( knobR - oldR ) > thresh || abs( knobG - oldG ) > thresh || abs( knobB - oldB ) > thresh )
  {
    flashStatus();
    R = knobR;
    G = knobG;
    B = knobB;
    oldR = knobR;
    oldB = knobB;
    oldG = knobG;

    updateLED();
    //reset knob timer
    knobTimer = 1000;
  }

} 

void flashStatus()

{
  digitalWrite( statusLED, HIGH );
  delay( 10 );
  digitalWrite( statusLED, LOW );
  delay( 10 );
}

void updateLED()
{
  // sets the value (range from 0 to 255):
  analogWrite(ledPinR, R);         
  analogWrite(ledPinG, G);         
  analogWrite(ledPinB, B);      
}



