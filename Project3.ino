
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <math.h>
#include <Wire.h>
#include <LSM303.h>
#include <BasicLinearAlgebra.h>

#define R       15.0f // Wheel rarius = 30/2 = 15 mm
#define L       90.0f // distance between center of each wheels 90 mm 
#define gratio  75.81f // gear ratio
#define ticks   2.0f  // 2 ticks/rev
using namespace BLA;

LSM303 compass;
char report[80];
int status = WL_IDLE_STATUS;
int reset = 0;
int direct = 0;
float head;
float Kp = 0.2;
float error;
float sp;
float x_global = 0.0f,
      y_global = 0.0f,
      phi_global = 0.0f;
float phi = (R/L)*(2.0f*PI)*(1.0f/gratio)*(1.0f/ticks);
unsigned long tick, tock;

Matrix<4, 4> I = {1.0, 0.0, 0.0, 0.0, // identity matrix 

                  0.0, 1.0, 0.0, 0.0,

                  0.0, 0.0, 1.0, 0.0,

                  0.0, 0.0, 0.0, 1.0},

                                          
             VeloR = {1.0, 0.0, 0.0, 0.0, // Right wheel

                      0.0, 1.0, 0.0, -L,

                      0.0, 0.0, 1.0, 0.0,

                      0.0, 0.0, 0.0, 1.0},


            VeloL = {1.0, 0.0, 0.0, 0.0, // Left wheel

                     0.0, 1.0, 0.0, L,

                     0.0, 0.0, 1.0, 0.0,

                     0.0, 0.0, 0.0, 1.0},

             ThetaR = {cos(phi), sin(phi), 0.0, 0.0,  // Right wheel

                       -sin(phi), cos(phi),  0.0, 0.0f,

                       0.0,         0.0,     1.0, 0.0,

                       0.0,         0.0,     0.0, 1.0},

                                              
             ThetaL = {cos(-phi), sin(-phi), 0.0, 0.0,   // Left wheel 

                      -sin(-phi), cos(-phi),  0.0, 0.0f,

                      0.0,        0.0,        1.0, 0.0,

                      0.0,        0.0,        0.0, 1.0};

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "marcanton21";        // your network SSID (name)
char pass[] = "ca11201980";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

unsigned int localPort = 1111;      // local port to listen on

  struct __attribute__((packed))  Robot //receive
  {
      float velo;
      float theta;
    //  int mode;
  }*Robot_t;

   struct __attribute__((packed)) Info //send
  {
      float xpos; //odo
      float ypos;
      float zpos;
      float ax;   //accelerator
      float ay;
      float az;
      float mx;   //magnetometer
      float my;
      float mz;
      float theta; //heading
     
  }*Info_t;
char packetBuffer[255]; //buffer to hold incoming packet
char  ReplyBuffer[] = "acknowledged";       // a string to send back


WiFiUDP Udp;

//double tick = 0;
boolean A, B;  
float a1,a2,a3,b1,b2,b3;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
void setup()
{
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  WiFi.setPins(8, 7, 4, 2);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWiFiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  Udp.begin(localPort);
 
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(14, INPUT_PULLUP);//IR sensor pins
  pinMode(15, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(14),IR1,RISING);//interrupt pins for IR sensor right wheel
  attachInterrupt(digitalPinToInterrupt(15),IR2,RISING); // Left wheel
  delay(5000);
  for (int i = 0; i < 80; i++)

  {
    IR1();

    IR2();

  }
  
   Wire.begin();
  compass.init();
  compass.enableDefault();
    /*
  Calibration values; the default values of +/-32767 for each axis
  lead to an assumed magnetometer bias of 0. Use the Calibrate example
  program to determine appropriate values for your particular unit.
  */
  compass.m_min = (LSM303::vector<int16_t>){-3338, -1862, -3383};
  compass.m_max = (LSM303::vector<int16_t>){+2604, +2781, +2987};
}

void IR1() //right
{
  I = I * ThetaR * VeloR; // update current global x and y positions

  x_global = I(0, 3); // update global x
  y_global = I(1, 3); // update global y

  phi_global += phi; // update global phi
}

