/*
    EXAMPLE - 2-zone Hi-Hat with hihat Controller (FSR or TCRT5000)

    This sample code tesed with Roland VH-10.
    https://open-e-drums.tumblr.com/
*/

/* NOTICE

    You have to install the MIDI library.
    MIDI : https://playground.arduino.cc/Main/MIDILibrary

*/

//////////////////////////////////////////   SETTING VALUE   ////////////////////////////////////////////

//Determine the setting value.
//By changing the number in this array you can set sensitivity, threshold and so on.

byte HIHAT[10] = {
    100, //sensitivity
    10,  //threshold
    10,  //scan start
    30,  //scan end
    10,  //edge threshold
    46,  //note of open
    42,  //note of close
    26,  //note of open edge
    22,  //note of close edge
    1    //curve type(0-4)
};

byte HIHAT_PEDAL[7] = {
    90, //sensitivity
    30, //threshold
    60, //scan start
    80, //scan end
    10, //pedal sensitivity
    44, //note of pedal
    1   //curve type(0-4)
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <MIDI.h>
#include <hellodrum.h>
MIDI_CREATE_DEFAULT_INSTANCE();

//Please name your piezo and sensor.
//So, you need the same number of lines as the number of pads.
//The piezo named "hihat" is connected to the A0 pin and A1 pin.
//The FSR or TCRT5000 named "hihatPedal" is connected to the A2 pin.
HelloDrum hihat(0, 1);
HelloDrum hihatPedal(2);

void setup()
{
    //If you use Hairless MIDI, you have to comment out the next line.
    //MIDI.begin(10);

    //And uncomment the next two lines.
    MIDI.begin();
    Serial.begin(38400);

    //Set Curve Type
    hihat.setCurve(HIHAT[9]);
    hihatPedal.setCurve(HIHAT_PEDAL[6]);
}

void loop()
{
    //Piezo sensing is done in this line. And it is returned as a velocity of 127 stages.
    //For each pad, one line is required.
    //So, you need the same number of lines as the number of pads or controller.
    hihat.HH2zone(HIHAT[0], HIHAT[1], HIHAT[2], HIHAT[3], HIHAT[4]);                                         //HH2zone(byte sens, byte thre, byte scan, byte mask, byte edgeThre);
    hihatPedal.hihatControl(HIHAT_PEDAL[0], HIHAT_PEDAL[1], HIHAT_PEDAL[2], HIHAT_PEDAL[3], HIHAT_PEDAL[4]); //hihatControl(byte sens, byte thre, byte scanStart, byte scanEnd, byte pedalSens);

    //MIDI signals are transmitted with this IF statement.
    //bow
    if (hihat.hit == true)
    {
        //check open or close
        //1.open
        if (hihatPedal.openHH == true)
        {
            MIDI.sendNoteOn(HIHAT[5], hihat.velocity, 10); //(note, velocity, channel)
            MIDI.sendNoteOff(HIHAT[5], 0, 10);
        }
        //2.close
        else if (hihatPedal.closeHH == true)
        {
            MIDI.sendNoteOn(HIHAT[6], hihat.velocity, 10); //(note, velocity, channel)
            MIDI.sendNoteOff(HIHAT[6], 0, 10);
        }
    }

    //edge
    else if (hihat.hitRim == true)
    {
        //check open or close
        //1.open
        if (hihatPedal.openHH == true)
        {
            MIDI.sendNoteOn(HIHAT[7], hihat.velocity, 10); //(note, velocity, channel)
            MIDI.sendNoteOff(HIHAT[7], 0, 10);
        }
        //2.close
        else if (hihatPedal.closeHH == true)
        {
            MIDI.sendNoteOn(HIHAT[8], hihat.velocity, 10); //(note, velocity, channel)
            MIDI.sendNoteOff(HIHAT[8], 0, 10);
        }
    }

    //when pedal is closed
    if (hihatPedal.hit == true)
    {
        MIDI.sendNoteOn(HIHAT_PEDAL[5], hihatPedal.velocity, 10); //(note, velocity, channel)
        MIDI.sendNoteOff(HIHAT_PEDAL[5], 0, 10);
    }

    //sending state of pedal with controll change
    if (hihatPedal.moving == true)
    {
        MIDI.sendControlChange(4, hihatPedal.pedalCC, 10);
    }
}
