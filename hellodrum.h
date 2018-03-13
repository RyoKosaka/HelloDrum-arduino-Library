#ifndef HelloDrum_h
#define HelloDrum_h

class HelloDrum
{
  public:
    HelloDrum(int pin1, int pin2);
    HelloDrum(int pin1);

    void HelloDrum::singlePiezo(int sens, int thre1, int thre2, int retri);

    //void singlePiezo(int sens, int thre1, int thre2, int retri);
    void HelloDrum::dualPiezo(int sens, int thre1, int thre2, int retri);
    void HelloDrum::cymbal3zone(int sens, int thre1, int thre2, int retri);
    void HelloDrum::cymbal2zone(int sens, int thre1, int thre2, int retri);
    void HelloDrum::TCRT5000(int sens, int thre1, int thre2);

    int velocity;
    int pedalCC;
    int note;
    int exValue;
    int exTCRT = 0;
    bool hit;
    bool openHH;
    bool closeHH;
    bool hit_rim;
    bool hit_edge;
    bool hit_cup;
    bool choke;
    bool moving;
    bool flag;
    bool pedalVelocityFlag;
    bool chokeFlag;
    unsigned long time_hit;
    unsigned long time_end;
    unsigned long time_choke;
    unsigned long time_hit_pedal_1;
    unsigned long time_hit_pedal_2;

  private:
    int pin_1;
    int pin_2;
    int piezoValue;
    int sensorValue;
};

#endif
