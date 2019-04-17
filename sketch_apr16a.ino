#include <SPI.h>
#include <WiFi101.h>

char ssid[] = "ssid";        // your network SSID (name)
char pass[] = "pass";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);

double tick = 0;
boolean A, B;

void AP_setup()
{
  //Configure pins for Adafruit ATWINC1500 Feather
  WiFi.setPins(8,7,4,2);
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Access Point Web Server");

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // by default the local IP address of will be 192.168.1.1
  // you can override it with the following:
  // WiFi.config(IPAddress(10, 0, 0, 1));

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  // wait 10 seconds for connection:
  delay(10000);

  // start the web server on port 80
  server.begin();

  // you're connected now, so print out the status
  printWiFiStatus();
}

void AP_loop()
{
   // compare the previous status to the current status
  if (status != WiFi.status()) 
  {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) 
    {
      byte remoteMac[6];

      // a device has connected to the AP
      Serial.print("Device connected to AP, MAC address: ");
      WiFi.APClientMacAddress(remoteMac);
      printMacAddress(remoteMac);
    }
    else 
    {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }
   
}

void IR_setup()
{
  pinMode(5, INPUT);//IR sensor pins
  pinMode(6, INPUT);
  attachInterrupt(digitalPinToInterrupt(5),IR1,RISING);//interrupt pins for IR sensor
  attachInterrupt(digitalPinToInterrupt(6),IR2,RISING); 
}

void IR1()
{
  A = digitalRead(5); // read signal from sensor
  if (A == HIGH) tick ++;  // count timer tick
}

void IR2()
{
  B = digitalRead(6); // read signal from sensor
  if (B == HIGH) tick ++;  // count timer tick
}

void motor_setup()
{
  pinMode(11,OUTPUT);// direction of motor
  pinMode(12,OUTPUT);
}

int setSpeed(float s)     //set motor speed
{
  if (s > 1)  s = 1.0;
  if (s < 0) s = 0.0;
  int out = (int)(s*255.0);
  return out;
}


void struct_setup()
{
  
}

void IMU_setup()
{
  
}

void check_IMU()
{
  
}

void check_UDP()
{
  
}

void setup() {
  Serial.begin(9600); 
    AP_setup();
    struct_setup();
    IR_setup();
    motor_setup();
    IMU_setup();
}


void loop() {
 AP_loop();
 check_IMU();
 
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
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);

}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
