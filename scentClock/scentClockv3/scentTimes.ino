const int time_slots = 13;

const time times[time_slots][pots] = 
    { 
        { {0,0}, {0,0}, {0,0	} },
        { {5,0}, {11,0}, {16,0	} },
        { {5,30}, {11,30}, {16,30	} },
        { {6,0}, {12,0}, {17,0	} },
        { {6,30}, {12,30}, {17,30	} },
        { {7,0}, {13,0}, {18,0	} },
        { {7,30}, {13,30}, {18,30	} },
        { {8,0}, {14,0}, {19,0	} },
        { {8,30}, {14,30}, {19,30	} },
        { {9,0}, {15,0}, {20,0	} },
        { {9,30}, {15,30}, {20,30	} },
        { {10,0}, {15,30}, {21,0	} },
        { {10,30}, {15,30}, {21,30	} },
    };

struct time get_time(int pot_num)
{
    int avg = 0;
    for(int i=0; i<samples; i++)
    {
        avg+=analogRead(pot_pins[pot_num]);
        delay(5);
    }
    avg = avg/samples;
        
    int index = map(avg,0,1024,0,time_slots);
    /*
    Serial.print(pot_num);
    Serial.print(" = ");
    Serial.println(index);
    */
    return times[index][pot_num];
}

void printTime(struct time t)
{
    Serial.print(t.hour);
    Serial.print(":");
    Serial.print(t.min);
}
