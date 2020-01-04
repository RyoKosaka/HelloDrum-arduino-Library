/*
    EXAMPLE - Teensy Simple WAV Player with Teensy Audio Audio Shield

    Library : https://github.com/RyoKosaka/HelloDrum-arduino-Library
    Blog : https://open-e-drums.tumblr.com/
*/

/* NOTICE

    This code is unstable!

*/

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <hellodrum.h>

int PAD1[5] = {
    100, //sensitivity (1-100)
    10,  //threshold (1-100)
    30,  //scan time (1-)
    10,  //mask time (1-)
    38   //note (0-127)
};

//name your pad
HelloDrum pad1(7); //connect to A7(21) pin

//Teensy Audio
AudioPlaySdWav playSdWav1;
AudioPlaySdWav playSdWav2;
AudioPlaySdWav playSdWav3;
AudioPlaySdWav playSdWav4;
AudioPlaySdWav playSdWav5;
AudioPlaySdWav playSdWav6;
AudioPlaySdWav playSdWav7;
AudioPlaySdWav playSdWav8;
AudioPlaySdWav playSdWav9;
AudioPlaySdWav playSdWav10;
AudioPlaySdWav playSdWav11;
AudioPlaySdWav playSdWav12;
AudioPlaySdWav playSdWav13;
AudioPlaySdWav playSdWav14;
AudioPlaySdWav playSdWav15;
AudioPlaySdWav playSdWav16;
AudioMixer4 mixerLeft1;
AudioMixer4 mixerLeft2;
AudioMixer4 mixerLeft3;
AudioMixer4 mixerLeft4;
AudioMixer4 mixerRight1;
AudioMixer4 mixerRight2;
AudioMixer4 mixerRight3;
AudioMixer4 mixerRight4;
AudioMixer4 mixerLeftMaster;
AudioMixer4 mixerRightMaster;
AudioOutputI2S i2s1;
AudioConnection patchCord1(playSdWav14, 0, mixerLeft4, 1);
AudioConnection patchCord2(playSdWav14, 1, mixerRight4, 1);
AudioConnection patchCord3(playSdWav10, 0, mixerLeft3, 1);
AudioConnection patchCord4(playSdWav10, 1, mixerRight3, 1);
AudioConnection patchCord5(playSdWav15, 0, mixerLeft4, 2);
AudioConnection patchCord6(playSdWav15, 1, mixerRight4, 2);
AudioConnection patchCord7(playSdWav11, 0, mixerLeft3, 2);
AudioConnection patchCord8(playSdWav11, 1, mixerRight3, 2);
AudioConnection patchCord9(playSdWav13, 0, mixerLeft4, 0);
AudioConnection patchCord10(playSdWav13, 1, mixerRight4, 0);
AudioConnection patchCord11(playSdWav9, 0, mixerLeft3, 0);
AudioConnection patchCord12(playSdWav9, 1, mixerRight3, 0);
AudioConnection patchCord13(playSdWav6, 0, mixerLeft2, 1);
AudioConnection patchCord14(playSdWav6, 1, mixerRight2, 1);
AudioConnection patchCord15(playSdWav2, 0, mixerLeft1, 1);
AudioConnection patchCord16(playSdWav2, 1, mixerRight1, 1);
AudioConnection patchCord17(playSdWav16, 0, mixerLeft4, 3);
AudioConnection patchCord18(playSdWav16, 1, mixerRight4, 3);
AudioConnection patchCord19(playSdWav12, 0, mixerLeft3, 3);
AudioConnection patchCord20(playSdWav12, 1, mixerRight3, 3);
AudioConnection patchCord21(playSdWav7, 0, mixerLeft2, 2);
AudioConnection patchCord22(playSdWav7, 1, mixerRight2, 2);
AudioConnection patchCord23(playSdWav3, 0, mixerLeft1, 2);
AudioConnection patchCord24(playSdWav3, 1, mixerRight1, 2);
AudioConnection patchCord25(playSdWav5, 0, mixerLeft2, 0);
AudioConnection patchCord26(playSdWav5, 1, mixerRight2, 0);
AudioConnection patchCord27(playSdWav1, 0, mixerLeft1, 0);
AudioConnection patchCord28(playSdWav1, 1, mixerRight1, 0);
AudioConnection patchCord29(playSdWav8, 0, mixerLeft2, 3);
AudioConnection patchCord30(playSdWav8, 1, mixerRight2, 3);
AudioConnection patchCord31(playSdWav4, 0, mixerLeft1, 3);
AudioConnection patchCord32(playSdWav4, 1, mixerRight1, 3);
AudioConnection patchCord33(mixerLeft4, 0, mixerLeftMaster, 3);
AudioConnection patchCord34(mixerLeft2, 0, mixerLeftMaster, 1);
AudioConnection patchCord35(mixerLeft1, 0, mixerLeftMaster, 0);
AudioConnection patchCord36(mixerLeft3, 0, mixerLeftMaster, 2);
AudioConnection patchCord37(mixerRight2, 0, mixerRightMaster, 1);
AudioConnection patchCord38(mixerRight3, 0, mixerRightMaster, 2);
AudioConnection patchCord39(mixerRight4, 0, mixerRightMaster, 3);
AudioConnection patchCord40(mixerRight1, 0, mixerRightMaster, 0);
AudioConnection patchCord41(mixerLeftMaster, 0, i2s1, 0);
AudioConnection patchCord42(mixerRightMaster, 0, i2s1, 1);
AudioControlSGTL5000 sgtl5000_1;

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN 10
#define SDCARD_MOSI_PIN 7
#define SDCARD_SCK_PIN 14

