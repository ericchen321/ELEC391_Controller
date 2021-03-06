/* included necessary libraries */
#include <PID_v1_modified_v1.h>

/* Define pins */
#define MOTOR0_ENA 9
#define MOTOR0_IN1 6
#define MOTOR0_IN2 7
#define ENCODER0_PINA  22
#define ENCODER0_PINB  24

/* Define other macros */

/* Define variables for the dual channel encoder reading algorithm */
volatile signed long encoder0Pos = 0;


/* Define desired angles */
double desiredYaw;
double desiredPitch;


/* Define variables for serialStuff */
signed long currentTime = 0;
signed long lastTime = 0;


/* Define control variables for the PID and initialze all PID related stuff */
double actualPitch, pwmOutput;
double Kp=3, Ki=0.0028, Kd=12.8;
PID myPID(&actualPitch, &pwmOutput, &desiredPitch, Kp, Ki, Kd, DIRECT);



void setup() {
  /* set pin modes */
  pinMode(ENCODER0_PINA, INPUT);
  pinMode(ENCODER0_PINB, INPUT);
  pinMode(MOTOR0_ENA, OUTPUT);
  pinMode(MOTOR0_IN1, OUTPUT);
  pinMode(MOTOR0_IN2, OUTPUT);


  /* initialize software interrupts */
  attachInterrupt(digitalPinToInterrupt(ENCODER0_PINA), doEncoder0_A, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER0_PINB), doEncoder0_B, CHANGE);


  /* Set initial rotation direction and pwm */
  digitalWrite(MOTOR0_IN1, HIGH);
  digitalWrite(MOTOR0_IN2, LOW);
  analogWrite(MOTOR0_ENA, 90);

  /* initialize serial communication */
  Serial.begin(115200);


  /* initialize actual and desired angle for the PID algorithm */
  encoder();
  desiredPitch = 15;


  /* turn the PID on */
  myPID.SetMode(AUTOMATIC);

  delay(5000);
}



void loop() {
  encoder();
  myPID.Compute();
  motor();
  serialStuff() ;
}



/* calculates the relative angle from the encoder's position count */
void encoder () {
  actualPitch = encoder0Pos * 0.9;
}



/* set the rotation and pwm of the motor */
void motor () {
  if ( pwmOutput < 0 ) {
    digitalWrite(MOTOR0_IN1, LOW);
    digitalWrite(MOTOR0_IN2, HIGH);
    analogWrite(MOTOR0_ENA, (-1 * pwmOutput));
  }
  else {
    digitalWrite(MOTOR0_IN1, HIGH);
    digitalWrite(MOTOR0_IN2, LOW);
    analogWrite(MOTOR0_ENA, pwmOutput);
  }

}


void serialStuff () {
  currentTime = millis();
  if (currentTime >= lastTime + 4) {  
    Serial.print(currentTime);
    Serial.print(",");
    Serial.print(actualPitch);
    Serial.print(",");
    Serial.print(pwmOutput);
    Serial.println("");
    lastTime = currentTime;
  }
}


void doEncoder0_A() {
  // look for a low-to-high on channel A
  if (digitalRead(ENCODER0_PINA) == HIGH) {

    // check channel B to see which way encoder is turning
    if (digitalRead(ENCODER0_PINB) == LOW) {
      encoder0Pos = encoder0Pos + 1;         // CW
    }
    else {
      encoder0Pos = encoder0Pos - 1;         // CCW
    }
  }

  else   // must be a high-to-low edge on channel A
  {
    // check channel B to see which way encoder is turning
    if (digitalRead(ENCODER0_PINB) == HIGH) {
      encoder0Pos = encoder0Pos + 1;          // CW
    }
    else {
      encoder0Pos = encoder0Pos - 1;          // CCW
    }
  }
  // use for debugging - remember to comment out
}

void doEncoder0_B() {
  // look for a low-to-high on channel B
  if (digitalRead(ENCODER0_PINB) == HIGH) {

    // check channel A to see which way encoder is turning
    if (digitalRead(ENCODER0_PINA) == HIGH) {
      encoder0Pos = encoder0Pos + 1;         // CW
    }
    else {
      encoder0Pos = encoder0Pos - 1;         // CCW
    }
  }

  // Look for a high-to-low on channel B

  else {
    // check channel B to see which way encoder is turning
    if (digitalRead(ENCODER0_PINA) == LOW) {
      encoder0Pos = encoder0Pos + 1;          // CW
    }
    else {
      encoder0Pos = encoder0Pos - 1;          // CCW
    }
  }
}
