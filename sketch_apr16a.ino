#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <math.h>

int status = WL_IDLE_STATUS;

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "id";        // your network SSID (name)
char pass[] = "pass";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

unsigned int localPort = 2390;      // local port to listen on

  struct __attribute__((packed))  Robot
  {
      float velo;
      float theta;
      int mode;
  }*Robot_t;

char packetBuffer[255]; //buffer to hold incoming packet
char  ReplyBuffer[] = "acknowledged";       // a string to send back

WiFiUDP Udp;

double tick = 0;
boolean A, B;  
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
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
  attachInterrupt(digitalPinToInterrupt(14),IR1,RISING);//interrupt pins for IR sensor
  attachInterrupt(digitalPinToInterrupt(15),IR2,RISING); 
}

void IR1()
{
  A = digitalRead(14); // read signal from sensor
  if (A == HIGH) tick ++;  // count timer tick
}

void IR2()
{
  B = digitalRead(15); // read signal from sensor
  if (B == HIGH) tick ++;  // count timer tick
}

int setSpeed(float s)
{
  if (s > 1)  s = 1.0;
  if (s < 0) s = 0.0; 
  int out = (int)(s * 255.0);
  return out;
}

void setVelocity(float v)
{
  float s = v;
  if (s < 0)  s = -s;
  int sp = setSpeed(s);
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
  { analogWrite(5, 0); // set Bpin to low
    analogWrite(6, sp);
    analogWrite(10, 0); // set Bpin to low
    analogWrite(9, sp);
  } // wrie analog value (PWM) to Apin. value − the duty cycle: between 0 (always off) and 255 (always on).
  else
  { analogWrite(6, 0);
    analogWrite(5, sp);
    analogWrite(9, 0);
    analogWrite(10, sp);
  }
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
    Serial.println(Robot_t->mode);

    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
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
void loop()
{
  while (1)
  {
    udp();
    setVelocity(Robot_t->velo); 
  }

}
