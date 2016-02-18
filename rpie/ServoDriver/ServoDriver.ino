/***************************************************
  This is an example for the Adafruit CC3000 Wifi Breakout & Shield

  Designed specifically to work with the Adafruit WiFi products:
  ----> https://www.adafruit.com/products/1469

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

/*
This example does a test of the TCP client capability:
 * Initialization
 * Optional: SSID scan
 * AP connection
 * DHCP printout
 * DNS lookup
 * Optional: Ping
 * Connect to website and print out webpage contents
 * Disconnect
SmartConfig is still beta and kind of works but is not fully vetted!
It might not work on all networks!
*/
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"
#include <Servo.h>
#include "ServoControl.h"

// Servo pin
#define servoPin 6
// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                         SPI_CLOCK_DIVIDER); // you can change this clock speed

#define WLAN_SSID       "ATT-WIFI-4857"           // cannot be longer than 32 characters!
#define WLAN_PASS       "84889775"

// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define IDLE_TIMEOUT_MS  3000      // Amount of time to wait (in milliseconds) with no data 
#define REPORTING_INTERVAL_MS  3000      // Interval between data reporting events (in milliseconds)
// received before closing the connection.  If you know the server
// you're accessing is quick to respond, you can reduce this value.

// Domain name for the open bms server
#define OBMSSERVER "openbms.elomar.me"


/**************************************************************************/
/*!
    @brief  Sets up the HW and the CC3000 module (called automatically
            on startup)
*/
/**************************************************************************/

uint32_t ip;
Servo servo1;   // Creating servo1 object
int level = 175;  // This value is fan's lowest speed

void setup(void)
{
  servo1.attach(servoPin);   
  servo1.write(level);
  
  Serial.begin(115200);
  Serial.println(F("Hello, CC3000!\n"));

  Serial.print("Free RAM: "); Serial.println(getFreeRam(), DEC);

  /* Initialise the module */
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while (1);
  }

  // Optional SSID scan
  // listSSIDResults();

  Serial.print(F("\nAttempting to connect to ")); Serial.println(WLAN_SSID);
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while (1);
  }

  Serial.println(F("Connected!"));

  /* Wait for DHCP to complete */
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }

  /* Display the IP address DNS, Gateway, etc. */
  while (! displayConnectionDetails()) {
    delay(1000);
  }

  ip = 0;
  // Try looking up the website's IP address
  Serial.print(OBMSSERVER); Serial.print(F(" -> "));
//  while (ip == 0) {
//    if (! cc3000.getHostByName(OBMSSERVER, &ip)) {
//      Serial.println(F("Couldn't resolve!"));
//    }
//    delay(500);
//  }
//  ip = cc3000.IP2U32(192, 168, 1, 51);
  ip = cc3000.IP2U32(192.168.1.68);
// ip = cc3000.IP2U32(10,0,0,137);
 // cc3000.printIPdotsRev(ip);
  Serial.println("Attempting to connect...");
  // Optional: Do a ping test on the website
  /*
  Serial.print(F("\n\rPinging ")); cc3000.printIPdotsRev(ip); Serial.print("...");
  replies = cc3000.ping(ip, 5);
  Serial.print(replies); Serial.println(F(" replies"));
  */

  /* Try connecting to the website.
     Note: HTTP/1.1 protocol is used to keep the server from closing the connection before all data is read.
  */
  Adafruit_CC3000_Client www = cc3000.connectTCP(ip, 6001);

  Serial.println(F("-------------------------------------"));

  /* Read data until either the connection is closed, or the idle timeout is reached. */
  unsigned long lastRead = millis();
  unsigned long lastReport = millis();
  String readBuffer = "";
  int i = 0;
  while (www.connected()) {
//    if(millis() - lastReport > REPORTING_INTERVAL_MS){
//      Serial.println(F("Sending data..."));
//        www.fastrprint(F("power=50W"));
//      lastReport = millis();   
//    }
//    
    if (www.available()) {
      char command = www.read();
      readBuffer += command;
      i = 1;
    }
    else{
      if(i == 1){
        Serial.println(readBuffer);
        if(readBuffer.indexOf('=') >= 0){
          String command = readBuffer.substring(0, readBuffer.indexOf('='));
          String value = readBuffer.substring(readBuffer.indexOf('=') + 1);
          if(command == "speed"){
              ServoControl(value.toInt());
              Serial.println("Fan Speed is " + value + ".");
          }
        }
        if(readBuffer == "action"){
          Serial.println("COMMAND!");
        }
        readBuffer = "";
        i = 0;
      }
    }
  }
  Serial.println(F("Connection failed"));
  www.close();
  Serial.println(F("-------------------------------------"));

  /* You need to make sure to clean up after yourself or the CC3000 can freak out */
  /* the next time your try to connect ... */
  Serial.println(F("\n\nDisconnecting"));
  cc3000.disconnect();

}

void loop(void)
{
  delay(1000);
}

/**************************************************************************/
/*!
    @brief  Begins an SSID scan and prints out all the visible networks
*/
/**************************************************************************/

void listSSIDResults(void)
{
  uint32_t index;
  uint8_t valid, rssi, sec;
  char ssidname[33];

  if (!cc3000.startSSIDscan(&index)) {
    Serial.println(F("SSID scan failed!"));
    return;
  }

  Serial.print(F("Networks found: ")); Serial.println(index);
  Serial.println(F("================================================"));

  while (index) {
    index--;

    valid = cc3000.getNextSSID(&rssi, &sec, ssidname);

    Serial.print(F("SSID Name    : ")); Serial.print(ssidname);
    Serial.println();
    Serial.print(F("RSSI         : "));
    Serial.println(rssi);
    Serial.print(F("Security Mode: "));
    Serial.println(sec);
    Serial.println();
  }
  Serial.println(F("================================================"));

  cc3000.stopSSIDscan();
}

/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;

  if (!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}
