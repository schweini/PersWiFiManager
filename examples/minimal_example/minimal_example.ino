#include <PersWiFiManager.h>
//#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

#define DEVICE_NAME "ESP DEVICE"

//server objects
WebServer server(80);
DNSServer dnsServer;
PersWiFiManager persWM(server, dnsServer);


void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting...");

  persWM.begin();

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
    server.send(200, "text/plain", "<html><h1>Hello!</h1><ul><li><a href='/wifi.htm'>wifi config</a></li><li><a href='/test'>test</a></li></ul></html>");
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
