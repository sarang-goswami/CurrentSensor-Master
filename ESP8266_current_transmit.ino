#include <SoftwareSerial.h>
#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1;

#define SSID "YOUR_ROUTER_NAME"
#define PASS "YOUR_ROUTER_PASSWORD"
#define DST_IP "IP_OF_SERVER_WHERE_YOU_WANT_TO_SEND_DATA"
SoftwareSerial wifi(2, 3); // RX, TX -Update as per your pins

void setup()
{
  Serial.begin(9600);

  emon1.current(1, 45);             // Current: input pin, calibration.

  boolean connected = false;

  wifi.begin(9600);

  while (!Serial);
  while (!wifi);

  Serial.println("ESP8266 Demo on UNO");

  delay(1000);  //wait for the ESP to turn On

  wifi.println("AT+RST");

  delay(2000); //wait for the ESP to reset

  Debug("AT+RST");

  wifi.println("AT+UART=9600,8,1,0,0");  //Set the baud rate between the Arduino and ESP

  Debug("AT+UART=9600,8,1,0,0");

  //Try to connect to the wifi, try 5 times , but code is incomplete


  for (int i = 0; i < 5; i++)
  {
    if (connectWiFi())
    {
      connected = true;
      break;
    }
  }
  delay(5000);

  //print the ip addr

  wifi.println("AT+CIFSR");

  Debug("AT+CIFSR");

  //set the single connection mode
  wifi.println("AT+CIPMUX=0");

  Debug("AT+CIPMUX");
}
void loop()
{
  String req = "";

  wifi.println("AT+CIPSTATUS");
  Debug("AT+CIPSTATUS");

  delay(500);

  establishTcpLink(DST_IP, 80);  //open the connection on port 80

  delay(500);  // Once again, let the link stabilize.
  double Irms = emon1.calcIrms(1480);  // Calculate Irms only

  //You need a page which can accept the request
 // so create a page and host it on a server (update the server's address in DST_IP at the top)

 //My sample application has a page which accepts a CurrentVal as a parameter in a post request
  
  req = "/CurrentSensor/AddDataToSession.aspx?CurrentVal=" ; 
  req = req + Irms;
  requestPage(req);

  Serial.flush();
  wifi.flush();
  delay(1000);
}

boolean requestPage(String page)
{
  String http_req = "GET " + page + "\r\n\r\n";
  String cmd = "AT+CIPSEND=";
  cmd = cmd + http_req.length() ; // Tell the ESP8266 how long the coming HTTP request is.
  wifi.println(cmd);
  Debug(cmd);
  // Send the raw HTTP request.
  delay(250);
  wifi.println(http_req);

  Debug(http_req);

//To-do figure out how to handle errors

  return true;
}

boolean connectWiFi()
{
  wifi.println("AT+CWMODE=1");

  Debug("AT+CWMODE=1");

  //The following commented code can be used if you want to directly send commands to the ESP8266 from the serial monitor
  // while (wifi.available())
  //  {
  //    Serial.write(wifi.read());
  //    char c = wifi.read();
  //    //dbgSerial.write(c);
  //    Serial.write(c);
  //    //if(c=='\r') dbgSerial.print('\n');
  //    if (c == '\r') Serial.print('\n');
  //  }
  // return true; //don't want below code to be executed

  String cmd = "AT+CWJAP=\"";
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";

  wifi.println(cmd);

  Debug("AT+CWJAP");
  Serial.println(cmd);
  delay(2000);

  //To-do figure out if connected or not, useful for retrying.
  if (wifi.find("OK"))
  {
    //  Serial.println("OK, Connected to WiFi.");
    //return true;
  } else
  {
    //dbgSerial.println("Can not connect to the WiFi.");
    // Serial.println("Can not connect to the WiFi.");
    //return false;
  }
  return true;
}

void Debug(String Message)
{
  Serial.println("Response for " + Message);
  while (wifi.available() > 0)
  {
    char c = wifi.read();

    Serial.print(c);
  }
  Serial.println("--------------------");
}

boolean establishTcpLink(String ip, int port)
{
  String cmd = "AT+CIPSTART=\"TCP\",\"" + ip + "\"," + port;
  wifi.println(cmd);
  Debug(cmd);

  //TO-DO figure out how to determine if connection is not established

  return true;
}
