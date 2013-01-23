struct time
{
    int hour;
    int minute;
};

const int num_pots = 3;
const int pot_pins[num_pots] = { 1,2,3 };
const int time_slots = 13;

const time times[time_slots][num_pots] = 
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
        { {10,0}, {-1,-1}, {21,0	} },
        { {10,30}, {-1,-1}, {21,30	} },
    };


void loop()
{}
void setup()
{}

struct time get_time(int pot_num)
{
    int index = map(analogRead(pot_pins[pot_num]),0,1024,0,time_slots);
    return times[pot_num][index];
}

