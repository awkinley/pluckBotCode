// Define pin connections & motor's steps per revolution
const int dirPin = 4;
const int stepPin = 5;
const int strumDirPin = 2;
const int strumStepPin = 3;
const int stepsPerRevolution = 200;

const int strumSteps = stepsPerRevolution / 3;
int stepsLeft = 0;
int strumStepsLeft = 0;

const int motorPotPin = A1;
const int guidePotPin = A2;

const int potMax = 883;
const int potMin = 0;
const int potROM = 270; // degrees

int motorRingBuffer[] = {0, 0, 0};
int motorRingBufferIndex = 0;

bool noteChanging = false;

#define NOTE_C 0;
#define NOTE_Csharp 1;
#define NOTE_D 2;
#define NOTE_Dsharp 3;
#define NOTE_E 4;
#define NOTE_F 5;
#define NOTE_Fsharp 6;
#define NOTE_G 7;
#define NOTE_Gsharp 8;
#define NOTE_A 9;
#define NOTE_Asharp 10;
#define NOTE_B 11;

typedef struct{
    int midiNote;
    unsigned int length_ms;
} Note;

Note marryLamb[30] = {{ 76, 948},
                    { 74, 948},
                    { 72, 948},
                    { 74, 948},
                    { 76, 948},
                    { 76, 948},
                    { 76, 948},
                    { -1, 1052},
                    { 74, 948},
                    { 74, 948},
                    { 74, 948},
                    { -1, 1052},
                    { 76, 948},
                    { 79, 948},
                    { 79, 948},
                    { -1, 1052},
                    { 76, 948},
                    { 74, 948},
                    { 72, 948},
                    { 74, 948},
                    { 76, 948},
                    { 76, 948},
                    { 76, 948},
                    { -1, 1052},
                    { 76, 948},
                    { 74, 948},
                    { 74, 948},
                    { 76, 948},
                    { 74, 948},
                    { 72, 948}};

int currentNote = 0;


typedef struct{
    char* name;
    Note* notes;
    uint16_t numNotes;
} Song;

int noteVals[] = {0, 80, 160, 240, 320, 410, 480, 560, 740, 820};

int targetVal = 0;
int lastTime = 0;

bool isHigh = true;
bool isClockwise = true;
void setup()
{
  // Declare pins as Outputs
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(strumStepPin, OUTPUT);
  pinMode(strumDirPin, OUTPUT);
  

  digitalWrite(strumStepPin, LOW);
  digitalWrite(strumDirPin, LOW);
  pinMode(motorPotPin, INPUT);
  pinMode(guidePotPin, INPUT);

  Serial.begin(9600);

  cli(); //stop interrupts
  //setup timer1
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 75; // = (16*10^6) / (2000 * 1024) - 1 (must be <256)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS01 and CS00 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei(); // allow interrupts
}

int midiNoteToTargetVal(int midiNote){
  switch(midiNote){
    case 72 :
      return noteVals[8];
    case 74 :
      return noteVals[6];
    case 76 :
      return noteVals[4];
    case 79 :
      return noteVals[1];
    default:
      return 0;
  }
}

void strum(){
  strumStepsLeft += strumSteps;
//  Serial.print("strumming, steps left: ");
//  Serial.print(stepsLeft);
//  Serial.print("\n");
}
void loop()
{
//    if(Serial.available() > 0){
//      int newNoteVal = Serial.parseInt();
//      if(newNoteVal == -1){
//        strum();
//      }
//      else{
////              Serial.print("new note val: ");
////      Serial.println(newNoteVal);
//      targetVal = noteVals[newNoteVal];
//      noteChanging = true;
////      Serial.print("targetVal: ");
////      Serial.println(targetVal);
//      }
//    }

  Serial.print(millis());
  Serial.print("\t");
  if((millis() - lastTime) > marryLamb[currentNote].length_ms){
    lastTime = millis();
    
    // move to next note
    currentNote++;
    if(currentNote == 30){
      currentNote = 0;
    }
    int note = marryLamb[currentNote].midiNote;
    if(note != -1){
      targetVal = midiNoteToTargetVal(note);
      noteChanging = true;
    }
  }
  while(stepsLeft > 0){
    __asm__("nop\n\t");
  }
//  Serial.println("we got here");
  
  int guideVal = targetVal; //analogRead(guidePotPin);
  int motorVal;
  if(noteChanging){
    motorVal = analogRead(motorPotPin);
  }
  else{
    motorRingBuffer[motorRingBufferIndex] = analogRead(motorPotPin);
    motorRingBufferIndex = motorRingBufferIndex == 2 ? 0 : motorRingBufferIndex + 1;
    float averageMotorVal = 0;
    for(int i = 0; i < 3; i++){
      averageMotorVal +=  motorRingBuffer[i];
    }
    motorVal = averageMotorVal / 3.0;
  }
  

//  Serial.print("motorVal: ");
  Serial.println(motorVal);
  //higher  motorVal implies more in the negative step direction

  int diff = abs(guideVal - motorVal);
  if(diff < 2 && noteChanging){
    strum();
    noteChanging = false;
    motorRingBuffer[0] = motorVal;
    motorRingBuffer[1] = motorVal;
    motorRingBuffer[2] = motorVal;
  }
//  Serial.print("diff: ");
//  Serial.println(diff);
  if(diff > 20){
    if(guideVal > motorVal){
      digitalWrite(dirPin, LOW);
      float proportionToMove = (float)(diff) / (float)(potMax - potMin);
//      Serial.print("proportionToMove: ");
//      Serial.println(proportionToMove);
      float angleToMove = proportionToMove * potROM;
//      Serial.print("angleToMove: ");
//      Serial.println(angleToMove);
      float stepsToMove = angleToMove * 200 / 360;
      stepsLeft = 0.8 * stepsToMove;
      //stepsLeft += 5;
    }
    else{
      digitalWrite(dirPin, HIGH);
      float proportionToMove = (float)(diff) / (float)(potMax - potMin);
//      Serial.print("proportionToMove: ");
//      Serial.println(proportionToMove);
      float angleToMove = proportionToMove * potROM;
//      Serial.print("angleToMove: ");
//      Serial.println(angleToMove);
      float stepsToMove = angleToMove * 200 / 360;
      stepsLeft = 0.8 *  stepsToMove;
//      stepsLeft += 5;
    }
  }

//  Serial.print("steps left: ");
//  Serial.print("\t");
//  Serial.println(stepsLeft);
}

ISR(TIMER1_COMPA_vect){
  if(stepsLeft > 0){
    digitalWrite(stepPin, HIGH);
    digitalWrite(stepPin, LOW);
    stepsLeft--;
  }  
  if(strumStepsLeft > 0){
    digitalWrite(strumStepPin, HIGH);
    digitalWrite(strumStepPin, LOW);
    strumStepsLeft--;
  }
}
