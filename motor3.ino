boolean A, B;
double Kp = 0.2;
double tick = 0;
double angle = 0;
double d_angle = 0;
byte state, nextstate;
float error;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);  
  pinMode(11,OUTPUT);// motor pins
  pinMode(12,OUTPUT);
  pinMode(5, INPUT);//IR sensor pins
  pinMode(6, INPUT);
  attachInterrupt(digitalPinToInterrupt(5),IR1,CHANGE);//interrupt pins for IR sensor
  attachInterrupt(digitalPinToInterrupt(6),IR2,CHANGE); 
}
void IR1()
{
  A = digitalRead(5); // read signal from sensor
  B = digitalRead(6);
  if ((A==HIGH)&&(B==HIGH)) state = 1; // state machine to check the direction
  if ((A==HIGH)&&(B==LOW)) state = 2;
  if ((A==LOW)&&(B==LOW)) state = 3;
  if((A==LOW)&&(B==HIGH)) state = 4;
  switch (state)
  {
    case 1:
    {
      if (nextstate == 2) tick++;  // count timer tick
      if (nextstate == 4) tick--;
      break;
    }
    case 2:
    {
      if (nextstate == 1) tick--;
      if (nextstate == 3) tick++;
      break;
    }
    case 3:
    {
      if (nextstate == 2) tick--;
      if (nextstate == 4) tick++;
      break;
    }
    default:
    {
      if (nextstate == 1) tick++;
      if (nextstate == 3) tick--;
    }
  }
  nextstate = state;

}

void IR2()
{
  A = digitalRead(5); // read signal from sensor
  B = digitalRead(6);
  if ((A==HIGH)&&(B==HIGH)) state = 1;
  if ((A==HIGH)&&(B==LOW)) state = 2;
  if ((A==LOW)&&(B==LOW)) state = 3;
  if((A==LOW)&&(B==HIGH)) state = 4;
  switch (state)
  {
    case 1:
    {
      if (nextstate == 2) tick++; // count timer tick
      if (nextstate== 4) tick--;
      break;
    }
    case 2:
    {
      if (nextstate == 1) tick--;
      if (nextstate == 3) tick++;
      break;
    }
    case 3:
    {
      if (nextstate == 2) tick--;
      if (nextstate == 4) tick++;
      break;
    }
    default:
    {
      if (nextstate == 1) tick++;
      if (nextstate == 3) tick--;
    }
  }
  nextstate = state;
  
}

int setSpeed(float s)     //set motor speed
{
  if (s > 1)  s = 1.0;
  if (s < 0) s = 0.0;
  int out = (int)(s*255.0);
  return out;
}


void loop() {
  // put your main code here, to run repeatedly:
  while(1)
  {
    angle = 0.6 * tick;                     //calculate angle 
    if (Serial.available())
    { 
        d_angle = Serial.parseFloat();      //desired angle from serial input
    }

    error = d_angle - angle;                // calculate error
    setSpeed(Kp*error);                     // P control
    if (angle < d_angle)
    {
      digitalWrite(11,LOW); // Forward
      digitalWrite(12,HIGH);
    }
    else
    {
      digitalWrite(11,HIGH); // Reverse
      digitalWrite(12,LOW);
    }
  }
  
}
