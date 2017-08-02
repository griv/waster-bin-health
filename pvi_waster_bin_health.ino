/**

   pvi waster bin - health
   v1.0
   Steve Berrick
   2017

**/

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <FadeLed.h>

AudioPlaySdWav           playWav1;
AudioOutputI2S           audioOutput;
AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);
AudioControlSGTL5000     sgtl5000_1;

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

bool playing = false;

// inbuilt led
const int led = 13;

// led strip 12v via mosfet
FadeLed light1(5);
FadeLed light2(6);

// headphone volume
const int volPin = A2;

// switch pin
const int switchPin = 2;

bool light1On = false;
bool light2On = false;

long previousMillis = 0;
long interval = 30; 

long startMillis = 0;

elapsedMillis msec = 0;
int randOn = 0;
int randOff = 0;

void setup() {

  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);

  Serial.begin(9600);

  Serial.println(" ");
  Serial.println("pvi collective: waster: health");
  Serial.println("______________________________");
  Serial.println(" ");

  // lights
  analogWriteFrequency(5, 375000);
  analogWriteFrequency(6, 375000);
  FadeLed::setInterval(1);
  light1.begin(0);
  light2.begin(0);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(8);

  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  // pullup resistors
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.37);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    bool blink = true;
    while (1) {
      Serial.println("Unable to access the SD card");
      if (blink) {
        digitalWrite(led, LOW);
        blink = false;
      } else {
        digitalWrite(led, HIGH);
        blink = true;
      }
      delay(500);
    }
  }

  // swicth
  pinMode(switchPin, INPUT_PULLUP);

  // random the random
  randomSeed(analogRead(0));

  // lights up
  light1.setTime(700);
  light1.set(100);
  light2.setTime(700);
  light2.set(100);

}

void playFile(const char *filename) {

  Serial.print("Playing file: ");
  Serial.println(filename);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playWav1.play(filename);

  // A brief delay for the library read WAV info
  delay(5);

  startMillis = millis();

}

void loop() {

  FadeLed::update();

  // Simply wait for the file to finish playing.
  if (playWav1.isPlaying()) {

    // if switch has been turned off - stop playing
    if (digitalRead(switchPin) == LOW) {

      playing = false;
      playWav1.stop();

      // bring lights back up
      light1.setTime(700);
      light1.set(100);
      light2.setTime(700);
      light2.set(100);

    } else {

      // update volume
      float vol = analogRead(volPin);
      vol = vol / 1024 * 0.75;
      //    Serial.println(vol);
      sgtl5000_1.volume(vol);

      // TODO: update flickery lights
      // 0 - 116 - full up 
      // 116 - 158 - phase 1 - flicker 1 - a little
      // 158 - 167 - full up
      // 167 - 193 - phase 2 - flicker 2 - a little more
      // 193 - 213 - full off - frank
      // 213 - 221 - flicker 1 
      // 221 - 229 - full up
      // 229 - 281 - flicker more - build over that time, 3 segments?
      // 243
      // 267
      // 281 - full up
      interval = 100;
      bool sureOn = false;
      bool sureOff = false;
      if (msec > 281000) {
        randOn = -1;
        randOff = 11;     
        sureOn = true;   
      } else if (msec > 267000) {
        interval = 30;
        randOn = 5;
        randOff = 5;        
      } else if (msec > 243000) {
        interval = 40;
        randOn = 4;
        randOff = 6;        
      } else if (msec > 229000) {
        interval = 50;
        randOn = 3;
        randOff = 7;        
      } else if (msec > 221000) {
        randOn = -1;
        randOff = 11;        
        sureOn = true;   
      } else if (msec > 213000) {
        interval = 60;
        randOn = 4;
        randOff = 8;        
      } else if (msec > 193000) {
        randOn = 11;
        randOff = -1;
        sureOff = true;        
      } else if (msec > 167000) {
        interval = 80;
        randOn = 3;
        randOff = 8;        
      } else if (msec > 158000) {
        randOn = -1;
        randOff = 11;        
        sureOn = true;   
      } else if (msec > 116000) {
        randOn = 2;
        randOff = 8;        
      } else {
        randOn = -1;
        randOff = 11;        
        sureOn = true;   
      }

      
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis > interval) {
        // save the last time you blinked the LED 
        previousMillis = currentMillis;  
      
        if (sureOn) {
          light1.setTime(170);
          light2.setTime(170);
          light1.set(100);
          light2.set(100);
        } else {

          if (sureOff) {

          light1.setTime(170);
          light2.setTime(170);
          light1.set(0);
          light2.set(0);

          } else {
            
            if (random(10) > randOn) {
              light1.setTime(10);
              light1.set(random(50, 100));
              Serial.println("L1 on");
            }
      
            if (random(10) > randOn) {
              light2.setTime(10);
              light2.set(random(50, 100));
              Serial.println("L2 on");
            }
      
            if (random(10) > randOff) {
              light1.setTime(10);
              light1.set(0);
              Serial.println("L1 off");
            }
      
            if (random(10) > randOff) {
              light2.setTime(10);
              light2.set(0);
              Serial.println("L2 off");
      
            }
            
          }


        }

      }
    }

  } else {

    // check for switch turned on
    if (digitalRead(switchPin) == HIGH) {

      // start playing if not already playing
      if (!playing) {
        playing = true;
        msec = 0;
        playFile("CRISIS/HEALTH.WAV");
      }

    }

  }

}

