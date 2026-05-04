int speedPin = 11;
int startButton = 5;

bool running = false;
enum Direction {Forward = 0, Backward = 1, Left = 2, Right = 3};
int movementIndex;

//Constants to figure out
float degPerMeter = 360;
float metersPerGridSquare = .5;
float metersForEntry = .2;

//Competition Variables
Direction path[5] = {Forward, Left, Forward, Right, Backward};
int pathIndex = -1;

struct motor
{
  public: int forwardPort;
  public: int backwardPort;

  public: int encoderA;
  public: int encoderB;
  public: volatile long currentPosition = 0;
  public: long targetPosition = 0;

  private: int isOnTarget;

  public: static int encodedMotors;
  public: static int landedMotors;

  public: void MoveToEncoder()
  {
    if(currentPosition > targetPosition)
    {
      isOnTarget = false;
      digitalWrite(forwardPort, 1);
      digitalWrite(backwardPort, 0);
    }
    else if(currentPosition < targetPosition)
    {
      isOnTarget = false;
      digitalWrite(forwardPort, 0);
      digitalWrite(backwardPort, 1);
    }
    else
    {
      if(!isOnTarget)
      {
        landedMotors++;
        isOnTarget = true;
      }
      digitalWrite(forwardPort, 0);
      digitalWrite(backwardPort, 0);
    }
  };

  public: void ChangeTargetSpot(long change)
  {
    targetPosition += change;
  }


  public: motor(int firstPort, int secondPort)
  {
    forwardPort = firstPort;
    backwardPort = secondPort;
    pinMode(forwardPort, OUTPUT);
    pinMode(backwardPort, OUTPUT);
  }

  public: motor(int firstMotorPort, int secondMotorPort, int firstEncPort, int secondEncPort)
  {
    forwardPort = firstMotorPort;
    backwardPort = secondMotorPort;
    pinMode(forwardPort, OUTPUT);
    pinMode(backwardPort, OUTPUT);

    encoderA = firstEncPort;
    encoderB = secondEncPort;
    pinMode(encoderA, INPUT);
    pinMode(encoderB, INPUT);
    encodedMotors++;
  }

  public: void encoderISR()
  {
    // if(digitalRead(forwardPort) != 0 && digitalRead(backwardPort) != 0)
    // {
      if (digitalRead(encoderA) == digitalRead(encoderB))
        currentPosition++;  // Forward rotation
      else
        currentPosition--;  // Backward rotation
    // }
  }
};

int motor::encodedMotors = 0;
int motor::landedMotors = 0;

motor frontLeft = motor(6, 7, A0, A1);
motor frontRight = motor(9, 8, A2, A3);
motor backLeft = motor(10, 11, A4, A5);
motor backRight = motor(13, 12, 2, 3);

void flISR()
{
  frontLeft.encoderISR();
}

void frISR()
{
  frontRight.encoderISR();
}

void blISR()
{
  backLeft.encoderISR();
}

void brISR()
{
  backRight.encoderISR();
}

void changeDirections(int shift)
{
  switch(shift)
  {
    case 0: //forwards
      frontLeft.ChangeTargetSpot(1 * degPerMeter * metersPerGridSquare);
      frontRight.ChangeTargetSpot(1 * degPerMeter * metersPerGridSquare);
      backLeft.ChangeTargetSpot(1 * degPerMeter * metersPerGridSquare);
      backRight.ChangeTargetSpot(1 * degPerMeter * metersPerGridSquare);
      break;
    case 1: //backwards
      frontLeft.ChangeTargetSpot(-1 * degPerMeter * metersPerGridSquare);
      frontRight.ChangeTargetSpot(-1 * degPerMeter * metersPerGridSquare);
      backLeft.ChangeTargetSpot(-1 * degPerMeter * metersPerGridSquare);
      backRight.ChangeTargetSpot(-1 * degPerMeter * metersPerGridSquare);
      break;
    case 2: //left
      frontLeft.ChangeTargetSpot(-1 * degPerMeter * metersPerGridSquare);
      frontRight.ChangeTargetSpot(1 * degPerMeter * metersPerGridSquare);
      backLeft.ChangeTargetSpot(1 * degPerMeter * metersPerGridSquare);
      backRight.ChangeTargetSpot(-1 * degPerMeter * metersPerGridSquare);
      break;
    case 3: //right
      frontLeft.ChangeTargetSpot(1 * degPerMeter * metersPerGridSquare);
      frontRight.ChangeTargetSpot(-1 * degPerMeter * metersPerGridSquare);
      backLeft.ChangeTargetSpot(1 * degPerMeter * metersPerGridSquare);
      backRight.ChangeTargetSpot(-1 * degPerMeter * metersPerGridSquare);
      break;
  }
  motor::landedMotors = 0;
}

void ProgressPath()
{
  if(motor::landedMotors == motor::encodedMotors)
  {
    pathIndex += 1;

    if(pathIndex < sizeof(path))
      changeDirections(path[pathIndex]);
    else
    {
      running = false;
      pathIndex = -1;
    }
  }
}

void startRun()
{
  frontLeft.ChangeTargetSpot(metersForEntry * metersPerGridSquare);
  frontRight.ChangeTargetSpot(metersForEntry * metersPerGridSquare);
  backLeft.ChangeTargetSpot(metersForEntry * metersPerGridSquare);
  backRight.ChangeTargetSpot(metersForEntry * metersPerGridSquare);
}



void setup() {
  Serial.begin(9600); //Opens console
  pinMode(speedPin, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(startButton, INPUT);
  digitalWrite(startButton, HIGH);
  digitalWrite(4, HIGH);

  attachInterrupt(digitalPinToInterrupt(frontLeft.encoderA), flISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(frontRight.encoderA), frISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(backLeft.encoderA), blISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(backRight.encoderA), brISR, CHANGE);
  digitalWrite(11, 1);
}

void loop() {
  if(running && digitalRead(startButton))
  {
    //Move the motors
    // frontLeft.MoveToEncoder();
    // frontRight.MoveToEncoder();
    // backLeft.MoveToEncoder();
    // backRight.MoveToEncoder();

    digitalWrite(6, HIGH);
    digitalWrite(9, HIGH);

    delay(5000);

    digitalWrite(6, LOW);
    digitalWrite(9, LOW);
    
    // digitalWrite(10, LOW);
    // digitalWrite(13, LOW);

    ProgressPath();
  }
  else
  {
    // digitalWrite(10, LOW);
    // digitalWrite(13, LOW);
    if(!digitalRead(startButton))
      running = true;
  }

  // Serial.println(running && digitalRead(startButton));
}
