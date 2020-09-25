#include <ESP8266WiFi.h>
#include <WiFiManager.h>
void setup() {
  Serial.begin(115200);
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  if(!wifiManager.autoConnect("Airtelshreya","bhukamp2020"))
  {
    Serial.println("Hit timeout on connect, restarting.");
    ESP.restart();
  }
  Serial.println("connected");
}

void loop()
{
  }
