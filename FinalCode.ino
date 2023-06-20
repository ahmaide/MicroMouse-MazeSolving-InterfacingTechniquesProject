 #include <NewPing.h>


const int motor_power = 150;      // 0-255 150
const float motor_offset = 1;       // Diff. when driving straight

const int wheel_d = 42;           // Wheel diameter (mm)
const float wheel_c = PI * wheel_d; // Wheel circumference (mm)
//const int counts_per_rev = 390;   // (4 pairs N-S) * (48:1 gearbox) * (2 falling/rising edges) = 384 --edit
const int counts_per_rev = 390;   // (4 pairs N-S) * (48:1 gearbox) * (2 falling/rising edges) = 384 --edit
int dist_ultra = 10;

/*working good for @120*/
const int drive_distance = 17.15;   // cm
const float dist_right = 7; //cm
const float dist_left = 5.5; //cm
const float dist_fullTurn = 13.286; //cm .. driveRight2(18, 120); is good for full turn

const int pwma_pin = 6;
const int ain1_pin = 4;
const int ain2_pin = 5;

const int pwmb_pin = 9;
const int bin1_pin = 7;
const int bin2_pin = 8;


const int trig_right = A5;
const int echo_right = A4;

const int trig_left = A1;
const int echo_left = A0;

const int trig_front = A2;
const int echo_front = A3;

const int ir_right = 10;
const int ir_left = 13;

int encoderPin_l = 3; //Encoder Output 'A' must connected with intreput pin of arduino.
int encoderPin2_l = 2; //Encoder Output 'B' must connected with intreput pin of arduino.

int encoderPin_r = 12; //Encoder Output 'A' must connected with intreput pin of arduino.
int encoderPin2_r = 11; //Encoder Output 'B' must connected with intreput pin of arduino

volatile int lastEncoded = 0; // Here updated value of encoder store.
volatile long enc_l = 0; // Raw encoder value

volatile int lastEncoded2 = 0; // Here updated value of encoder store.
volatile long enc_r = 0; // Raw encoder value
int right_wall, left_wall;
NewPing sonar[3] = { // array of sonar sensor objects
  NewPing(trig_right, echo_right, 350),
  NewPing(trig_left, echo_left, 350),
  NewPing(trig_front, echo_front, 350)
};
float distance[3] = {0.0, 0.0, 0.0};

void setup() {
  PCICR |= B00000100; //Bit2 = 1 -> "PCIE2" enabled (PCINT16 to PCINT23) 
  PCICR |= B00000001; //Bit2 = 1 -> "PCIE2" enabled (PCINT16 to PCINT23) 
  PCMSK0 |= B00011000; //Bit4, Bit5 = 1 -> "PCIE2" enabled (PCINT16 to PCINT23) 
  PCMSK2 |= B00001100;  

// driveLeft(dist_left, 200);
//   driveStraight(drive_distance,200);
//   delay(500);
//   driveRight(dist_right, 200);//right 6.5
  initialize();
  Serial.begin(9600);
  delay(2000);


}

void loop() {
  forwardChecking();
  delay(500);
    /*Serial.print(enc_l);
    Serial.print("\t");
    Serial.println(enc_r);*/
    /*forwardChecking();
    delay(500);*/

}


void driveStraight(float dist, int power) {

  unsigned long num_ticks_l;
  unsigned long num_ticks_r;

  // Set initial motor power
  int power_l = power;
  //int power_l = (int)(1.08 * motor_power);
  int power_r = (int)(1.105 * power);
  //int power_l = motor_power;

  // Used to determine which way to turn to adjust
  unsigned long diff_l;
  unsigned long diff_r;

  // Reset encoder counts
  enc_l = 0;
  enc_r = 0;

  // Remember previous encoder counts
  unsigned long enc_l_prev = enc_l;
  unsigned long enc_r_prev = enc_r;

  // Calculate target number of ticks
  float num_rev = (dist * 10) / wheel_c;  // Convert to mm
  unsigned long target_count = num_rev * counts_per_rev;
  
  // Debug
  /*.print("Driving for ");
  Serial.print(dist);
  Serial.print(" cm (");
  Serial.print(target_count);
  Serial.print(" ticks) at ");
  Serial.print(power);
  Serial.println(" motor power");*/

  // Drive until one of the encoders reaches desired count
  while ((enc_l < target_count) && (enc_r < target_count)) {

    // Sample number of encoder ticks
    num_ticks_l = (unsigned long)(enc_l*1);
    num_ticks_r = enc_r;

    // Print out current number of ticks
    Serial.print(num_ticks_l);
    Serial.print("\t");
    Serial.print(num_ticks_r);
    
    Serial.print("\t");
    Serial.print(power_l);
    Serial.print("\t");
    Serial.println(power_r);

    //updateSonar();
    //updateIR();

    //
    //checkWallHit(power_l, power_r);

    // Drive
    drive(power_l, power_r);

    // Number of ticks counted since last time
    diff_l = num_ticks_l - enc_l_prev;
    diff_r = num_ticks_r - enc_r_prev;

    // Store current tick counter for next time
    enc_l_prev = num_ticks_l;
    enc_r_prev = num_ticks_r;
    // If left is faster, slow it down and speed up right
    if ( diff_l > diff_r ) {
      
      power_l -= motor_offset;
      power_r += motor_offset;
    }

    // If right is faster, slow it down and speed up left
    if ( diff_l < diff_r ) {
      power_l += motor_offset;
      power_r -= motor_offset;
    }

    // Brief pause to let motors respond
    //delay(10);
  }

  /*target_count = 0;
  enc_l_prev = enc_l;
  enc_r_prev = enc_r;
  while((power_r > target_count) || (power_l > target_count)){
    power_r -= 15;//can be edited - decreasing rate
    power_l -= 15;
    if(power_r < 0)
      power_r = 0;
    if(power_l < 0)
      power_l = 0;

    drive(power_l, power_r);
    diff_l = enc_l - enc_l_prev;
    diff_r = enc_r - enc_r_prev;
     if ( diff_l > diff_r ) {
      power_l -= 2;//
      power_r += 3;
    }
    if ( diff_l < diff_r ) {
      power_l += 3;
      power_r -= 2;
    }
    enc_l_prev = enc_l;
    enc_r_prev = enc_r;
    //delayMicroseconds(200);
  }*/
  brake();
  delay(500);
  checkWallHit();
  delay(250);
}

