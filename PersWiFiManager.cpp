/* PersWiFiManager ESP32
   version 3.0.2
   https://r-downing.github.io/PersWiFiManager/
   https://github.com/schweini/PersWiFiManager
*/
 
 
#include "PersWiFiManager.h"

#define WIFI_HTM_PROGMEM

// for some reason this one isn't in ESP32
#ifndef ENC_TYPE_NONE
#define ENC_TYPE_NONE 7
#endif

#ifdef WIFI_HTM_PROGMEM
const char wifi_htm[] PROGMEM = R"=====(<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no"/><title>ESP WiFi</title><script>function g(i){return document.getElementById(i);};function p(t,l){if(confirm(t)) window.location=l;};function E(s){return document.createElement(s)};var S="setAttribute",A="appendChild",H="innerHTML",X,wl;function scan(){if(X) return;X=new XMLHttpRequest(),wl=document.getElementById('wl');wl[H]="Scanning...";X.onreadystatechange=function(){if (this.readyState==4&&this.status==200){X=0;wl[H]="";this.responseText.split("\n").forEach(function (e){let t=e.split(","), s=t.slice(2).join(',');var d=E('div'),i=E('a'),c=E('a');i[S]('class','s'); c[S]('class','q');i.onclick=function(){g('s').value=s;g('p').focus();};i[A](document.createTextNode(s));c[H]=t[0]+"%"+(parseInt(t[1])?"\uD83D\uDD12":"\u26A0");wl[A](i); wl[A](c);wl[A](document.createElement('br'));});}};X.open("GET","wifi/list",true);X.send();};</script><style>input{padding:5px;font-size:1em;width:95%;}body{text-align:center;font-family:verdana;background-color:black;color:white;}a{color:#1fa3ec;}button{border:0;border-radius:0.3em;background-color:#1fa3ec;color:#fff;line-height:2.4em;font-size:1.2em;width:100%;display:block;}.q{float:right;}.s{display:inline-block;width:14em;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;}#wl{line-height:1.5em;}</style></head><body><div style='text-align:left;display:inline-block;width:320px;padding:5px'><button onclick="scan()">&#x21bb; Scan</button><p id='wl'></p><form method='post' action='/wifi/connect'><input id='s' name='n' length=32 placeholder='SSID'><br><input id='p' name='p' length=64 type='password' placeholder='password'><br><br><button type='submit'>Connect</button></form><br><br><button onclick="p('Start WPS?','/wifi/wps')">WPS Setup</button><br><button onclick="p('Reboot device?','/wifi/rst')">Reboot</button><br><a href="javascript:history.back()">Back</a> |<a href="/">Home</a></div></body></html>)=====";
#endif

const byte DNS_PORT = 53;
IPAddress apIP(192,168,4,1);
//IPAddress apIP(10,10,10,1);
//IPAddress apIP(172,217,28,1);
PersWiFiManager::PersWiFiManager(WEBSERVER& s, DNSServer& d) {
  _server = &s;
  _dnsServer = &d;
  _apPass = "";
} //PersWiFiManager

bool PersWiFiManager::attemptConnection(const String& ssid, const String& pass) {
  //attempt to connect to wifi
  WiFi.mode(WIFI_STA);
  if (ssid.length()) 
  {
    if (pass.length()) WiFi.begin(ssid.c_str(), pass.c_str());
    else WiFi.begin(ssid.c_str());	
  } 
  else 
  {
    WiFi.begin();
  }

  //if in nonblock mode, skip this loop
  _connectStartTime = millis();// + 1;
  while (!_connectNonBlock && _connectStartTime) {
    handleWiFi();
    delay(10);
  }

  return (WiFi.status() == WL_CONNECTED);

} //attemptConnection

void PersWiFiManager::handleWiFi() {
  if (!_connectStartTime) return;

  if (WiFi.status() == WL_CONNECTED) {
    _connectStartTime = 0;
    if (_connectHandler) _connectHandler();
    return;
  }

  //if failed or not connected and time is up
  if (
		(WiFi.status() == WL_CONNECT_FAILED) 
		|| 
		((WiFi.status() != WL_CONNECTED) && ((millis() - _connectStartTime) > (1000 * WIFI_CONNECT_TIMEOUT)))
		) 
  {
	  DEBUG_PRINTLN("Wifi Timed out, starting AP");
    startApMode();
    _connectStartTime = 0; //reset connect start time
  }

} //handleWiFi

void PersWiFiManager::startApMode(){
  //start AP mode
//  IPAddress apIP(192, 168, 1, 1);
  //IPAddress apIP(10, 10, 10, 1);
  WiFi.mode(WIFI_AP);
//  WiFi.softAPConfig(apIP, IPAddress(0, 0, 0, 0), IPAddress(255, 255, 255, 0));
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  _apPass.length() ? WiFi.softAP(getApSsid().c_str(), _apPass.c_str()) : WiFi.softAP(getApSsid().c_str());
  if (_apHandler) _apHandler();  
}//startApMode

void PersWiFiManager::setConnectNonBlock(bool b) {
  _connectNonBlock = b;
} //setConnectNonBlock

void PersWiFiManager::setupWiFiHandlers() {

	DEBUG_PRINTLN("wifi handlers");

  //IPAddress apIP(10,10,10,1);

	//DEBUG_PRINTLN("Delay 10 secs before dns start");
	//delay(10000);
  
  _dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
  _dnsServer->start( DNS_PORT, "*", apIP); //used for captive portal in AP mode
  //_dnsServer->start((byte)53, "*", apIP); //used for captive portal in AP mode
  
  _server->on("/wifi/list", [&] () {
    //scan for wifi networks
    int n = WiFi.scanNetworks();

    //build array of indices
    int ix[n];
    for (int i = 0; i < n; i++) ix[i] = i;

    //sort by signal strength
    for (int i = 0; i < n; i++) for (int j = 1; j < n - i; j++) if (WiFi.RSSI(ix[j]) > WiFi.RSSI(ix[j - 1])) std::swap(ix[j], ix[j - 1]);
    //remove duplicates
    for (int i = 0; i < n; i++) for (int j = i + 1; j < n; j++) if (WiFi.SSID(ix[i]).equals(WiFi.SSID(ix[j])) && WiFi.encryptionType(ix[i]) == WiFi.encryptionType(ix[j])) ix[j] = -1;

    //build plain text string of wifi info
    //format [signal%]:[encrypted 0 or 1]:SSID
    String s = "";
    s.reserve(2050);
    for (int i = 0; i < n && s.length() < 2000; i++) { //check s.length to limit memory usage
      if (ix[i] != -1) {
        s += String(i ? "\n" : "") + ((constrain(WiFi.RSSI(ix[i]), -100, -50) + 100) * 2) + ","
             // '7' hardcoded for none for ESP32
			 // + ((WiFi.encryptionType(ix[i]) == 7) ? 0 : 1) + "," + WiFi.SSID(ix[i]);
             + ((WiFi.encryptionType(ix[i]) == ENC_TYPE_NONE) ? 0 : 1) + "," + WiFi.SSID(ix[i]);
      }
    }

    //send string to client
    _server->send(200, "text/plain", s);
  }); //_server->on /wifi/list

  
  // dunno why WPS doens't work on ESP32, but i don't like WPS anyhow.

#ifdef ESP8266
  _server->on("/wifi/wps", [&]() {
    _server->send(200, "text/html", "attempting WPS");
    WiFi.mode(WIFI_STA);
    WiFi.beginWPSConfig();
    delay(100);
    if (WiFi.status() != WL_CONNECTED) {
      attemptConnection("", "");
    }
  }); //_server->on /wifi/wps
#else
  _server->on("/wifi/wps", [&]() {
    _server->send(200, "text/html", "<h1>WPS not implemented on ESP32 yet.");
  }); //_server->on /wifi/wps
	
#endif

  _server->on("/wifi/connect", [&]() {
    _server->send(200, "text/html", "connecting...");
    attemptConnection(_server->arg("n"), _server->arg("p"));
  }); //_server->on /wifi/connect

  _server->on("/wifi/ap", [&](){
    _server->send(200, "text/html", "access point: "+getApSsid());
    startApMode();
  }); //_server->on /wifi/ap

  _server->on("/wifi/rst", [&]() {
    _server->send(200, "text/html", "Rebooting...");
    delay(100);
    //ESP.restart();
  });

#ifdef WIFI_HTM_PROGMEM
  _server->on("/wifi.htm", [&]() {
    _server->send(200, "text/html", wifi_htm);
  });
#endif

}//setupWiFiHandlers

bool PersWiFiManager::begin(const String& ssid, const String& pass) {
#ifdef PERSAPDEBUG
	PERSAPDEBUG.println("beginning.")
#endif


#ifdef ESP8266
	// at least on ESP32, this doesn't work, becayse the DNServer has to be declared AFTER the AP is ready. Not sure on ESP8266
	setupWiFiHandlers();
	return attemptConnection(ssid, pass); //switched order of these two for return
#else
  attemptConnection(ssid, pass);
  setupWiFiHandlers();
#endif
} //begin

String PersWiFiManager::getApSsid() {
  return _apSsid.length() ? _apSsid : "ESP32AP";
} //getApSsid

void PersWiFiManager::setApCredentials(const String& apSsid, const String& apPass) {
  if (apSsid.length()) _apSsid = apSsid;
  if (apPass.length() >= 8) _apPass = apPass;
} //setApCredentials

void PersWiFiManager::onConnect(WiFiChangeHandlerFunction fn) {
  _connectHandler = fn;
}

void PersWiFiManager::onAp(WiFiChangeHandlerFunction fn) {
  _apHandler = fn;
}


