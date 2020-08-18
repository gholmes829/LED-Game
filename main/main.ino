/* Author: Grant Holmes
 * Date: Early Spring 2020
 * Description: Three LEDs and three buttons are connected to Arduino. Lights flash n times while user tries to memorize sequence.
 *              User then has to press buttons corresponding to LEDs in correct order. EG if LEDs flash red->green->blue, user presses red, green, then blue button.
 *              If they remembered the sequence and inputted it back in correctly, they advance to the next level with increased difficulty.
 *              Has three levels of difficulty that user selects on device with buttons.
 *              Also has secret "insane" mode with random and sporadic delays to increase difficulty by holding all three buttons down at startup.
 *              Sequences are randomly generated each time.
 */

#define NUMELEMENTS(x) (sizeof(x) / sizeof(x[0]))

int LED_pinG,LED_pinR, LED_pinY, LED_pinW;  // leds
int push_button_pinG, push_button_pinR, push_button_pinY, push_button_pinStart;  // buttons
int push_button_valueG, push_button_valueR, push_button_valueY, push_button_valueStart;  // button states

int Delay, Halt;
int counter = 0;

unsigned long timer, insane_timer;

int red_state = 0, yellow_state = 0, green_state = 0;

int red = 3, green = 1, yellow = 2;
int insane_1 = 0, insane_2 = 0, insane_3 = 0; //"insane" variables relate to secret hardcore mode

bool button_state_1, button_state_2, button_state_3;
bool checkButtons;
bool checkFinal = true;
int mode = 0; //1 = easy, 2 = medium, 3 = hard