void drive( int power_b,int power_a) {
  // Constrain power to between -255 and 255
  
  int power = (int)(1.5*motor_power);
  power_a = constrain(power_a, -power, power);
  power_b = constrain(power_b, -power, power);

  // Left motor direction
  if ( power_a < 0 ) {
    digitalWrite(ain1_pin, LOW);
    digitalWrite(ain2_pin, HIGH);
  } else {
    digitalWrite(ain1_pin, HIGH);
    digitalWrite(ain2_pin, LOW);
  }

  // Right motor direction
  if ( power_b < 0 ) {
    digitalWrite(bin1_pin, LOW);
    digitalWrite(bin2_pin, HIGH);
  } else {
    digitalWrite(bin1_pin, HIGH);
    digitalWrite(bin2_pin, LOW);
  }

  // Set speed
  analogWrite(pwma_pin, abs(power_a));
  analogWrite(pwmb_pin, abs(power_b));
}
void brake(){
  digitalWrite(ain1_pin, HIGH);
  digitalWrite(ain2_pin, HIGH);
  analogWrite(pwma_pin, 255);
  digitalWrite(bin1_pin, HIGH);
  digitalWrite(bin2_pin, HIGH);
  analogWrite(pwmb_pin, 255);
}

void updateSonar(){

  for (int i = 0; i < 3; i++) {
    distance[i] = (double)sonar[i].ping() / 2.0 * 0.0343;
  }
}

bool wallRight(){
  return right_wall == LOW || distance[0] < 16;
}

bool wallLeft(){
  return left_wall == LOW || distance[1] < 16;
}

bool wallFront(){
  return distance[2] < 16;
}

void updateIR(){
  left_wall = digitalRead(ir_left);
  right_wall = digitalRead(ir_right);
}

void fullTurn(){
  driveRight(dist_fullTurn, 140);
}

void checkWallHit(){
  unsigned long diff;
  float dist = 0.0;
  int offset = 10;
  if ( abs(enc_l) > (abs(enc_r) + offset)) {
    diff = abs(enc_l) - abs(enc_r);
    dist = (diff*wheel_c)/(330.0*10.0);
    driveRight(dist/3, 120);//100  dist/2.0
  }
  // If right is faster, slow it down and speed up left
  else if ((offset + abs(enc_l)) < abs(enc_r) ) {
    diff = abs(enc_r) - abs(enc_l);
    dist = (diff*wheel_c)/(390.0*10.0);
    driveLeft(dist/3, 120);//100   dist/2.0
  }
}

void checkTurn(){
  unsigned long diff;
  float dist = 0.0;
  int offset = 10;
  if ( abs(enc_l) > (abs(enc_r) + offset)) {
    diff = abs(enc_l) - abs(enc_r);
    dist = (diff*wheel_c)/(330.0*10.0);
    driveLeft(dist/3, 100);//100  dist/2.0
  }
  else if ((offset + abs(enc_l)) < abs(enc_r) ) {
    diff = abs(enc_r) - abs(enc_l);
    dist = (diff*wheel_c)/(390.0*10.0);
    driveRight(dist/3, 100);//100   dist/2.0
  }
}