// Use these with the Teensy 3.5 & 3.6 SD card
//#define SDCARD_CS_PIN    BUILTIN_SDCARD
//#define SDCARD_MOSI_PIN  11  // not actually used
//#define SDCARD_SCK_PIN   13  // not actually used

// Use these for the SD+Wiz820 or other adaptors
//#define SDCARD_CS_PIN    4
//#define SDCARD_MOSI_PIN  11
//#define SDCARD_SCK_PIN   13

void setup()
{
    // Audio connections require memory to work.  For more
    // detailed information, see the MemoryAndCpuUsage example
    AudioMemory(20);

    // Comment these out if not using the audio adaptor board.
    // This may wait forever if the SDA & SCL pins lack
    // pullup resistors
    sgtl5000_1.enable();
    sgtl5000_1.volume(1.0);

    SPI.setMOSI(SDCARD_MOSI_PIN);
    SPI.setSCK(SDCARD_SCK_PIN);
    if (!(SD.begin(SDCARD_CS_PIN)))
    {
        // stop here, but print a message repetitively
        while (1)
        {
            Serial.println("Unable to access the SD card");
            delay(500);
        }
    }

    //mixer gain
    mixerLeft1.gain(0,0.25);
    mixerLeft1.gain(1,0.25);
    mixerLeft1.gain(2,0.25);
    mixerLeft1.gain(3,0.25);
    mixerLeft2.gain(0,0.25);
    mixerLeft2.gain(1,0.25);
    mixerLeft2.gain(2,0.25);
    mixerLeft2.gain(3,0.25);
    mixerLeft3.gain(0,0.25);
    mixerLeft3.gain(1,0.25);
    mixerLeft3.gain(2,0.25);
    mixerLeft3.gain(3,0.25);
    mixerLeft4.gain(0,0.25);
    mixerLeft4.gain(1,0.25);
    mixerLeft4.gain(2,0.25);
    mixerLeft4.gain(3,0.25);
    mixerRight1.gain(0,0.25);
    mixerRight1.gain(1,0.25);
    mixerRight1.gain(2,0.25);
    mixerRight1.gain(3,0.25);
    mixerRight2.gain(0,0.25);
    mixerRight2.gain(1,0.25);
    mixerRight2.gain(2,0.25);
    mixerRight2.gain(3,0.25);
    mixerRight3.gain(0,0.25);
    mixerRight3.gain(1,0.25);
    mixerRight3.gain(2,0.25);
    mixerRight3.gain(3,0.25);
    mixerRight4.gain(0,0.25);
    mixerRight4.gain(1,0.25);
    mixerRight4.gain(2,0.25);
    mixerRight4.gain(3,0.25);
    mixerLeftMaster.gain(0,0.25);
    mixerLeftMaster.gain(1,0.25);
    mixerLeftMaster.gain(2,0.25);
    mixerLeftMaster.gain(3,0.25);
    mixerRightMaster.gain(0,0.25);
    mixerRightMaster.gain(1,0.25);
    mixerRightMaster.gain(2,0.25);
    mixerRightMaster.gain(3,0.25);
}

void playFile(const char *filename)
{
    if (playSdWav1.isPlaying() == false)
    {
        playSdWav1.play(filename);
    }
    else if (playSdWav2.isPlaying() == false)
    {
        playSdWav2.play(filename);
    }
    else if (playSdWav3.isPlaying() == false)
    {
        playSdWav3.play(filename);
    }
    else if (playSdWav4.isPlaying() == false)
    {
        playSdWav4.play(filename);
    }
    else if (playSdWav5.isPlaying() == false)
    {
        playSdWav5.play(filename);
    }
    else if (playSdWav6.isPlaying() == false)
    {
        playSdWav6.play(filename);
    }
    else if (playSdWav7.isPlaying() == false)
    {
        playSdWav7.play(filename);
    }
    else if (playSdWav8.isPlaying() == false)
    {
        playSdWav8.play(filename);
    }
    else if (playSdWav9.isPlaying() == false)
    {
        playSdWav9.play(filename);
    }
    else if (playSdWav10.isPlaying() == false)
    {
        playSdWav10.play(filename);
    }
    else if (playSdWav11.isPlaying() == false)
    {
        playSdWav11.play(filename);
    }
    else if (playSdWav12.isPlaying() == false)
    {
        playSdWav12.play(filename);
    }
    else if (playSdWav13.isPlaying() == false)
    {
        playSdWav13.play(filename);
    }
    else if (playSdWav14.isPlaying() == false)
    {
        playSdWav14.play(filename);
    }
    else if (playSdWav15.isPlaying() == false)
    {
        playSdWav15.play(filename);
    }
    else if (playSdWav16.isPlaying() == false)
    {
        playSdWav16.play(filename);
    }
}

void loop()
{
    pad1.singlePiezo(PAD1[0], PAD1[1], PAD1[2], PAD1[3]);

    if (pad1.hit == true)
    {
        if (pad1.velocity < 25)
        {
            playFile("snare1.wav");
        }
        if (pad1.velocity >= 25 && pad1.velocity < 50)
        {
            playFile("snare2.wav");
        }
        if (pad1.velocity >= 50 && pad1.velocity < 75)
        {
            playFile("snare3.wav");
        }
        if (pad1.velocity >= 75 && pad1.velocity < 100)
        {
            playFile("snare4.wav");
        }
        if (pad1.velocity >= 100)
        {
            playFile("snare5.wav");
        }
    }
}
