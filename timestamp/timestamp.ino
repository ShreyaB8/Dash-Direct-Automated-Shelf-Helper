#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#define NTP_ADDRESS  "asia.pool.ntp.org"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS);

String formattedTime = timeClient.getFormattedTime();
unsigned long epochTime =  timeClient.getEpochTime();


const char* ssid = "Airtelshreya";
const char* password = "bhukamp2020";


void setup(){
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  timeClient.begin();
  timeClient.setTimeOffset(19800);
  
}

String time_stamp() {
  formattedTime = timeClient.getFormattedTime();
  epochTime =  timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);
  int currentYear = ptm->tm_year+1900;
  int currentMonth = ptm->tm_mon+1;
  int monthDay = ptm->tm_mday;
  
  String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);

  timeClient.update();
  String time_stamp_full = String(formattedTime) + " " + String(currentDate);
  return(time_stamp_full);
}

void loop()
{
   String timestampret = time_stamp();
   Serial.print("from loop  ");
   Serial.print(timestampret);
   Serial.print("\n");
  }
