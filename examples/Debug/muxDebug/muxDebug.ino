/******************************************************************************
Mux_Analog_Input
SparkFun Multiplexer Analog Input Example
Jim Lindblom @ SparkFun Electronics
August 15, 2016
https://github.com/sparkfun/74HC4051_8-Channel_Mux_Breakout

This sketch demonstrates how to use the SparkFun Multiplexer
Breakout - 8 Channel (74HC4051) to read eight, separate
analog inputs, using just a single ADC channel.

Hardware Hookup:
Mux Breakout ----------- Arduino
     S0 ------------------- 2
     S1 ------------------- 3
     S2 ------------------- 4
     Z -------------------- A0
    VCC ------------------- 5V
    GND ------------------- GND
    (VEE should be connected to GND)

The multiplexers independent I/O (Y0-Y7) can each be wired
up to a potentiometer or any other analog signal-producing
component.

Development environment specifics:
Arduino 1.6.9
SparkFun Multiplexer Breakout - 8-Channel(74HC4051) v10
(https://www.sparkfun.com/products/13906)
******************************************************************************/
/////////////////////
// Pin Definitions //
/////////////////////
const int selectPins[3] = {4, 16, 2}; // S0~2, S1~3, S2~4
const int zOutput = 5;
const int zInput = 36; // Connect common (Z) to A0 (analog input)

void setup()
{
  Serial.begin(38400); // Initialize the serial port
  // Set up the select pins as outputs:
  for (int i = 0; i < 3; i++)
  {
    pinMode(selectPins[i], OUTPUT);
    digitalWrite(selectPins[i], HIGH);
  }
  pinMode(zInput, INPUT); // Set up Z as an input

  // Print the header:
  //Serial.println("Y0\tY1\tY2\tY3\tY4\tY5\tY6\tY7");
  //Serial.println("---\t---\t---\t---\t---\t---\t---\t---");
}

void loop()
{
  // Loop through all eight pins.
  for (byte pin = 0; pin <= 7; pin++)
  {
    selectMuxPin(pin); // Select one at a time
    if (pin == 0)
    {
      int inputValue0 = analogRead(zInput);
      Serial.print(inputValue0);
      Serial.print(",");
    }
    else if (pin == 1)
    {
      int inputValue1 = analogRead(zInput);
      Serial.print(inputValue1);
      Serial.print(",");
    }
    else if (pin == 2)
    {
      int inputValue2 = analogRead(zInput);
      Serial.print(inputValue2);
      Serial.print(",");
    }
    else if (pin == 3)
    {
      int inputValue3 = analogRead(zInput);
      Serial.print(inputValue3);
      Serial.print(",");
    }
    else if (pin == 4)
    {
      int inputValue4 = analogRead(zInput);
      Serial.print(inputValue4);
      Serial.print(",");
    }
    else if (pin == 5)
    {
      int inputValue5 = analogRead(zInput);
      Serial.print(inputValue5);
      Serial.print(",");
    }
    else if (pin == 6)
    {
      int inputValue6 = analogRead(zInput);
      Serial.print(inputValue6);
      Serial.print(",");
    }
    else if (pin == 7)
    {
      int inputValue7 = analogRead(zInput);
      Serial.print(inputValue7);
      Serial.print(",");
    }
  }
  Serial.println();
  //delay(1000);
}

// The selectMuxPin function sets the S0, S1, and S2 pins
// accordingly, given a pin from 0-7.
void selectMuxPin(byte pin)
{
  for (int i = 0; i < 3; i++)
  {
    if (pin & (1 << i))
      digitalWrite(selectPins[i], HIGH);
    else
      digitalWrite(selectPins[i], LOW);
  }
}