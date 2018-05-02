# PersWiFiManager + ESP32

(I'm just trying to add ESP32 support right now, will issue a PR later)
- needs an ESP32 webserver. Until the official one is fixed, use this one: https://github.com/bbx10/WebServer_tng

Current status on ESP32:
- compiles correctly
- default AP IP is 192.168.1.1
- default wifi config panel address is 192.168.1.1/wifi.htm
~~- 'sign in to network' doesn't seem to be detected on my ESP32/Android combination yet. So turn off mobil data to access ESP's pages.~~
- At least on my Android OnePlus One running LineageOS, the phone will route all traffic over the mobile data connection while connected to a wifi AP that is either in 'captive portal' mode, or if it is detected as not having internet. Annoying workaround is to turn of mobile data. (https://stackoverflow.com/questions/26982762/get-wifi-captive-portal-info?lq=1)

Todo:
- fix (create?) basic usage case documentation
- wrap ESP32 specific stuff in ifdefs.
~~- fix "Sign in to network" response.~~

Persistent WiFiManager Arduino library for ESP8266 and ESP32 -based microcontrollers


public funtions:
```
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
```


This Persistent WiFi Manager provides a WiFi Settings web interface for ESP8266-based microcontrollers. It can autoconnect to the last used network, and then if unsuccessful, switch to AP mode with a captive DNS server. This action is non-blocking; the program will proceed to the main loop after, regardless of WiFi connection status. In AP mode, any device (phone, computer, etc) can connect to this network to configure the wifi connection, or access the device as a webserver. 

This library and UI was inspired by tzapu's [WiFiManager library](https://github.com/tzapu/WiFiManager). The main difference is that it allows the program to continue functioning normally, even in AP mode. It is also more memory efficient, as it does not have to dynamically build the page, and can serve it from SPIFFS rather than PROGMEM. 

[Full Documentation](http://ryandowning.net/PersWiFiManager)