void IR2()//left
{
 I = I * ThetaL * VeloL; // update current global x and y positions

  x_global = I(0, 3); // update global x
  y_global = I(1, 3); // update global y

  phi_global += phi; // update global phi
}


void setVelocity(float v)
{
  float s = abs(v);
  if (s>255) int sp = 255;
  else sp = s;
  bool a = LOW;
  bool b = LOW;
  if (v >= 0)
  { a = HIGH;
    b = LOW;
  }
  else
  { a = LOW;
    b = HIGH;
  }

  if (HIGH == a)
  { analogWrite(6, 0); // set Bpin to low
    analogWrite(5, sp);
    analogWrite(10, 0); // set Bpin to low
    analogWrite(9, sp);
  } // wrie analog value (PWM) to Apin. value − the duty cycle: between 0 (always off) and 255 (always on).
  else
  { analogWrite(5, 0);
    analogWrite(6, sp);
    analogWrite(9, 0);
    analogWrite(10, sp);
  }
}

void turn(float v)
{
  float s = abs(v);
  if (s < 2)  sp = 0;
  else sp = s * 25.5;
  if (v >= 0)
  { analogWrite(6, 0); // set Bpin to low
    analogWrite(5, sp);
    analogWrite(10,0); // set Bpin to low
    analogWrite(9, 0);
  } // wrie analog value (PWM) to Apin. value − the duty cycle: between 0 (always off) and 255 (always on).
  else
  { analogWrite(6, 0);
    analogWrite(5, 0);
    analogWrite(10, 0);
    analogWrite(9, sp);
  }
}

void comp()
{
   compass.read();
  
  /*
  When given no arguments, the heading() function returns the angular
  difference in the horizontal plane between a default vector and
  north, in degrees.
  
  The default vector is chosen by the library to point along the
  surface of the PCB, in the direction of the top of the text on the
  silkscreen. This is the +X axis on the Pololu LSM303D carrier and
  the -Y axis on the Pololu LSM303DLHC, LSM303DLM, and LSM303DLH
  carriers.
  
  To use a different vector as a reference, use the version of heading()
  that takes a vector argument; for example, use
  
    compass.heading((LSM303::vector<int>){0, 0, 1});
  
  to use the +Z axis as a reference.
  */
  Info_t->theta = compass.heading();

  Serial.println(head);
  //delay(10);
}
void imu()
{
  compass.read();
  Info_t->ax = compass.a.x*0.061/1000.0;
  Info_t->ay = compass.a.y*0.061/1000.0;
  Info_t->az = compass.a.z*0.061/1000.0;
  Info_t->mx = compass.m.x*0.160/1000.0;
  Info_t->my = compass.m.y*0.160/1000.0;
  Info_t->mz = compass.m.z*0.160/1000.0;
//  Serial.println(a1);
//  Serial.println(a2);
//  Serial.println(a3);
//  Serial.println(b1);
//  Serial.println(b2);
//  Serial.println(b3);
//  snprintf(report, sizeof(report), "A: %6d %6d %6d    M: %6d %6d %6d",
//  compass.a.x, compass.a.y, compass.a.z,
//  compass.m.x, compass.m.y, compass.m.z);
//  Serial.println(report);
  delay(100);
}

void udp()
{
   // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    Robot_t = (Robot*)packetBuffer;
   // if (len > 0) packetBuffer[len] = 0;
    Serial.println("Contents:");
    Serial.println(Robot_t->velo);
    Serial.println(Robot_t->theta);
   // Serial.println(Robot_t->mode);

    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
   // Udp.write((char*)(&Info_t));
    Udp.endPacket();
  }
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


void Rotate(float angle)
{
  comp();
  error = (angle - Info_t->theta);
  turn(Kp*error);
 
}
void loop()
{
  while (1)
  {
    
    udp();
    imu(); 
    setVelocity(Robot_t->velo);
    Rotate(Robot_t->theta);
  }
}
