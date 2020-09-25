//Written by Shreyashri Biswas
//Company: Dot
//File: Dash full
//Version: 2.0


#include "HX711.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "FS.h"
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//#define RFIDPIN 13

#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif
#define UPDATE_SIZE_UNKNOWN 0xFFFFFFFF

const char* host = "esp8266-webupdate";
const char* ssid = STASSID;
const char* password = STAPSK;

HX711 scale;
char tag[] ="TAG ID";  // Add the Tag ID or leave it empty if you dont know
char input[12];        // A variable to store the Tag ID being presented
int count = 0;        // A counter variable to navigate through the input[] character array
boolean flag = 0;     // A variable to store the Tag match status

uint8_t dataPin = 5;
uint8_t clockPin = 4;

//Server
ESP8266WebServer server(80);

/* Style */
String style =
"<style>#file-input,input{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px}"
"input{background:#f1f1f1;border:0;padding:0 15px}body{background:#3498db;font-family:sans-serif;font-size:14px;color:#777}"
"#file-input{padding:0;border:1px solid #ddd;line-height:44px;text-align:left;display:block;cursor:pointer}"
"#bar,#prgbar{background-color:#f1f1f1;border-radius:10px}#bar{background-color:#3498db;width:0%;height:10px}"
"form{background:#fff;max-width:258px;margin:75px auto;padding:30px;border-radius:5px;text-align:center}"
".btn{background:#3498db;color:#fff;cursor:pointer}</style>";

/* Login page */
String loginIndex = 
"<form name=loginForm>"
"<h1>ESP32 Login</h1>"
"<input name=userid placeholder='User ID'> "
"<input name=pwd placeholder=Password type=Password> "
"<input type=submit onclick=check(this.form) class=btn value=Login></form>"
"<script>"
"function check(form) {"
"if(form.userid.value=='admin' && form.pwd.value=='admin')"
"{window.open('/serverIndex')}"
"else"
"{alert('Error Password or Username')}"
"}"
"</script>" + style;
 
/* Server Index Page */
String serverIndex = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
"<input type='file' name='update' id='file' onchange='sub(this)' style=display:none>"
"<label id='file-input' for='file'>   Choose file...</label>"
"<input type='submit' class=btn value='Update'>"
"<br><br>"
"<div id='prg'></div>"
"<br><div id='prgbar'><div id='bar'></div></div><br></form>"
"<script>"
"function sub(obj){"
"var fileName = obj.value.split('\\\\');"
"document.getElementById('file-input').innerHTML = '   '+ fileName[fileName.length-1];"
"};"
"$('form').submit(function(e){"
"e.preventDefault();"
"var form = $('#upload_form')[0];"
"var data = new FormData(form);"
"$.ajax({"
"url: '/update',"
"type: 'POST',"
"data: data,"
"contentType: false,"
"processData:false,"
"xhr: function() {"
"var xhr = new window.XMLHttpRequest();"
"xhr.upload.addEventListener('progress', function(evt) {"
"if (evt.lengthComputable) {"
"var per = evt.loaded / evt.total;"
"$('#prg').html('progress: ' + Math.round(per*100) + '%');"
"$('#bar').css('width',Math.round(per*100) + '%');"
"}"
"}, false);"
"return xhr;"
"},"
"success:function(d, s) {"
"console.log('success!') "
"},"
"error: function (a, b, c) {"
"}"
"});"
"});"
"</script>" + style;

// Time stamp
#define NTP_ADDRESS  "asia.pool.ntp.org"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS);

String formattedTime = timeClient.getFormattedTime();
unsigned long epochTime =  timeClient.getEpochTime();

//aws end point

const char* AWS_endpoint = "a2bpcrj37zjg84-ats.iot.ap-northeast-1.amazonaws.com"; //MQTT broker ip

void callback(char* topic, byte* payload, unsigned int length) {
Serial.print("Message arrived [");
Serial.print(topic);
Serial.print("] ");
for (int i = 0; i < length; i++) {
Serial.print((char)payload[i]);
}
Serial.println();

}
WiFiClientSecure espClient;
PubSubClient client(AWS_endpoint, 8883, callback, espClient); //set MQTT port number to 8883 as per //standard
long lastMsg = 0;
char msg[50];
int value = 0;


