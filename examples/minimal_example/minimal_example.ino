#include <PersWiFiManager.h>
#ifdef ESP8266
	#include <ESP8266WiFi.h>
	#include <ESP8266WebServer.h>
	#define AP_SSID "ESP8266_PERSAP"
#else
	#include <WiFi.h>
	#include <WebServer.h>
	#define AP_SSID "ESP32_PERSAP"
#endif

#include <DNSServer.h>

#define DEVICE_NAME "ESP DEVICE"
#define WIFI_CONNECT_TIMEOUT 10

//server objects
WebServer server(80);
DNSServer dnsServer;
PersWiFiManager persWM(server, dnsServer);


void setup() 
{
  Serial.begin(115200);
  Serial.println("Wifi connect timeout is:");
  Serial.print(WIFI_CONNECT_TIMEOUT);
  Serial.println("Starting...");


  persWM.begin(AP_SSID);

  //serve files from SPIFFS
  server.onNotFound([]() {
      server.send(404, "text/html", "<html>four-oh-four! not found!</html>");
  }); //server.onNotFound

  //handles commands from webpage, sends live data in JSON format
  server.on("/test", []() 
  {
    Serial.println("server.on /test was called.");
    server.send(200, "text/plain", "testing!");
  }); //server.on api

  server.on("/", []() 
  {
    Serial.println("server.on / was called.");
    server.send(200, "text/html", "<html><h1>Hello!</h1><ul><li><a href='/wifi.htm'>wifi config</a></li><li><a href='/test'>test</a></li></ul></html>");
  }); //server.on api


  server.begin();
  Serial.print("My client IP is: ");
  Serial.println(WiFi.localIP());
  Serial.println("setup complete.");
} //void setup

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();


} //void loop