// sequence of guesses
int guess_1[] = {0, 0, 0};
int guess_2[] = {0, 0, 0, 0};
int guess_3[] = {0, 0, 0, 0, 0};
int guess_4[] = {0, 0, 0, 0, 0, 0};
int guess_5[] = {0, 0, 0, 0, 0, 0, 0};
int guess_6[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
int guess_7[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int guess_8[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

bool seq_1 = false, seq_2 = true, seq_3 = true, seq_4 = true, seq_5 = true, seq_6 = true, seq_7 = true, seq_8 = true;

//initializing sequences to be displayed
int sequence_1[] = {0, 0, 0}; 
int sequence_2[] = {0, 0, 0, 0};
int sequence_3[] = {0, 0, 0, 0, 0};
int sequence_4[] = {0, 0, 0, 0, 0, 0};
int sequence_5[] = {0, 0, 0, 0, 0, 0, 0};
int sequence_6[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
int sequence_7[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int sequence_8[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

struct SEQUENCES {
  int*sequences;
  int*guesses;
  bool seq;
  uint16_t numElements;
};

//sequence to display, guesses, bool, length of sequence
SEQUENCES seqMatrix[] =
{
  {sequence_1, guess_1, seq_1, NUMELEMENTS(sequence_1)},
  {sequence_2, guess_2, seq_2, NUMELEMENTS(sequence_2)},
  {sequence_3, guess_3, seq_3, NUMELEMENTS(sequence_3)},
  {sequence_4, guess_4, seq_4, NUMELEMENTS(sequence_4)},
  {sequence_5, guess_5, seq_5, NUMELEMENTS(sequence_5)},
  {sequence_6, guess_6, seq_6, NUMELEMENTS(sequence_6)},
  {sequence_7, guess_7, seq_7, NUMELEMENTS(sequence_7)},
  {sequence_8, guess_8, seq_8, NUMELEMENTS(sequence_8)}
};

void setup() {
randomSeed(analogRead(0));
Serial.begin(9600);

LED_pinG=3;
pinMode(LED_pinG,OUTPUT);
digitalWrite(LED_pinG,LOW);

LED_pinR=5;
pinMode(LED_pinR,OUTPUT);
digitalWrite(LED_pinR,LOW);

LED_pinY=4;
pinMode(LED_pinY,OUTPUT);
digitalWrite(LED_pinY,LOW);

LED_pinW=6;
pinMode(LED_pinW,OUTPUT);
digitalWrite(LED_pinW,LOW);

push_button_pinG=7;
pinMode(push_button_pinG,INPUT);

push_button_pinR=9;
pinMode(push_button_pinR,INPUT);

push_button_pinY=8;
pinMode(push_button_pinY,INPUT);

push_button_pinStart=10;
pinMode(push_button_pinStart,INPUT);

Delay=375;
Halt=100; }

void turn_on_red(int Delay, int Halt){
  //flashes red LED
  digitalWrite(LED_pinR,HIGH);
  delay(Delay);
  digitalWrite(LED_pinR,LOW);
  delay(100);
}
void turn_on_green(int Delay, int Halt){
  //flashes green LED
  digitalWrite(LED_pinG,HIGH);
  delay(Delay);
  digitalWrite(LED_pinG,LOW);
  delay(Halt);
}
void turn_on_yellow(int Delay, int Halt){
  //flashes yellow LED
  digitalWrite(LED_pinY,HIGH);
  delay(Delay);
  digitalWrite(LED_pinY,LOW);
  delay(Halt);
}

void turn_on_white(int Delay) {
  //turns on white LED for Delay time
  digitalWrite(LED_pinW,HIGH);
  delay(375);
  digitalWrite(LED_pinW,LOW);
  delay(100);
}

void waitDisplay(int wait) {
  //displays cool wating display before game
  if ((millis()-timer)/wait < 1) {
    digitalWrite(LED_pinY,LOW);
    digitalWrite(LED_pinG,HIGH);
  }
  else if ((millis()-timer)/wait >= 1 && (millis()-timer)/wait < 2) {
    digitalWrite(LED_pinY,HIGH);
    digitalWrite(LED_pinG,LOW);
  }
  else if ((millis()-timer)/wait >= 2 && (millis()-timer)/wait < 3) {
    digitalWrite(LED_pinR,HIGH);
    digitalWrite(LED_pinY,LOW);
  }
  else if ((millis()-timer)/wait >= 3 && (millis()-timer)/wait < 4) {
    digitalWrite(LED_pinY,HIGH);
    digitalWrite(LED_pinR,LOW);
  }
  else if ((millis()-timer)/wait >= 4) {
    timer = millis();
  } }
    
bool play_sequence(int index, int difficulty) {
  Delay = 375;
  Halt = 100;
  
  if (difficulty == 3) {
    Delay = 750; }
  else if (difficulty == 4) {
    Delay = 500;
  }
  else if (difficulty == 5) {
    Delay = 250;
  }
  
  checkButtons = true;
  
  for (int i = 0; i < seqMatrix[index].numElements; i++) {
    seqMatrix[index].guesses[i] = 0; }

  for (int i = 0; i< seqMatrix[index].numElements; i++) {
    
    if (difficulty == 100) {
      Delay = int(random(50, 250));
      Halt = int(random(250, 1000)); }
      
    if (seqMatrix[index].sequences[i] == 1) {
      turn_on_green(Delay, Halt); }
      
    else if (seqMatrix[index].sequences[i] == 2) {
      turn_on_yellow(Delay, Halt); }
      
    else if (seqMatrix[index].sequences[i] == 3) {
      turn_on_red(Delay, Halt); } }

  while (checkButtons == true) {
    button_state_1 = digitalRead(push_button_pinG);
    button_state_2 = digitalRead(push_button_pinY);
    button_state_3 = digitalRead(push_button_pinR);
    
    for (int i = 0; i < seqMatrix[index].numElements; i++) {
    Serial.print(seqMatrix[index].guesses[i]); }
    Serial.println("");

    for (int i = 0; i < seqMatrix[index].numElements; i++) {
    Serial.print(seqMatrix[index].sequences[i]); }
    Serial.println("");

    if (button_state_1 != green_state) {
      green_state = 0; }

    if (button_state_2 != yellow_state) {
      yellow_state = 0; }
      
    if (button_state_3 != red_state) {
      red_state = 0; }
    
    if (button_state_1==HIGH && green_state == 0) {
      green_state = 1;
      
      for (int i=0; i< seqMatrix[index].numElements; i++) {
        if (seqMatrix[index].guesses[i] == 0) {
          seqMatrix[index].guesses[i] = green;
          break; } } }
          
    else if (button_state_2==HIGH && yellow_state == 0) {
      yellow_state = 1;
      for (int i=0; i< seqMatrix[index].numElements; i++) {
        if (seqMatrix[index].guesses[i] == 0) {
          seqMatrix[index].guesses[i] = yellow;
          break; } } }
          
    else if (button_state_3==HIGH && red_state == 0) {
      red_state = 1;
      for (int i=0; i< seqMatrix[index].numElements; i++) {
        if (seqMatrix[index].guesses[i] == 0) {
          seqMatrix[index].guesses[i] = red;
          break; } } }
          
  checkButtons = false;
  
  for (int i = 0; i < seqMatrix[index].numElements; i++) {
  
    if (seqMatrix[index].guesses[i] == 0) {
      checkButtons = true; } } }  //end while loop
      
  checkFinal = true;
  
  for (int i = 0; i < seqMatrix[index].numElements; i++) {
 
    if (seqMatrix[index].guesses[i] != seqMatrix[index].sequences[i]) {
      checkFinal = false; } }
  
  if (checkFinal == true) {
    Serial.println("Correct!!!");
    Serial.print("Guess is: ");
    
    for (int i = 0; i < seqMatrix[index].numElements; i++) {
      Serial.print(String(seqMatrix[index].guesses[i])); }
      
    Serial.println("");
    Serial.print("Correct is: ");
    
    for (int i = 0; i < seqMatrix[index].numElements; i++) {
      Serial.print(String(seqMatrix[index].sequences[i])); }
      Serial.println("");
      
    delay(1000);
    
    if (index+1<NUMELEMENTS(seqMatrix)) {
      seqMatrix[index+1].seq = false; }
      
    return true; }
     
  else {
    Serial.println("Wrong!!!");
    Serial.println("Guess is: ");
    
    for (int i = 0; i < seqMatrix[index].numElements; i++) {
      Serial.print(String(seqMatrix[index].guesses[i])); }
      
    Serial.println(" ");
    Serial.println("Correct is: ");
    
    for (int i = 0; i < seqMatrix[index].numElements; i++) {
      Serial.print(String(seqMatrix[index].sequences[i])); }
      Serial.println(" ");
      
    digitalWrite(LED_pinR,HIGH);
    digitalWrite(LED_pinG,HIGH);
    digitalWrite(LED_pinY,HIGH);
    delay(500);
    
    digitalWrite(LED_pinR,LOW);
    digitalWrite(LED_pinG,LOW);
    digitalWrite(LED_pinY,LOW);
    delay(1000);
    
    for (int i = 0; i < NUMELEMENTS(seqMatrix); i++) {
      seqMatrix[i].seq = true; }
      
    seqMatrix[0].seq = false;
    
  return false; } }

void show_points() {
  for (int i = 0; i<counter; i++) {
    turn_on_white(Delay);
  } }

void reseed() {
  for (int i = 0; i < NUMELEMENTS(seqMatrix); i++) {
    for (int n = 0; n < seqMatrix[i].numElements; n++) {
      seqMatrix[i].sequences[n] = int(random(1, 4)); } } }

void loop() {
  timer = millis();
  insane_timer = millis();
  insane_1 = 0;
  insane_2 = 0;
  insane_3 = 0;
  mode = 0;
  
  while (true) {
    waitDisplay(500);
    button_state_1 = digitalRead(push_button_pinG);
    button_state_2 = digitalRead(push_button_pinY);
    button_state_3 = digitalRead(push_button_pinR);
    
    if (button_state_1 == HIGH) {
      mode = 3;
      insane_1 = 1; }
      
    else if (button_state_2 == HIGH) {
      mode = 4;
      insane_2 = 1; }
      
    else if (button_state_3 == HIGH) {
      mode = 5;
      insane_3 = 1; }
      
    if (insane_1 == 1 && insane_2 == 1 && insane_3 == 1) {
      mode = 100;
      Serial.println("Initializing INSANE MODE...");
    }
    
    if (mode != 0 && (millis()-insane_timer)/3000 >= 1) {
      break; } }
      
  digitalWrite(mode, HIGH);
  
  for (int i = 3; i<6; i++) {
    if (i != mode) {
      digitalWrite(i,LOW); } }
  
  delay(1000);
  digitalWrite(mode, LOW);
  delay(1000);
  
  reseed();

  //for each pre-defined sequence
  for (int i = 0; i<NUMELEMENTS(seqMatrix); i++) {
    if (seqMatrix[i].seq == false) {
      if (i>0) {
   
        counter++; }

      //run sequence
      seqMatrix[i].seq = play_sequence(i, mode); }

      //checks to see if accurate
      if (seqMatrix[i].seq == false or (i == (NUMELEMENTS(seqMatrix)-1) && seqMatrix[(NUMELEMENTS(seqMatrix)-1)].seq == true)) {
        show_points();
        delay(1000);
        break; } }
        
  counter = 0; }
