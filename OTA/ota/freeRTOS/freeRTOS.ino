
const int led1 = 2; // Pin of the LED

void setup(){
  pinMode(led1, OUTPUT);
}

void toggleLED(void * parameter){
  for(;;){
    digitalWrite(led1, HIGH);
    //vTaskDelay(500 / portTICK_PERIOD_MS);
    digitalWrite(led1, LOW);
    // Pause the task again for 500ms
    //vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup() {
  xTaskCreate(
    toggleLED,    // Function that should be called
    "Toggle LED",   // Name of the task (for debugging)
    1000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    1,               // Task priority
    NULL             // Task handle
  );
  
}

