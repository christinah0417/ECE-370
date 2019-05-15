#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <math.h>
#include <Wire.h>
#include <LSM303.h>
#include <BasicLinearAlgebra.h>

LSM303 compass;
char report[80];
int status = WL_IDLE_STATUS;

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "marcanton21";        // your network SSID (name)
char pass[] = "ca11201980";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

unsigned int localPort = 2390;      // local port to listen on

struct __attribute__((packed))  Robot
{
  float velo;
  float theta;
}Robot_t;

char packetBuffer[255]; //buffer to hold incoming packet
char  ReplyBuffer[] = "acknowledged";       // a string to send back

WiFiUDP Udp;
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
  Wire.begin();
  compass.init();
  compass.enableDefault();
  /*
    Calibration values; the default values of +/-32767 for each axis
    lead to an assumed magnetometer bias of 0. Use the Calibrate example
    program to determine appropriate values for your particular unit.
  */
  compass.m_min = (LSM303::vector<int16_t>) {
    -1890, -2410, -3572
  };
  compass.m_max = (LSM303::vector<int16_t>) {
    +2940, +2855, +2996
  };
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
    Udp.read((char*)(&Robot_t), sizeof(Robot_t));
    //Robot_t = (Robot*)packetBuffer;
    //if (len > 0) packetBuffer[len] = 0;
    Serial.println("Contents:");
     Serial.println(packetBuffer);  
     Serial.println(Robot_t.velo);
    Serial.println(Robot_t.theta);
    //Serial.println(Robot_t->mode);
    

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
  
  int mode = 0;
  while (1)
  {
    udp();
    compass.read();
     
    Serial.println(compass.a.z); 
    if(compass.a.z > -10000)
      Robot_t.velo = 0;
    setVelocity(Robot_t.velo);
    delay(100);
  }

}
