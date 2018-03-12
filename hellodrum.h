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
    void HelloDrum::hihat(int sens, int thre1, int thre2, int retri);
    void HelloDrum::hihatControl(int sens, int thre1, int thre2, int retri);

    int velocity;
    int note;
    int exValue;
    bool hit;
    bool hit_rim;
    bool hit_edge;
    bool hit_cup;
    bool choke;
    bool flag;
    bool chokeFlag;
    unsigned long time_hit;
    unsigned long time_end;
    unsigned long time_choke;

  private:
    int pin_1;
    int pin_2;
    int piezoValue;
    int sensorValue;
};

#endif
