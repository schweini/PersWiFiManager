#ifndef PERSWIFIMANAGER_H
#define PERSWIFIMANAGER_H

#ifdef ESP8266
	#include <ESP8266WiFi.h>
	#include <ESP8266WebServer.h>
	#define WEBSERVER ESP8266WebServer
	#define WIFI ESP8266WiFi
#else
	#include <WiFi.h>
	#include <WebServer.h>
	#define WEBSERVER WebServer
	#define WIFI WiFi

#endif

#include <DNSServer.h>

#ifndef WIFI_CONNECT_TIMEOUT
#define WIFI_CONNECT_TIMEOUT 30
#endif

class PersWiFiManager {

  public:

    typedef std::function<void(void)> WiFiChangeHandlerFunction;

    PersWiFiManager(WEBSERVER& s, DNSServer& d);

    bool attemptConnection(const String& ssid = "", const String& pass = "");

    void setupWiFiHandlers();

    bool begin(const String& ssid = "", const String& pass = "");

    String getApSsid();

    void setApCredentials(const String& apSsid, const String& apPass = "");

    void setConnectNonBlock(bool b);

    void handleWiFi();

    void startApMode();

    void onConnect(WiFiChangeHandlerFunction fn);

    void onAp(WiFiChangeHandlerFunction fn);

  private:
    WEBSERVER * _server;
    DNSServer * _dnsServer;
    String _apSsid, _apPass;

    bool _connectNonBlock;
    unsigned long _connectStartTime;

    WiFiChangeHandlerFunction _connectHandler;
    WiFiChangeHandlerFunction _apHandler;

};//class

#endif

