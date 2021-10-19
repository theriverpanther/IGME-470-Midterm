/*
 File/Sketch Name: AudioFrequencyDetector

 Version No.: v1.0 Created 12 December, 2019
 
 Original Author: Clyde A. Lettsome, PhD, PE, MEM
 
 Description:  This code/sketch makes displays the approximate frequency of the loudest sound detected by a sound detection module. For this project, the analog output from the 
 sound module detector sends the analog audio signal detected to A0 of the Arduino Uno. The analog signal is sampled and quantized (digitized). A Fast Fourier Transform (FFT) is
 then performed on the digitized data. The FFT converts the digital data from the approximate discrete-time domain result. The maximum frequency of the approximate discrete-time
 domain result is then determined and displayed via the Arduino IDE Serial Monitor.

 Note: The arduinoFFT.h library needs to be added to the Arduino IDE before compiling and uploading this script/sketch to an Arduino.

 License: This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License (GPL) version 3, or any later
 version of your choice, as published by the Free Software Foundation.

 Notes: Copyright (c) 2019 by C. A. Lettsome Services, LLC
 For more information visit https://clydelettsome.com/blog/2019/12/18/my-weekend-project-audio-frequency-detector-using-an-arduino/

*/

#include "arduinoFFT.h"
 
#define SAMPLES 128             //SAMPLES-pt FFT. Must be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 2048 //Ts = Based on Nyquist, must be 2 times the highest expected frequency.

// The average frequencies of each note as played on the ocarina
#define A_FREQ 460
#define B_FREQ 495
#define C_FREQ 530
#define D_FREQ 590
#define E_FREQ 670
#define F_FREQ 730
#define G_FREQ 875 
#define A2_FREQ 890
#define RANGE 10
 
arduinoFFT FFT = arduinoFFT();
 
unsigned int samplingPeriod;
unsigned long microSeconds;
int numInput = -1;
bool prevPressed = false;

int pauseDuration = 1000;
unsigned long currentMillis = 0;
unsigned long prevStateMillis = 0;
char prevChar = 'L';
 
double vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
double vImag[SAMPLES]; //create vector of size SAMPLES to hold imaginary values

uint8_t buff[8] = {0};
 
void setup() 
{
    Serial.begin(9600); //Baud rate for the Serial Monitor
    samplingPeriod = round(1000000*(1.0/SAMPLING_FREQUENCY)); //Period in microseconds 
}
 
void loop() 
{  
    /*Sample SAMPLES times*/
    
    for(int i=0; i<SAMPLES; i++)
    {
        microSeconds = micros();    //Returns the number of microseconds since the Arduino board began running the current script. 
     
        vReal[i] = analogRead(0); //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
        vImag[i] = 0; //Makes imaginary term 0 always

        /*remaining wait time between samples if necessary*/
        while(micros() < (microSeconds + samplingPeriod))
        {
          //do nothing
        }
    }
 
    /*Perform FFT on samples*/
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

    /*Find peak frequency and print peak*/
    double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
    // Serial.println(peak);     //Print out the most dominant frequency.
    
    currentMillis = millis();

    char character = translateFrequency(peak);

    if(character != prevChar || currentMillis - prevStateMillis >= pauseDuration)
    {
       writeToSerial(character);
       prevChar = character;
       prevStateMillis = currentMillis;
    }
}

void keyRelease()
{
  buff[0] = 0;
  buff[2] = 0;
  Serial.write(buff,8);
}

char translateFrequency(double peak)
{
    //Translate Frequency to distinguished notes
    //Note A = A
    //Note B = B
    //Note C = Forward
    //Note D = Back
    //Note E = Left
    //Note F = Right
    //Note G = Start
    //Note A2 = Select
    char character;
    if(peak > A_FREQ - RANGE && peak <= A_FREQ + RANGE)
    {
      character = 'A';
    }
    if(peak > B_FREQ - RANGE && peak <= B_FREQ + RANGE)
    {
      character = 'B';
    }
    if(peak > C_FREQ - RANGE && peak <= C_FREQ + RANGE)
    {
      character = 'C';
    }
    if(peak > D_FREQ - RANGE && peak <= D_FREQ + RANGE)
    {
      character = 'D';
    }
    if(peak > E_FREQ - RANGE && peak <= E_FREQ + RANGE)
    {
      character = 'E';
    }
    if(peak > F_FREQ - RANGE && peak <= F_FREQ + RANGE)
    {
      character = 'F';
    }
    if(peak > G_FREQ - RANGE && peak <= G_FREQ + RANGE)
    {
      character = 'G';
    }
    if(peak > A2_FREQ - RANGE && peak <= A2_FREQ + RANGE)
    {
      character = 'H';
    }
    
    return character;
}

void writeToSerial(char character)
{
  //Print to Serial to Python
    switch(character)
    {
      case 'A':
        // X
        //buff[2] = 120;
        Serial.write(88);
        //keyRelease();
        break;
      case 'B':
        // Z
        //buff[2] = 122;
        Serial.write(90);
        //keyRelease();
        break;
      case 'C':
        // Arrow Up
        //buff[2] = 38;
        Serial.write(38);
        //keyRelease();
        break;
      case 'D':
        // Arrow Down
        //buff[2] = 40;
        Serial.write(40);
        //keyRelease();
        break;
      case 'E':
        // Arrow Left
        //buff[2] = 37;
        Serial.write(37);
        //keyRelease();
        break;
      case 'F':
        // Arrow Right
        //buff[2] = 39;
        Serial.write(39);
        //keyRelease();
        break;
      case 'G':
        // Enter
        //buff[2] = 10;
        Serial.write(10);
        //keyRelease();
        break;
      case 'H':
        // Shift
        //buff[2] = 16;
        // Press Shift Twice so the keyboard gets rid of caps + shift issues
        Serial.write(16);
        Serial.write(16);
        //keyRelease();
        break;
      default:
      break;
    }
}
