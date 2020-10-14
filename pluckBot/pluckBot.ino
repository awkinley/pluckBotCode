#include "Music.h"
#include "Tuning.h"

// Define pin connections & motor's steps per revolution
const int strumDirPin = 2;
const int strumStepPin = 3;
const int dirPin = 4;
const int stepPin = 5;

const int firstLEDPin = 7;

const int stepsPerRevolution = 200;
const int strumSteps = stepsPerRevolution / 3;

int stepsLeft = 0;
int strumStepsLeft = 0;

const int motorPotPin = A1;
const int buttonPin = 6;

const int potMax = 883;
const int potMin = 0;
const int potROM = 270; // degrees

int motorRingBuffer[] = {0, 0, 0};
int motorRingBufferIndex = 0;

bool noteChanging = false;

bool isStart = true;

#define NUM_SONGS 2
Song songs[NUM_SONGS] = {sevenNationArmySong, marryLambSong};
int currentSongIndex = 0;
int currentSongLength = songs[currentSongIndex].numNotes;
Note* currentNotes = songs[currentSongIndex].notes;

//Note testSong[2] =
//{
//  {0, 1000},
//  {1, 1000}
//};

int currentNote = 0;

int targetVal = 0;

unsigned long lastTime = 0;
unsigned long pauseTime = 0;

bool isPlaying = true;
bool buttonPressed = false;
long lastButtonPress = 0;
const long debounceDelay = 200;
long lastButtonUp = 0;
long buttonPressStart = 0;
long buttonPressEnd = 0;
bool isLongPress = false;
unsigned long longPressTime = 1000;

bool inSongSelect = false;

void setup()
{
  // Declare pins as Outputs
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(strumStepPin, OUTPUT);
  pinMode(strumDirPin, OUTPUT);

  for (int i = 0; i < NUM_SONGS; i++) {
    pinMode(firstLEDPin + i, OUTPUT);
  }


  digitalWrite(strumDirPin, HIGH);
  pinMode(motorPotPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);



  Serial.begin(9600);

  cli(); //stop interrupts
  //setup timer1
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 50; // sets the compare register, basically set as low as it can go and have the motors still spin
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS01 and CS00 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei(); // allow interrupts
}

void strum() {
  strumStepsLeft += strumSteps;
}

void turnOnAllLEDs() {
  for (int i = 0; i < NUM_SONGS; i++) {
    digitalWrite(firstLEDPin + i, HIGH);
  }
}

void turnOffAllLEDs() {
  for (int i = 0; i < NUM_SONGS; i++) {
    digitalWrite(firstLEDPin + i, LOW);
  }
}

void turnOnSpecificLED(int num) {
  turnOffAllLEDs();

  digitalWrite(firstLEDPin + num, HIGH);
}

void loop()
{

  int buttonState = digitalRead(buttonPin);
  //button is LOW when depressed, HIGH when untouched
  long buttonTime = millis();

  // button down
  if ( buttonState == LOW) {
    lastButtonPress = buttonTime;
    if (!buttonPressed && (buttonTime - lastButtonUp) > debounceDelay) {
      buttonPressStart = buttonTime;
      buttonPressed = true;
    }

    if (buttonTime - buttonPressStart > longPressTime && !isLongPress) {
      isLongPress = true;
      Serial.println("long press");
      // do something to indicate long press
      Serial.println("turning on all LED's");
      turnOnAllLEDs();
    }
  }
  // button up
  if ( buttonState == HIGH) {
    // we actually can be sure a button has stayed up long enough to count
    if (buttonPressed && buttonTime - lastButtonPress > debounceDelay) {

      buttonPressed = false;
      long buttonHoldTime = buttonTime - buttonPressStart;
      Serial.println(buttonHoldTime);
      
      if (isLongPress) {
        // a long press is how you get into and out of song select
        if (inSongSelect) {
          currentSongLength = songs[currentSongIndex].numNotes;
          currentNotes = songs[currentSongIndex].notes;
          currentNote = 0;
          isStart = true;

          Serial.println("turning on specific led");
          turnOnSpecificLED(currentSongIndex);
          
          isPlaying = true;
          Serial.println("exiting song select");
          inSongSelect = false;
        }
        else {
          Serial.println("turning on specific led");
          turnOnSpecificLED(currentSongIndex);
          
          inSongSelect = true;
          isPlaying = false;
          pauseTime = millis();
        }
        isLongPress = false;
      }
      else {
        if (inSongSelect) {
          // move to next song
          currentSongIndex = currentSongIndex == NUM_SONGS - 1 ? 0 : currentSongIndex + 1;
          // change LED
          Serial.println("turning on specific led");
          turnOnSpecificLED(currentSongIndex);
        }
        else {
          if (isPlaying) {
            isPlaying = false;
            pauseTime = millis();
          }
          else {
            isPlaying = true;
            lastTime += millis() - pauseTime;
          }
        }
      }
    }

  }

  if (isPlaying) {
    if (isStart) {
      strum();
      lastTime = millis();
      isStart = false;
    }

    if ((millis() - lastTime) > currentNotes[currentNote].length_ms) {
      lastTime = millis();

      // move to next note
      currentNote++;
      if (currentNote == currentSongLength) {
        currentNote = 0;
      }
      int note = currentNotes[currentNote].midiNote;
      if (note != -1) {
        targetVal = midiNoteToTargetVal(note);
        noteChanging = true;
        strum();
      }
    }

    //wait until we're done our current move to make the next move
    while (stepsLeft > 0) {
      __asm__("nop\n\t");
    }

    int motorVal;
    if (noteChanging) {
      motorVal = analogRead(motorPotPin);
    }
    else {
      motorRingBuffer[motorRingBufferIndex] = analogRead(motorPotPin);
      motorRingBufferIndex = motorRingBufferIndex == 2 ? 0 : motorRingBufferIndex + 1;
      float averageMotorVal = 0;
      for (int i = 0; i < 3; i++) {
        averageMotorVal +=  motorRingBuffer[i];
      }
      motorVal = averageMotorVal / 3.0;
    }

    //  Serial.print("motorVal: ");

    //  int potVal = analogRead(motorPotPin);
    //  Serial.println(potVal);

    int diff = abs(targetVal - motorVal);
    if (diff < 2 && noteChanging) {
      noteChanging = false;
      motorRingBuffer[0] = motorVal;
      motorRingBuffer[1] = motorVal;
      motorRingBuffer[2] = motorVal;
    }


    if (diff > 20) {
      //higher  motorVal implies more in the negative step direction
      if (targetVal > motorVal) {
        digitalWrite(dirPin, LOW);
        float proportionToMove = (float)(diff) / (float)(potMax - potMin);
        float angleToMove = proportionToMove * potROM;
        float stepsToMove = angleToMove * 200 / 360;
        stepsLeft = 0.8 * stepsToMove;
      }
      else {
        digitalWrite(dirPin, HIGH);
        float proportionToMove = (float)(diff) / (float)(potMax - potMin);
        float angleToMove = proportionToMove * potROM;
        float stepsToMove = angleToMove * 200 / 360;
        stepsLeft = 0.8 *  stepsToMove;
      }
    }
  }

}

ISR(TIMER1_COMPA_vect) {
  if (stepsLeft > 0) {
    digitalWrite(stepPin, HIGH);
    digitalWrite(stepPin, LOW);
    stepsLeft--;
  }
  if (strumStepsLeft > 0) {
    digitalWrite(strumStepPin, HIGH);
    digitalWrite(strumStepPin, LOW);
    strumStepsLeft--;
  }
}
