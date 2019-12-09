#include <Arduino.h>
#include <WiFiUdp.h>
#include <WiFiNINA.h>
#include "wifi.h"
#include "config.h"

// 0) Kuba Signal: -52 dBm Encryption: 4
// MAC: 24:0A:C4:AC:D7:DC

//SSID: Kuba
//BSSID: 5A:35:E1:FF:FB:72
//signal strength (RSSI):-45
//Encryption Type:4


char ssid[] = "Kuba";     //  your network SSID (name)
char pass[] = "tajneheslo";  // your network password

int status = WL_IDLE_STATUS;
unsigned int localPort = 2390;      // local port to listen on
char packetBuffer[255];
WiFiUDP Udp;
int command_angle = 0;
long last_command_time = 0;
bool halted = true;

void wifi_init() {
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  Udp.begin(localPort);
}


void wifi_run() {
  if(status != WL_CONNECTED) wifi_init();  
  
  int packetSize = Udp.parsePacket();
//  Serial.println(packetSize);
  while (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    respond_command(packetBuffer[0]);
//    byte msg = "x";
//    byte ip[4] = {192, 168, 43, 161};
//    Udp.beginPacket(ip, port);
//    Udp.write(msg);
    packetSize = Udp.parsePacket();
  }
}

void wifi_clear() {
  while(1) {
    int packetSize = Udp.parsePacket();
    if (packetSize) {
      // read the packet into packetBufffer
      int len = Udp.read(packetBuffer, 255);
    }
    else return;
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

// ####################################################### Respond to Command #################################################
void respond_command (byte I) {
  if((char) I == 'h') {
    halted = true;
  } else {
    halted = false;
    int ang = I;
    if (ang >= 128) ang = ang - 256;
    Serial.println(ang);
    command_angle = ang;
  }
  last_command_time = millis();
//  digitalWrite(RED_LED, digitalRead(RED_LED) ^ 1);
}