void setup_wifi() {

delay(10);
// We start by connecting to a WiFi network
espClient.setBufferSizes(512, 512);
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}

Serial.println("");
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());

timeClient.begin();
while(!timeClient.update()){
timeClient.forceUpdate();
}

espClient.setX509Time(timeClient.getEpochTime());

}

void reconnect() {
// Loop until we're reconnected
while (!client.connected()) {
Serial.print("Attempting MQTT connection...");
// Attempt to connect
if (client.connect("ESPthing")) {
Serial.println("connected");
// Once connected, publish an announcement...
client.publish("outTopic", "hello world");
// ... and resubscribe
client.subscribe("inTopic");
} else {
Serial.print("failed, rc=");
Serial.print(client.state());
Serial.println(" try again in 5 seconds");

char buf[256];
espClient.getLastSSLError(buf,256);
Serial.print("WiFiClientSecure SSL error: ");
Serial.println(buf);

// Wait 5 seconds before retrying
delay(5000);
}
}
}

void setup()
{
  Serial.begin(115200);
  
  //RFID
  //Serial.setTimeout(2000); //sets the maximum milliseconds to wait for serial data. It defaults to 1000 milliseconds.
  //pinMode(RFIDPIN,OUTPUT); // RFID OUTPUT


  //load cell
  Serial.println(__FILE__);
  Serial.print("LIBRARY VERSION: ");
  Serial.println(HX711_LIB_VERSION);
  Serial.println();

  scale.begin(dataPin, clockPin);

  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));
  
  Serial.println("\nEmpty the scale, press a key to continue");
  while(!Serial.available());
  while(Serial.available()) Serial.read();
  
  scale.tare();
  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));


  Serial.println("\nPut a 1 kg in the scale, press a key to continue");
  while(!Serial.available());
  while(Serial.available()) Serial.read();

  scale.callibrate_scale(1000, 5);
  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));

  Serial.println("\nScale is callibrated, press a key to continue");
  while(!Serial.available());
  while(Serial.available()) Serial.read();

//web update
  pinMode(2, OUTPUT);
  Serial.println();
  Serial.println("Booting Sketch...");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    MDNS.begin(host);
    server.on("/", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", serverIndex);
    });
    server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
    MDNS.addService("http", "tcp", 80);

    Serial.printf("Ready! Open http://%s.local in your browser\n", host);
  } else {
    Serial.println("WiFi Failed");
  }


  //Deep sleep
  /*
  while(!Serial) { }
  Serial.println("I'm awake, but I'm going into deep sleep mode in 10sec");
  ESP.deepSleep(10e6); //20 sec sleep rn
  */
   //aws
      Serial.setDebugOutput(true);
      // initialize digital pin LED_BUILTIN as an output.
      pinMode(LED_BUILTIN, OUTPUT);
      setup_wifi();
      delay(1000);
      if (!SPIFFS.begin()) {
      Serial.println("Failed to mount file system");
      return;
      }
      
      Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());
      
      // Load certificate file
      File cert = SPIFFS.open("/cert.der", "r"); //replace cert.crt eith your uploaded file name
      if (!cert) {
      Serial.println("Failed to open cert file");
      }
      else
      Serial.println("Success to open cert file");
      
      delay(1000);
      
      if (espClient.loadCertificate(cert))
      Serial.println("cert loaded");
      else
      Serial.println("cert not loaded");
      
      // Load private key file
      File private_key = SPIFFS.open("/private.der", "r"); //replace private eith your uploaded file name
      if (!private_key) {
      Serial.println("Failed to open private cert file");
      }
      else
      Serial.println("Success to open private cert file");
      
      delay(1000);
      
      if (espClient.loadPrivateKey(private_key))
      Serial.println("private key loaded");
      else
      Serial.println("private key not loaded");
      
      // Load CA file
      File ca = SPIFFS.open("/ca.der", "r"); //replace ca eith your uploaded file name
      if (!ca) {
      Serial.println("Failed to open ca ");
      }
      else
      Serial.println("Success to open ca");
      
      delay(1000);
      
      if(espClient.loadCACert(ca))
      Serial.println("ca loaded");
      else
      Serial.println("ca failed");
      
      Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());

      //timestamp
      
        timeClient.begin();
        timeClient.setTimeOffset(19800);
}


