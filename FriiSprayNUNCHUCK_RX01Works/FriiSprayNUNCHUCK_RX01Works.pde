  /*
 * This file is an adaptation of the code by these authors:
 * Tod E. Kurt, http://todbot.com/blog/
 *
 * The Wii Nunchuck reading code is taken from Windmeadow Labs
 * http://www.windmeadow.com/node/42
 * modified to include wireless nunchuck by Homer Manalo, October 2010
 */
#include <Wire.h>
#include <math.h>
#include <utility/twi.h>

#define RADIUS 210  // probably pretty universal

#define numReadings 7

        byte cnt;
        uint8_t status[6];              // array to store wiichuck output
        byte averageCounter;
        //int accelArray[3][AVERAGE_N];  // X,Y,Z
        int i;
        int total;
        uint8_t zeroJoyX = 127;   // these are about where mine are
        uint8_t zeroJoyY = 134; // use calibrateJoy when the stick is at zero to correct
        int ZEROX = 510;
        int ZEROY = 460;
        int ZEROZ = 0;
        int lastJoyX;
        int lastJoyY;
        int angles[3];
#define LED 8
        boolean lastZ, lastC;

        byte joyX;
        byte joyY;
        boolean buttonZ;
        boolean buttonC;
        const byte NunchuckID[6] = {0x00, 0x00, 0xA4, 0x20, 0x00, 0x00};
        byte getID[6] = {0};
        byte connection = 0;

byte C; //!< Stores press state of the C button
byte Z; //!< Stores press State of the Z button
int aveJX; //!< Stores the filtered joystick x-axis reading
int aveJY; //!< Stores the filtered joystick y-axis reading
int aveAX; //!< Stores the filtered accelerometer x-axis reading
int aveAY; //!< Stores the filtered accelerometer y-axis reading
//int last_reading[6]; //!< Stores last filtered reading from the joystick, accelerometer and buttons  
int readJX[numReadings]; //!< Stores readings from joystick x-axis in multiple successions
int readJY[numReadings]; //!< Stores readings from joystick y-axis in multiple successions
int readAX[numReadings]; //!< Stores readings from accelerometer x-axis in multiple successions
int readAY[numReadings]; //!< Stores readings from accelerometer y-axis in multiple successions

void setup()
{
  Serial.begin(115200);
  Serial.println("Start");
  pinMode( LED, OUTPUT );  
  Wire.begin();
  #define TWI_FREQ_NUNCHUCK 400000L
  TWBR = ((CPU_FREQ / TWI_FREQ_NUNCHUCK) - 16) / 2;
  cnt = 0;
  averageCounter = 0;
  
  while(connection == 0)
  {
  Wire.beginTransmission (0x52);      // transmit to device 0x52
  Wire.send (0xF0);           // sends memory address
  Wire.send (0x55);           // sends data
  if(Wire.endTransmission () == 0)
  {
    Serial.println("0x55 data send success");
              
    Wire.beginTransmission (0x52);      // transmit to device 0x52
    Wire.send (0xFB);           // sends memory address
    Wire.send (0x00);           // sends data
    if(Wire.endTransmission () == 0) Serial.println("0x00 data send success");
    else Serial.println("0x00 data send failed");
    
      do
      {
        Wire.beginTransmission (0x52);
        Wire.send (0xFA);
        delay(1);
        Serial.println(Wire.endTransmission(),DEC);
      }
      while(Wire.endTransmission() != 0);
  
      Wire.requestFrom(0x52, 6);
      if(Wire.available() == 0) connection = 2;
      else
      {
        byte i=0;
        while(Wire.available())
        {
          getID[i] = Wire.receive();
          i++;
        }
    
        for(byte j=0; j<6; j++)
        {
          if(getID[j] == NunchuckID[j])
          {
            if(j==0) connection = 1;
            else if(j>0 && connection == 1) connection = 1;
            else connection = 0;
          }
          else connection = 0;
        
          Serial.println(connection,DEC);
        }
      }
  }
  else Serial.println("0x55 data send failed");
  }
  
  if(connection == 1)
  {
    ZEROX = 610;
    ZEROY = 290;
    ZEROZ = 0;
    zeroJoyX =  128;
    zeroJoyY =  124;
    for(byte j=0;j<6;j++)
    {
      Serial.print(getID[j], HEX);
      Serial.print(" ");
    }
    delay(3000);
  }
            
  update();
            
  for (i = 0; i<3;i++)
    angles[i] = 0;            
            
  for(int j=0;j<=6;j++)
    readJX[j] = readJY[j] = readAX[j] = readAY[j] = 0;
}

        void calibrateJoy() {
            zeroJoyX = joyX;
            zeroJoyY = joyY;
        }

        void update() {

            Wire.requestFrom (0x52, 6); // request data from nunchuck
            while (Wire.available ()) {
                // receive byte as an integer
                status[cnt] = _nunchuk_decode_byte (Wire.receive()); //
                cnt++;
            }
            if (cnt > 5) {
                lastZ = buttonZ;
                lastC = buttonC;
                lastJoyX = readJoyX();
                lastJoyY = readJoyY();
                //averageCounter ++;
                //if (averageCounter >= AVERAGE_N)
                //    averageCounter = 0;

                cnt = 0;
                joyX = (status[0]);
                joyY = (status[1]);
                for (i = 0; i < 3; i++)
                    //accelArray[i][averageCounter] = ((int)status[i+2] << 2) + ((status[5] & (B00000011 << ((i+1)*2) ) >> ((i+1)*2)));
                    angles[i] = (status[i+2] << 2) + ((status[5] & (B00000011 << ((i+1)*2) ) >> ((i+1)*2)));

                //accelYArray[averageCounter] = ((int)status[3] << 2) + ((status[5] & B00110000) >> 4);
                //accelZArray[averageCounter] = ((int)status[4] << 2) + ((status[5] & B11000000) >> 6);

                buttonZ = !( status[5] & B00000001);
                buttonC = !((status[5] & B00000010) >> 1);
                _send_zero(); // send the request for next bytes
            }
        }


    // UNCOMMENT FOR DEBUGGING
    //byte * getStatus() {
    //    return status;
    //}

    float readAccelX() {
       // total = 0; // accelArray[xyz][averageCounter] * FAST_WEIGHT;
        return (float)angles[0] - ZEROX;
    }
    float readAccelY() {
        // total = 0; // accelArray[xyz][averageCounter] * FAST_WEIGHT;
        return (float)angles[1] - ZEROY;
    }
    float readAccelZ() {
        // total = 0; // accelArray[xyz][averageCounter] * FAST_WEIGHT;
        return (float)angles[2] - ZEROZ;
    }

    boolean zPressed() {
        return (buttonZ && ! lastZ);
    }
    boolean cPressed() {
        return (buttonC && ! lastC);
    }
    
    boolean buttC() { //homer edit
        return buttonC;
    }

    boolean buttZ() { //homer edit
        return buttonZ;
    }

    // for using the joystick like a directional button
    boolean rightJoy(int thresh=60) {
        return (readJoyX() > thresh and lastJoyX <= thresh);
    }

    // for using the joystick like a directional button
    boolean leftJoy(int thresh=60) {
        return (readJoyX() < -thresh and lastJoyX >= -thresh);
    }


    int readJoyX() {
        return (int) joyX - zeroJoyX;
    }

    int readJoyY() {
        return (int)joyY - zeroJoyY;
    }


    // R, the radius, generally hovers around 210 (at least it does with mine)
   // int R() {
   //     return sqrt(readAccelX() * readAccelX() +readAccelY() * readAccelY() + readAccelZ() * readAccelZ());  
   // }


    // returns roll degrees
    int readRoll() {
        return (int)(atan2(readAccelX(),readAccelZ())/ M_PI * 180.0);
    }

    // returns pitch in degrees
    int readPitch() {        
        return (int) (acos(readAccelY()/RADIUS)/ M_PI * 180.0);  // optionally swap 'RADIUS' for 'R()'
    }

    byte _nunchuk_decode_byte (byte x)
        {
            //x = (x ^ 0x17) + 0x17;
            return x;
        }

   void _send_zero()
        {
            Wire.beginTransmission (0x52);      // transmit to device 0x52
            Wire.send (0x00);           // sends one byte
            Wire.endTransmission ();    // stop transmitting
        }