void forwardChecking(){
  updateSonar();
  updateIR();
  /*
  1. All closed. --
  2. All open --
  3. Right, front --
  4. Left, front --
  5. Right, left --
  6. Right
  7. Left
  8. Front
  */
  if (wallRight() && wallLeft() && wallFront()){ //1
      fullTurn();
      driveStraight(drive_distance, 140);

  }else if (!wallRight() && !wallLeft() && !wallFront()){//2
      //need to modify according to the algorithm
      // driveRight(dist_right, 120);
      //driveLeft(dist_left, 120);
      driveStraight(drive_distance, 140);

  }else if (!wallRight() && wallLeft() && !wallFront()){//3
  //depends on the algorithm
      //driveRight(dist_right, 120);
      driveStraight(drive_distance, 140);

  }else if(wallRight() && !wallLeft() && !wallFront()){//4
  //depends on the algorithm
      //driveLeft(dist_left, 120);
      driveStraight(drive_distance, 140);

  }else if (!wallRight() && !wallLeft() && wallFront()){//5
  //depends on the algorithm
      driveLeft(dist_right, 200);
      //driveLeft(dist_left, 120);
  }else if (!wallRight() && wallLeft() && wallFront()){//6
      driveLeft(dist_right, 200);

  }else if(wallRight() && !wallLeft() && wallFront()){//7
      driveRight(dist_left, 200);

  }else if(wallRight() && wallLeft() && !wallFront()){//8
      driveStraight(drive_distance, 140);
  }
}

void initialize(){
  pinMode(ir_right, INPUT);
  pinMode(ir_left, INPUT);

  pinMode(trig_right, OUTPUT); // Sets the trigPin as an Output
  pinMode(echo_right, INPUT); // Sets the echoPin as an Input

  pinMode(trig_left, OUTPUT); // Sets the trigPin as an Output
  pinMode(echo_left, INPUT); // Sets the echoPin as an Input

  pinMode(trig_front, OUTPUT); // Sets the trigPin as an Output
  pinMode(echo_front, INPUT); // Sets the echoPin as an Input

  pinMode(ain1_pin, OUTPUT); 
  pinMode(ain2_pin, OUTPUT);
  pinMode(pwma_pin, OUTPUT);

  pinMode(bin1_pin, OUTPUT); 
  pinMode(bin2_pin, OUTPUT);
  pinMode(pwmb_pin, OUTPUT);
  Serial.begin(9600); //initialize serial comunication
 
  pinMode(encoderPin_l, INPUT_PULLUP); 
  pinMode(encoderPin2_l, INPUT_PULLUP);
  pinMode(encoderPin_r, INPUT_PULLUP); 
  pinMode(encoderPin2_r, INPUT_PULLUP);
 
  digitalWrite(encoderPin_l, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2_l, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin_r, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2_r, HIGH); //turn pullup resistor on
}

ISR (PCINT2_vect) {
  int MSB = digitalRead(encoderPin_l); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2_l); //LSB = least significant bit
 
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value
 
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) enc_l --;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) enc_l ++;
 
  lastEncoded = encoded; //store this value for next time
}

ISR (PCINT0_vect) {
  int MSB = digitalRead(encoderPin2_r ); //MSB = most significant bit
  int LSB = digitalRead(encoderPin_r); //LSB = least significant bit
 
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded2 << 2) | encoded; //adding it to the previous encoded value
 
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) enc_r --;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) enc_r ++;
 
  lastEncoded2 = encoded; //store this value for next time
}

void driveLeft(float dist, int power) {

  int power_r = power;
  int power_l = (int)(1.15*power);

  // Used to determine which way to turn to adjust
  unsigned long diff_l;
  unsigned long diff_r;

  // Reset encoder counts
  enc_l = 0;
  enc_r = 0;

  // Remember previous encoder counts
  unsigned long enc_l_prev = enc_l;
  unsigned long enc_r_prev = enc_r;

  // Calculate target number of ticks
  float num_rev = (dist * 10) / wheel_c;  // Convert to mm
  unsigned long target_count = num_rev * 390;

  // Drive until one of the encoders reaches desired count
  while ( (abs(enc_l) < target_count) && (abs(enc_r) < target_count) ) {
    drive(power_r,-1*power_l);
  }
  brake();
  delay(500);
  checkTurn();
  delay(250);
}

void driveRight(float dist, int power) {

  unsigned long num_ticks_l;
  unsigned long num_ticks_r;

  // Set initial motor power
  int power_l = power;
  int power_r = (int)(1.0*power);

  // Used to determine which way to turn to adjust
  unsigned long diff_l;
  unsigned long diff_r;

  // Reset encoder counts
  enc_l = 0;
  enc_r = 0;

  // Remember previous encoder counts
  unsigned long enc_l_prev = enc_l;
  unsigned long enc_r_prev = enc_r;

  // Calculate target number of ticks
  float num_rev = (dist * 10) / wheel_c;  // Convert to mm
  unsigned long target_count = num_rev * 330;//counts_per_rev;
  
  // Debug
  Serial.print("Driving for ");
  Serial.print(dist);
  Serial.print(" cm (");
  Serial.print(target_count);
  Serial.print(" ticks) at ");
  Serial.print(power);
  Serial.println(" motor power");

  // Drive until one of the encoders reaches desired count

  while ( (abs(enc_l) < target_count) && (abs(enc_r) < target_count) ) {
    drive(-1*power_r,power_l);
  }
  brake();
  delay(500);
  checkTurn();
  delay(250);
}