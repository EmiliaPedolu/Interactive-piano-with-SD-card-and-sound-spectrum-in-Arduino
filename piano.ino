#include <Wire.h>                     //for I2C communication
#include <LiquidCrystal_I2C.h>        //LCD library for I2C LCDs
#include <TMRpcm.h>                   //for audio playback from SD card
#define SD_ChipSelectPin 10           //Chip select pin for SD card
#include <SPI.h>                      //SPI communication (used for SD card)
#include <SD.h>                       //SD card library
#include "LedControl.h"              //for controlling LED matrix display

//initialize audio, LCD, and LED matrix
TMRpcm sound;
LiquidCrystal_I2C lcd(0x27, 16, 2);   //LCD address 0x27, 16x2 display
LedControl lc = LedControl(8, 2, 7, 1); //DataIn=8, CLK=2, LOAD=7, 1 device
const int maxScale = 11;            

// Sound sensor setup
const int sensorPin = A1;
const int sampleWindow = 50;          // sampling time window in milliseconds
unsigned int sample;                  // variable to store analog read sample

//timing variables
unsigned long startMillis;
unsigned long timeCycle;

unsigned int signalMax = 0;
unsigned int signalMin = 1024;
unsigned char index = 0; //index for circular buffer

//arrays to track signal peak and display values
unsigned int peakToPeak[8];
unsigned int displayPeak[8];
unsigned int temp[8] = {0, 0, 0, 0, 0, 0, 0, 0};//temporary buffer for smooth LED update
unsigned int signalMaxBuff[8];
unsigned int signalMinBuff[8];

//multiplexer setup
int muxS0 = 3;
int muxS1 = 4;
int muxS2 = 5;
int muxS3 = 6;
int muxSIG = A0;//signal pin from multiplexer

int buzzer = 9;//buzzer pin

//button handling variables
int counter = 0;
int stateB10 = 0;
int lastButtonStateB10 = LOW;

//function to set the multiplexer channel
void SetMuxChannel(byte channel) {
    digitalWrite(muxS0, bitRead(channel, 0));
    digitalWrite(muxS1, bitRead(channel, 1));
    digitalWrite(muxS2, bitRead(channel, 2));
    digitalWrite(muxS3, bitRead(channel, 3));
    delay(5);
}

void setup() {
    Serial.begin(9600);
    Serial.println("Initializing...");

    if (!SD.begin(SD_ChipSelectPin)) {
        Serial.println("SD Initialization failed...");
        return;
    }
    Serial.println("SD Initialization done...");

    //setup sound parameters
    sound.speakerPin = 9;
    sound.quality(1);        //set higher quality
    sound.setVolume(5);      //volume level 0–7

    //initialize LCD
    lcd.init();
    lcd.backlight();
    lcd.print("Hi");
    delay(10);

  //set multiplexer control pins as output
    pinMode(muxS0, OUTPUT);
    pinMode(muxS1, OUTPUT);
    pinMode(muxS2, OUTPUT);
    pinMode(muxS3, OUTPUT);

    pinMode(buzzer, OUTPUT);//set buzzer pin as output

    //initialize LED matrix
    lc.shutdown(0, false);   //wake up LED matrix
    lc.setIntensity(0, 1);   //set brightness (0–15)
    lc.clearDisplay(0);      //clear display
}

void loop() {
    readSoundSensor();   //sample audio levels
    updateLEDMatrix();   //display levels on LED matrix
    handleButtons();     //read button inputs
}

//function to read audio sensor and store min/max signal
void readSoundSensor() {
    startMillis = millis();
    signalMax = 0;
    signalMin = 1024;

    //collect samples for a given window
    while (millis() - startMillis < sampleWindow) {
        sample = analogRead(sensorPin);
        if (sample < 1024) {
            signalMax = max(signalMax, sample);
            signalMin = min(signalMin, sample);
        }
    }

    //store values in buffers
    signalMaxBuff[index] = signalMax;
    signalMinBuff[index] = signalMin;
    index = (index + 1) % 8; //move to next index
}

void updateLEDMatrix() {
    for (int i = 0; i < 8; i++) {
        peakToPeak[i] = signalMaxBuff[i] - signalMinBuff[i];
        displayPeak[i] = map(peakToPeak[i], 0, 1023, 0, maxScale);

        if (displayPeak[i] >= temp[i]) {
            temp[i] = displayPeak[i];
        } else {
            temp[i]--;
        }

        lc.setLed(0, i, temp[i], true);
        delayMicroseconds(250);
    }
}

void handleButtons() {
    for (byte i = 0; i < 10; i++) {
        SetMuxChannel(i);
        int buttonState = digitalRead(muxSIG);

        if (buttonState == HIGH) {
            noTone(buzzer);  //stop any ongoing tone before setting LED
            lcd.clear();
            delay(10);
            switch (i) {
                case 0:
                    if (lastButtonStateB10 == LOW && stateB10 == 0) {
                        counter++;
                        lcd.clear();
                        delay(10);
                        if (counter % 2 == 1) {
                            lcd.print("Playing Music");
                            sound.play("song.wav");
                        } else {
                            lcd.print("Music Stopped");
                            sound.stopPlayback();
                        }
                        stateB10 = 1;
                    }
                    break;
                case 1: lcd.print("Note: Re#"); tone(buzzer, 587, 100); break;//button 10
                case 2: lcd.print("Note: Do#"); tone(buzzer, 523, 100); break;//button 9
                case 3: lcd.print("Note: Si"); tone(buzzer, 494, 100); break;
                case 4: lcd.print("Note: La"); tone(buzzer, 440, 100); break;
                case 5: lcd.print("Note: Sol"); tone(buzzer, 392, 100); break;
                case 6: lcd.print("Note: Fa"); tone(buzzer, 349, 100); break;
                case 7: lcd.print("Note: Mi"); tone(buzzer, 330, 100); break;
                case 8: lcd.print("Note: Re"); tone(buzzer, 294, 100); break;
                case 9: lcd.print("Note: Do"); tone(buzzer, 262, 100); break;
            }
        } else {
            if (lastButtonStateB10 == HIGH) {
                stateB10 = 0;
            }
        }
        lastButtonStateB10 = buttonState;
    }
}