int count = 0;
void loop()
{
  
  update();
  SGFilter();
  int nozz = map( aveJY, 5, -127, 0, 255 );
  if( nozz < 0 )
    nozz = 0;
  if( nozz > 255 )
    nozz = 255;

  if( Serial.available() > 0  )
  {
  digitalWrite(LED,HIGH);  
      char q = Serial.read();
    

//  Serial.print( "" );

  if( q == 'n' )
  {
  Serial.print( nozz, BYTE );
  }
  else if( q == 'z' )
  {
  
  Serial.print( Z, BYTE );
  }
    digitalWrite(LED,LOW);
  }

/*  
  Serial.print(aveJX, DEC);
  Serial.print("\t");
  Serial.print(aveJY, DEC);
  Serial.print("\t");
  Serial.print(aveAX, DEC);
  Serial.print("\t");
  Serial.print(aveAY, DEC);
  Serial.print("\t");
  Serial.print( Z, DEC );
  Serial.print("\t");
  Serial.print( C, DEC );
  Serial.print("\t");
  Serial.print( connection, DEC );
  Serial.print("\r\n");
  */
}

void SGFilter()
{
  //Savitzky-Golay Filter
  for(int index = 0; index<6; index++) {
    readJX[index] = readJX[index+1];
    readJY[index] = readJY[index+1];
    readAX[index] = readAX[index+1];
    readAY[index] = readAY[index+1];
  }
  
  readJX[6] = readJoyX();
  readJY[6] = readJoyY();
  readAX[6] = (int)readAccelX();
  readAY[6] = (int)readAccelY();

  if(buttZ() == true) Z=1;
  else Z=0;

  if(buttC() == true) C=1;
  else C=0;
  
    aveJX = ((-2*readJX[0]) + (3*readJX[1]) + (6*readJX[2]) + (7*readJX[3]) + (6*readJX[4]) + (3*readJX[5]) - (2*readJX[6]))/21;
  
    aveJY = ((-2*readJY[0]) + (3*readJY[1]) + (6*readJY[2]) + (7*readJY[3]) + (6*readJY[4]) + (3*readJY[5]) - (2*readJY[6]))/21;
  
    aveAX = ((-2*readAX[0]) + (3*readAX[1]) + (6*readAX[2]) + (7*readAX[3]) + (6*readAX[4]) + (3*readAX[5]) - (2*readAX[6]))/21;
    //aveAX = 2.5*aveAX;
    //aveAX = constrain(aveAX, -500, 500);
  
    aveAY = ((-2*readAY[0]) + (3*readAY[1]) + (6*readAY[2]) + (7*readAY[3]) + (6*readAY[4]) + (3*readAY[5]) - (2*readAY[6]))/21;
    //aveAY = 2.5*aveAY;
    //aveAY = constrain(aveAY, -500, 500);
} 

