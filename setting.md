# Setting Values
In most cases, it will not work properly if you don't do it with the right settings.  
The set values in the sample code are for reference only. You may be able to use them as they are, but it is a must to change them and find a value that fits your pad.  

## Read Raw Values for Threshold and Sensitivity  
The easiest way to find the right setting value for threshold and sensitivity is to see what values can be read when you hit the pad.  
So first let's use the built-in example, [AnalogReadSerial.ino](https://github.com/arduino/arduino-examples/blob/main/examples/01.Basics/AnalogReadSerial/AnalogReadSerial.ino).  
analogRead can read values of 0-1023.
And since the library sets the range of values to 1-100, actually 10 times the value of the setting is compared to the value of analogRead.  

Or use below codes. Then open a serial monitor or a serial plotter and check the value.  
- **Single Piezo or Hihat Controller**
   ```cpp
    void setup() {
        Serial.begin(38400);
    }

    void loop() {
        int piezoValue = analogRead(A0); //Connect the piezo or switch to the A0 pin
        Serial.println(piezoValue);
    }
    ```

- **Dual Piezo or Cymbals**
   ```cpp
    void setup() {
        Serial.begin(38400);
    }

    void loop() {
        int piezoValue = analogRead(A0); //Connect the piezo or switch to the A0 pin
        int edgeValue = analogRead(A1);  //Connect the piezo or switch to the A1 pin
        Serial.print(piezoValue);
        Serial.print(", ");
        Serial.println(edgeValue);
    }
    ```

## Sensitivity   
It's a bit complicated, but the higher this value, the lower the sensitivity. For example, when Sensitivity is set to 100, Velocity will be 127 if the value is greater than 1000 in analogRead. Lesser values will change depending on the value of Threshold. In other words, if you make the Sensitivity and Threshold values the same, Velocity will be constant.

## Threshold
It is the threshold for being recognized as a hit. For example, if you set Threshold to 10, when the value of analogRead exceeds 100, a peak scan will begin. So it's recognized as a hit.  
Sometimes there is USB noise on the piezo signal. In that case, the value of analogRead without giving the signal to the piezo is not 0, but a value of 10 or more. If Threshold is set to 1 at this time, it will continue to be recognized as a hit. **If the MIDI signal continues to be sent even though you are not hitting it, try setting Threshold higher first.** And check the value is in analogRead.

## Edge Threshold (Cymbal only)
In the case of cymbals, if the value of the edge switch exceeds this Edge Threshold when the piezo value begins to scan above the Threshold, it is determined to be a hit on the edge. So you need to know how much value the analogRead is when you press the switch on the edge.  
For example, suppose you press an edge switch and the analogRead value is 100. Then, the Edge Threshold should be 8 or 9.

## Cup Threshold (3-Zone Cymbal only)
The method is almost the same as the Edge Threshold. When a scan starts with a piezo value above the Threshold, it is recognized as a hit to the cup if the switch value is above this Cup Threshold.
You need to know how much the value of the analogRead is when you press the edge switch and the cup switch. If the two values are too close, you may need to change the circuit to make it easier to determine.

## Scan Time
The value of a piezo is not always the highest value at the moment of hitting it. To get the value based on the strength of the hit, keep monitoring the piezo value for a certain time and get the peak value from it. This time is the Scan Time. The unit is ms. For example, if the Scan Time is set to 10, the piezo value is monitored for 10 milliseconds after the threshold is exceeded. The highest value is determined to be the peak and converted to velocity. Scan Time should be less than 10ms.

## Mask Time
The vibration of the piezo lasts for some time after hitting. It may continue to vibrate even after sending a MIDI signal. And may exceed the Threshold. Then the MIDI signal is sent multiple times in succession, even if you think you are only hitting it once. This is called Retrigger or Double Trigger.
To prevent this, ignore the piezo value for a period of time immediately after the hit. This time is the Mask Time. The unit is ms. Note that if this time is too long, there may be hits that go unrecognized when hit in succession.