////Setup ends



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


int rfid()
{
  Serial.print("In RFID function");
  if(Serial.available())// Check if there is incoming data in the RFID Reader Serial Buffer.
  {
    count = 0; // Reset the counter to zero
    /* Keep reading Byte by Byte from the Buffer till the RFID Reader Buffer is empty 
       or till 12 Bytes (the ID size of our Tag) is read */
    while(Serial.available() && count < 12) 
    {
      input[count] = Serial.read(); // Read 1 Byte of data and store it in the input[] variable
      count++; // increment counter
      delay(5);
    }
    /* When the counter reaches 12 (the size of the ID) we stop and compare each value 
        of the input[] to the corresponding stored value */
    if(count == 12) // check with the RFID card max length 
    {
      count =0; // reset counter varibale to 0
      flag = 1;
      /* Iterate through each value and compare till either the 12 values are 
         all matching or till the first mistmatch occurs */
      while(count<12 && flag !=0)  
      {
        if(input[count]==tag[count])
        flag = 1; // everytime the values match, we set the flag variable to 1
        else
        flag= 0; 
                               /* if the ID values don't match, set flag variable to 0 and 
                                  stop comparing by exiting the while loop */
        count++; // increment i
      }
    }
    if(flag == 1) // If flag variable is 1, then it means the tags match
    {
      Serial.println("Access Allowed!");
      return(1);
    }
    else
    {
      Serial.println("Access Denied"); // Incorrect Tag Message
      return(0);
    }
    /* Fill the input variable array with a fixed value 'F' to overwrite 
    all values getting it empty for the next read cycle */
    for(count=0; count<12; count++) 
    {
      input[count]= 'F';
    }
    count = 0; // Reset counter variable
  }
}

float loadcell()
{
  Serial.print("In load cell function");
  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));
  delay(250);
  return(scale.get_units(10));
}

void aws_iot(int rfid, float data, String dt_timestamp)
{
            
        char rfid_st[15], data_st[10], time_st[10], buffer[10];
        //dtostrf(FLOAT,WIDTH,PRECSISION,BUFFER);
        String rfid_dt = dtostrf(rfid, 5, 2, buffer );
        rfid_dt.toCharArray(rfid_st,10);
        String data_dt = dtostrf(data, 5, 2, buffer);
        data_dt.toCharArray(data_st,10);
        dt_timestamp.toCharArray(time_st,10);
        
        if (!client.connected()) {
        reconnect();
        }
        client.loop();
        long now = millis();
        if (now - lastMsg > 5000) {
          lastMsg = now;
        Serial.print("Publish message: ");
        Serial.println(rfid_st);
        Serial.print("/t");
        Serial.println(data_st);
        Serial.print("/t");
        Serial.println(time_st);
        Serial.print("/n");
        client.publish("dash/rfid", rfid_st);
        client.publish("dash/data", data_st);
        client.publish("dash/time", time_st);
        Serial.print("Heap: "); Serial.println(ESP.getFreeHeap()); //Low heap can cause problems
        }
        digitalWrite(LED_BUILTIN, HIGH); 
        delay(100); 
        digitalWrite(LED_BUILTIN, LOW); 
        delay(100);
     
}


/////Loop function
void loop()
{
    String timestampret = time_stamp();
    float LC_data = loadcell();
    int rfid_stat=rfid();
    rfid_stat=1;
    if(rfid_stat==1)
    {
      
      aws_iot(26378912639,LC_data,timestampret);
      //dummy rfid value
    
    }

  //web update
  server.handleClient();
  MDNS.update();
  digitalWrite(2, HIGH);
}
