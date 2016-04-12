void setup()
{
    Serial.begin(9600);
    Serial.print("OK\n");
    randomSeed(analogRead(A0));
}

int num = 1;
void loop()
{
    if(Serial.available())
    {
        delay(100);
        get_random();
    }
}
bool get_random()
{
    int albums = 1;
    String choice;
    while(true)
    {
        String line = Serial.readStringUntil('\n');
        if(line == "OK")
        {
            Serial.print("got OK\n");
            break;
        }
        int ind = line.indexOf(':');
        if(ind == -1)
        {
            Serial.print("unexpected line: ");
            Serial.print(line);
            Serial.print("\n");
            return false;
        }
        String album = line.substring(ind + 2);
        album.trim();

        if(album.length() == 0)
            continue;

        albums += 1;
        //Serial.println(album);
        if(random(1, albums) == 1)
            choice = album;
    }
    Serial.print("choice:");
    Serial.print(choice);
    Serial.print("\n");
}
        
