// SEB MEGAWORLD Machine Code Lucky Chinatown Mall

// IR sensor pins
const int irPins[] = { 29, 28, 8, 9, 10, 11, 12, 25, 24, 23 };
const int numSensors = sizeof(irPins) / sizeof(irPins[0]);

// Motor pins
const int motorPin1 = 2;
const int motorPin2 = 4;
const int enablePin = 5;

// State
bool motorTriggered = false;
unsigned long lastDetectionTime = 0;
const unsigned long timeoutDuration = 20000;  // 20 seconds

// LASER
const int receiverPin = 3;
bool waitingForLaserReturn = false;
bool laserRestored = false;
int scanCount = 0;  // Tracks number of finished scanning sections

// UI DATA
char input = '0';
bool isLogin = false;
// TEST DATA
bool testrun = false;

void setup() {
  // IR pins as input
  for (int i = 0; i < numSensors; i++) {
    pinMode(irPins[i], INPUT);
  }

  // Motor pins as output
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(enablePin, OUTPUT);
  analogWrite(enablePin, 250);

  // LASER
  pinMode(receiverPin, INPUT);

  // Start Serial for debugging (optional)
  Serial.begin(9600);
  Serial.println("SETUP STARTED");
}

void runConveyor() {
  bool detected = false;

  // Check if any IR sensor is triggered (reads LOW)
  for (int i = 0; i < numSensors; i++) {
    if (digitalRead(irPins[i]) == LOW) {
      detected = true;
      break;
    }
  }

  // If detection occurs, trigger motor and reset timer
  if (detected) {
    if (!motorTriggered) {
      Serial.println("Motor activated");
    }
    motorTriggered = true;
    lastDetectionTime = millis();
  }

  // If motor is on, check for timeout
  // if (motorTriggered) {

  //   unsigned long currentTime = millis();
  //   if (currentTime - lastDetectionTime >= timeoutDuration) {
  //     motorTriggered = false;
  //     Serial.println("Motor stopped due to inactivity");
  //   }
  // }

  // Motor control
  if (motorTriggered) {
    // Serial.println("Motor TEST");
    run_motor();
    // Serial.println("Motor TESTING");
  } else {
    stop_motor();
    // Serial.println("Motor FAIL");
  }
  // Check if motor was triggered and receiver lost laser
  if (motorTriggered && !waitingForLaserReturn) {
    delay(100);
    if (digitalRead(receiverPin) == HIGH) {
      waitingForLaserReturn = true;
      Serial.println("Laser interrupted, waiting for it to return...");
    }
  }

  // If receiver regains laser after interruption, mark scanning finished
  if (waitingForLaserReturn && digitalRead(receiverPin) == LOW) {
    scanCount++;
    Serial.print("Finished Scanning Section #: ");
    Serial.println(scanCount);
    waitingForLaserReturn = false;
    delay(500); 
  }
}



//----Function to run the motor----//
void run_motor() {
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
}
//----Function to stop the motor----//
void stop_motor() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
}



void checkInput() {
  if (Serial.available()) {
    input = Serial.read();  // Read the serial input
    if (input == '3' && !isLogin) {
      Serial.println("User is Logged In");
      isLogin = true;
    }
    if (input == '2' && !isLogin) {
      testrun = true;
      Serial.println("Motor Condition: RUNNING");
      Serial.println(scanCount);
    }
    if (input == '0' && !isLogin) {
      testrun = false;
      Serial.println("Motor Condition: STOPPED");
      Serial.println(scanCount);
      // Serial.println("Stop motor");
    }
    if (input == '0' && isLogin) {
      stop_motor();
      motorTriggered = false;
      Serial.println("Motor Condition: STOPPED");
      Serial.println(scanCount);
      // Serial.println("Stop motor");
    }
    if (input == '4' && isLogin) {
      stop_motor();
      motorTriggered = false;
      scanCount = 0;
      Serial.println("User is Logged Out");
      Serial.println(scanCount);
      isLogin = false;
    }
  }
}


void loop() {
  checkInput();  // Check and process the serial input

  if (isLogin) {
    runConveyor();
    testrun = false;
  }
  if (testrun && !isLogin) {
      digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  } 
  if (!testrun && !isLogin) {
    digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  }

  delay(500);  // Optional delay to reduce CPU usage
}