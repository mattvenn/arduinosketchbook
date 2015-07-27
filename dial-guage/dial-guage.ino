const int dial = 5;
const int range = 1024;
const int wait = 20;
void setup()
{
	pinMode(dial,OUTPUT);
}

void loop()
{
	for(int i =0; i< range; i++)
	{
		analogWrite(dial,i);
		delay(wait);
	}
	for(int i =0; i< range; i++)
	{
		analogWrite(dial,range-i);
		delay(wait);
	}
}


