#include <SoftwareSerial.h>

SoftwareSerial mySerial(8, 9);

const int PIN_TO_SENSOR = 2;   // the pin that OUTPUT pin of sensor is 
int pinStateCurrent   = LOW; // current state of pin
int pinStatePrevious  = LOW; 
int buzzer = 12;
unsigned int prevTime = 0;
unsigned int prev = 0;
bool isOn = false;
bool isReady = false;



void setup() {
  Serial.begin(9600);            // initialize serial
  mySerial.begin(9600);

  Serial.println("Initializing..."); 
  delay(1000);

  mySerial.println("AT");
  isReady = waitForResponse("OK");
  updateSerial();
  

  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();

  pinMode(PIN_TO_SENSOR, INPUT); // set arduino pin to input mode to read value 
  pinMode(buzzer,OUTPUT);
}

bool waitForResponse(const char* expectedResponse) {
  String response = "";

  while (true) {
    while (mySerial.available()) {
      char c = mySerial.read();
      response += c;
    }
    if (response.indexOf(expectedResponse) != -1) {
      return true; // Found the expected response
    }
  }
}

void sendMessage(){
  mySerial.println("AT+CMGS=\"+233557650797\"");
  updateSerial();
  mySerial.print("Detected Motion"); //text content
  updateSerial();
  mySerial.write(26);
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}

void loop() {
  pinStatePrevious = pinStateCurrent; // store old state
  pinStateCurrent = digitalRead(PIN_TO_SENSOR);   // read new state

  unsigned int currentTime = millis();

  
  if (pinStatePrevious == LOW && pinStateCurrent == HIGH) { 
    delay(500);
    if(pinStateCurrent == 1){
      Serial.println("Motion detected!");
      digitalWrite(buzzer,HIGH);

      
      isOn = true;
      
      sendMessage();
      delay(2000);
    } 
    
  }
  
  if (pinStatePrevious == HIGH && pinStateCurrent == LOW) {   // pin state change: HIGH -> LOW
    Serial.println("Motion stoopped!");
  }

  if(currentTime - prevTime >= 10000){
    if(isOn){

      digitalWrite(buzzer,LOW);
      Serial.println("stop buzzer");
      isOn = false;
    }

    prevTime = currentTime;
  }
  
}
