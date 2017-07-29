#include <SoftwareSerial.h>
#define SSID "YOUR_ROUTER_NAME"
#define PASS "YOUR_ROUTER_PASSWORD"
#define DST_IP "IP_ADDRESS_OF_SERVER_WHERE_YOU_WANT_TO_SEND_DATA"
SoftwareSerial wifi(2, 3); // RX, TX
void setup()
{
  Serial.begin(9600);
 
  wifi.begin(9600);
 

  while (!Serial);
  while (!wifi);

  Serial.println("ESP8266 on UNO");

  delay(1000);

  wifi.println("AT+RST");  //Reset the ESP8266

  delay(2000); //Wait for the reset to complete
  
  Debug("AT+RST"); //Keep track of what's going on
  
  wifi.println("AT+UART=9600,8,1,0,0"); //Define the baud rate and other parameters
  
  Debug("AT+UART=9600,8,1,0,0");
  
  //connect to the wifi
  boolean connected = false;
  
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


  wifi.println("AT+CIPSTATUS");
  Debug("AT+CIPSTATUS");

  delay(1000);
  establishTcpLink(DST_IP, 80);
    delay(2000);  // Once again, let the link stabilize.
    int r = 0;
    r = random(300);
    String req = "/CurrentSensor/AddDataToSession.aspx?CurrentVal=" ;
    req = req + r;
 requestPage(req);

Serial.flush();
wifi.flush();
  delay(1000);
}

boolean requestPage(String page)
{

 //---> this works when page is /teset.htm String http_req = "GET " + page + "\r\n\r\n";
 String http_req = "GET " + page + "\r\n\r\n";
  String cmd = "AT+CIPSEND=";
  cmd = cmd + http_req.length() ; // Tell the ESP8266 how long the coming HTTP request is.
  wifi.println(cmd);
  Debug(cmd);
  // Send the raw HTTP request.
  delay(250);
  wifi.println(http_req);

  Debug(http_req);

  return true;
}

boolean connectWiFi()
{
  wifi.println("AT+CWMODE=1");

  Debug("AT+CWMODE=1");

  String cmd = "AT+CWJAP=\"";
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";
  
  wifi.println(cmd);

  Debug("AT+CWJAP");
  Serial.println(cmd);
  delay(2000);

  if (wifi.find("OK"))
  {
  //TO-DO figure out how to check proper connection
  //return true;
  } else
  {    
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
  return true;
}
