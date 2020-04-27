/*
    EXAMPLE - Teensy Simple Drum Synthesizer with Teensy Audio Audio Shield

    Library : https://github.com/RyoKosaka/HelloDrum-arduino-Library
    Blog : https://open-e-drums.tumblr.com/
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

//name your pad and knobs
HelloDrum pad1(7);      //connect to A7(21) pin
HelloDrumKnob knob1(2); //connect to A2(16) pin
HelloDrumKnob knob2(3); //connect to A3(17) pin
HelloDrumKnob knob3(6); //connect to A6(20) pin

//Teesy Audio
AudioSynthNoiseWhite noise0;
AudioEffectEnvelope envelope0;
AudioSynthSimpleDrum drum0;
AudioMixer4 mixer0;
AudioOutputI2S i2s2;
AudioConnection patchCord1(noise0, envelope0);
AudioConnection patchCord2(envelope0, 0, mixer0, 1);
AudioConnection patchCord3(drum0, 0, mixer0, 0);
AudioConnection patchCord4(mixer0, 0, i2s2, 0);
AudioConnection patchCord5(mixer0, 0, i2s2, 1);
AudioControlSGTL5000 audioShield;

void setup()
{
    //Set Curve Type
    pad1.setCurve(1);

    // Audio connections require memory to work.  For more
    // detailed information, see the MemoryAndCpuUsage example
    AudioMemory(10);

    // turn on the output
    audioShield.enable();
    audioShield.volume(0.6);

    //drum and noise setting
    drum0.secondMix(0);
    drum0.pitchMod(1);
    envelope0.delay(0);
    envelope0.attack(0);
    envelope0.hold(0);
    envelope0.sustain(1);
    envelope0.decay(0);
}

void loop()
{
    //Read the piezo and knob
    pad1.singlePiezo(PAD1[0], PAD1[1], PAD1[2], PAD1[3]);
    knob1.read();
    knob2.read();
    knob3.read();

    //apply knob value
    float noiseLevel = knob3.knobValue;
    noise0.amplitude(noiseLevel / 2000);
    drum0.frequency(knob1.knobValue);
    drum0.length(knob2.knobValue * 2 + 50);
    envelope0.release(knob2.knobValue / 5 + 50);

    if (pad1.hit == true)
    {
        //apply velocity to gain
        float ampLevel = pad1.velocity;
        mixer0.gain(0, ampLevel / 127);
        mixer0.gain(1, ampLevel / 127);

        //noteOn
        drum0.noteOn();
        envelope0.noteOn();
    }

    //noise noteOff
    envelope0.noteOff();
}
