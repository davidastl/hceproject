//all Pins on the Arduino defined as variables for better programming
#include <Arduino.h>
#define enable 8
#define xDir 5
#define xStep 2

#define yDir 6
#define yStep 3

#define aDir 13
#define aStep 12

double stepDelay = 300;
double currentDelay = 0;
int Stps = 0;
int serialData = 0;
char string [50];
bool moveHome = false;
bool direct = false;
int count = 0;
int i = 0;
int r = 0;
int data [10];
char delimiter[] = ",;";
String initial;
char *ptr;
String axis[10];
bool done = true;

void ledpulse(int pulselength, int pause, int iterations) {

  for (int i = 0; i < iterations; i++) {

    digitalWrite(A3, HIGH);
    delay(pulselength);
    digitalWrite(A3, LOW);
    delay(pause);
  }

}

void move(double x, double y, double z, double stepspeed) {
  digitalWrite(enable, LOW);
  double stepsx = x / 0.0096;
  double stepsy = y / 0.0096;
  long stepsz = (long) (z / 0.0007);

  if (x < 0) {
    digitalWrite(xDir, true);
    digitalWrite(yDir, true);
    stepsx = -stepsx;
  } else {
    digitalWrite(xDir, false);
    digitalWrite(yDir, false);
  }

  for (int i = 0; i < stepsx; i++) {
    digitalWrite(xStep, LOW);
    digitalWrite(yStep, LOW);
    digitalWrite(xStep, HIGH);
    digitalWrite(yStep, HIGH);
    delayMicroseconds(stepspeed);
  }

  if (y < 0) {
    digitalWrite(xDir, false);
    digitalWrite(yDir, true);
    stepsy = -stepsy;
  } else {
    digitalWrite(xDir, true);
    digitalWrite(yDir, false);
  }

  for (int i = 0; i < stepsy; i++) {
    digitalWrite(xStep, LOW);
    digitalWrite(yStep, LOW);
    digitalWrite(xStep, HIGH);
    digitalWrite(yStep, HIGH);
    delayMicroseconds(stepspeed);
  }

  if (z < 0) {
    digitalWrite(aDir, true);
    stepsz = -stepsz;
  } else {
    digitalWrite(aDir, false);
  }

  for (int i = 0; i < stepsz; i++) {
    digitalWrite(aStep, LOW);
    digitalWrite(aStep, HIGH);
    delayMicroseconds(500);
  }
  digitalWrite(enable, HIGH);
}

void setup() {

  Serial.begin(250000);        //setting Baudrate

  pinMode(xDir, OUTPUT);      //setting pins to output pins
  pinMode(xStep, OUTPUT);
  pinMode(yDir, OUTPUT);
  pinMode(yStep, OUTPUT);
  pinMode(aStep, OUTPUT);
  pinMode(enable, OUTPUT);
  pinMode(A3, OUTPUT);
  digitalWrite(A3, LOW);        //set LED to off when starting
  digitalWrite(enable, HIGH);

  pinMode(LED_BUILTIN, OUTPUT);

}

void MoveHome() {
  while (moveHome) {
    if (digitalRead(10) == HIGH) { // if not
      digitalWrite(enable, LOW);
      move(0, -0.01, 0, 200);
    }
    else  { // if touches
      digitalWrite(enable, HIGH);
      if (digitalRead(9) == HIGH) { // if not
        digitalWrite(enable, LOW);
        move(0.01, 0, 0, 200);
      }
      else  { // if touches
        digitalWrite(enable, HIGH);
        moveHome = false;
        break;
      }
    }
  }
}


void grid(double gridsizex, double gridsizey, double distancex, double distancey, double pulse) {

  move(-(distancex * (gridsizex - 1)) / 2, (distancey * (gridsizey - 1)) / 2, 0, 800);
  
  for (int i = 0; i < gridsizey; i++) {
    delay(100);
    ledpulse(pulse, 50, 1);
    delay(50);
    for (int i = 0; i < gridsizex - 1; i++) {
      move(distancex, 0, 0, 200);
      delay(100);
      ledpulse(pulse, 50, 1);
      delay(50);

    }
    move(-distancex * (gridsizex - 1), 0, 0, 200);
    if (i < gridsizey - 1) {
      move(0, -distancey, 0, 200);
    }

  }
  move((distancex * (gridsizex - 1)) / 2, (distancey * (gridsizey - 1)) / 2, 0, 800);

}

void grid2(double gridsizex, double gridsizey, double distancex, double distancey) {

  int direct = -1;

  move(-(distancex * (gridsizex - 1)) / 2, (distancey * (gridsizey - 1)) / 2, 0, 800);

  for (int i = 0; i < gridsizey; i++) {
    digitalWrite(enable, HIGH);
    delay(5);
    ledpulse(5, 10, 1);
    delay(5);
    digitalWrite(enable, HIGH);
    for (int i = 0; i < gridsizex - 1; i++) {
      move(distancex * direct*(-1), 0, 0, 200);
      delay(5);
      ledpulse(5, 5, 1);

    }
    direct = direct*(-1);
    
    if (i < gridsizey - 1) {
      move(0, -distancey, 0, 200);
    }

  }
  move(direct*(-1)*(distancex * (gridsizex - 1)) / 2, (distancey * (gridsizey - 1)) / 2, 0, 800);

}


void loop () {

  // go to the starting point

  MoveHome();
  int availableBytes = Serial.available();
  while (Serial.available() > 0) // Don't read unless
    // there you know there is data
  {
    string[i] = Serial.read();
    i++;
  }

  ptr = strtok(string, delimiter);
  initial = ptr;

  while (ptr != NULL) {
    ptr = strtok(NULL, delimiter);
    axis[r] = ptr;
    Serial.println(axis[r]);
    r++;
  }

  if (initial == "g" && done == true) {

    grid(axis[0].toDouble(), axis[1].toDouble(), axis[2].toDouble(), axis[3].toDouble(),axis[4].toDouble());

    delay(1);
    initial = "";
    done = false;
  }

  if (initial == "t" && done == true) {

    grid2(axis[0].toDouble(), axis[1].toDouble(), axis[2].toDouble(), axis[3].toDouble());

    delay(1);
    initial = "";
    done = false;
  }

  if (initial == "a" && done == true) {

    if (axis[3].toDouble() < 199) {
      axis[3] = "200";
    }


    move(axis[0].toDouble(), axis[1].toDouble(), axis[2].toDouble(), axis[3].toDouble());

    delay(1);
    initial = "";
    done = false;
  }
  if (initial == "h" && done == true) {

    moveHome = true;

    delay(1);
    initial = "";
    done = false;
  }
  if (initial == "s" && done == true) {

    ledpulse(axis[0].toDouble(), axis[1].toDouble(), axis[2].toDouble());

    delay(1);
    initial = "";
    done = false;
  }

  if (availableBytes == 0) {

    done = true;
    for (int i = 0; i < sizeof(string) - 1; i++) {
      string[i] = NULL;
      delay(1);
    }
    for (int i = 0; i < sizeof(axis) - 1; i++) {
      //      axis[i] = NULL;
      delay(1);
    }
    i = 0;
    r = 0;
  }
  //Serial.print(string);
}
