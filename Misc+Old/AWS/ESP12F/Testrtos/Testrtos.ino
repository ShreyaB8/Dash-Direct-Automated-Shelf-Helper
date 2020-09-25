#include <Arduino_FreeRTOS.h>

const int led1 = 2; // Pin of the LED

void setup(){
  Serial.begin(112500);
  delay(1000);
  
   xTaskCreate(
    RFID,    // Function that should be called
    "RFID",   // Name of the task (for debugging)
    10000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    1,               // Task priority
    NULL             // Task handle
  );
  xTaskCreate(
    AWS_loadcell,    // Function that should be called
    "AWS_loadcell",   // Name of the task (for debugging)
    10000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    4,               // Task priority
    NULL             // Task handle
  );
  xTaskCreate(
    AWS_IOT,    // Function that should be called
    "AWS_IOT",   // Name of the task (for debugging)
    100,            // Stack size (bytes)
    NULL,            // Parameter to pass
    5,               // Task priority
    NULL             // Task handle
  );
}


void RFID(void * parameter){
    Serial.println("RFID Working");
  }
void AWS_loadcell(void * parameter){
    Serial.println("LC Working");
  }
void AWS_IOT(void * parameter){
    Serial.println("AWIOT Working");
  }
}

//  All working! Need Harshil's code. Stack sizes are setup acc the expected data 
void loop( void)
{
}
