#define _CS 2 //pin 3
#define _WR 3 //pin 9
#define A_0 12 //pin 7
#define D0 4 //pin 11 LSB
#define D1 5 //pin 13
#define D2 6 //pin 15
#define D3 7 //pin 17
#define D4 8 //pin 19
#define D5 9 //pin 21
#define D6 10 //pin 23
#define D7 11 //pin 25 MSB
#define LED 13
int data_pins [8] = { D0, D1, D2, D3, D4, D5, D6, D7 };

//unsure if CS is needed. needs to be low for writing
//A0 might be needed for some intructions like reading data, needs to be low for writing
void setup()
{
  Serial.begin(9600);
  Serial.println("started");
  pinMode(_CS,OUTPUT);
  pinMode(_WR,OUTPUT);
  pinMode(A_0,OUTPUT);
  pinMode(LED,OUTPUT);
  digitalWrite(A_0,LOW);
  digitalWrite(_WR,HIGH);
  digitalWrite(_CS,LOW);

  for(int i=0; i<8; i++)
  {
    pinMode(data_pins[i],OUTPUT);
    digitalWrite(data_pins[i],LOW);
  }
 
}

void loop()
{
  String str = String(millis());
  send_string(str);
  
  delay(100);
  blink();  
 
}

void send_string(String str)
{
  load_char(0x0a); //newline
  for(int i=0; i<str.length(); i++)
 {
    load_char(str.charAt(i));
 } 
}
void blink()
{
  digitalWrite(LED,HIGH);
  delay(100);
  digitalWrite(LED,LOW);
}


void load_char(char c)
{
  send_byte(c);
  
 // digitalWrite(A_0,LOW);
 // digitalWrite(_CS,LOW);

  delay(1);

  digitalWrite(_WR,LOW);
  delay(1);
  digitalWrite(_WR,HIGH);
  
  delay(1);
 // digitalWrite(_CS,HIGH);
 // digitalWrite(A_0,HIGH);

  clear_all_data();
}
void clear_all_data()
{
   for(int i=0; i<8; i++)
     digitalWrite(data_pins[i],LOW);
}
void send_byte(char send_byte)
{
  for(int i=0; i<8; i++)
  {
    if(send_byte & (1<<i))
     {
       Serial.print("1");
       digitalWrite(data_pins[i],HIGH);
     }
     else
     {
       Serial.print("0");
       digitalWrite(data_pins[i],LOW);
     }  
  }
  Serial.println("");
}
