#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

float stateTemperature = 0.0;
float stateLight = 0.0;

long lastTimeout = 0;
int stateButtonShutter=0;
int stateButtonLamp=0;
int stateButtonRadiator=0;

int sensorTemperature = A0;
int sensorLight= 16;
int lampKitchenPin = 5;
int windowsKitchenPin = 4;
int radiatorKitchenPin = 13;
int buttonWindowsKitchenPin = 15;
int buttonLampKitchenPin = 12;
int buttonRadiatorKitchenPin = 14;

const char* ssid = "SmartHome";
const char* password = "6165869092FD";
const char* mqtt_server = "192.168.0.100";

const char* LampKitchen= "kitchen/command/lamp/set";
const char* WindowsShutterKitchen= "kitchen/command/shutter/set";
const char* RadiatorKitchen = "kitchen/command/radiator/set";


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
 Serial.println("callback:");
 String message = "";
 if (length > 0) {
   for (int i=0; i<length; i++) {
        message += (char)payload[i];
   } 
   Serial.println("Topic:");
   Serial.println(topic);
   Serial.println("Message:");
   Serial.println(message);
   if (strcmp(topic,"kitchen/command/radiator/set") == 0) {
     if (message == "1") {
      stateButtonRadiator=1;
      digitalWrite(radiatorKitchenPin, HIGH);
     }else {
      stateButtonRadiator=0;
      digitalWrite(radiatorKitchenPin, LOW);
     }         
   }
   if (strcmp(topic,"kitchen/command/lamp/set") == 0) {    
     if (message == "1") {
      stateButtonLamp=1;
      digitalWrite(lampKitchenPin, HIGH);
     }else {
      stateButtonLamp=0;
      digitalWrite(lampKitchenPin, LOW);
     }         
   }
   if (strcmp(topic,"kitchen/command/shutter/set") == 0) {
     if (message == "1") {
      stateButtonShutter=1;
      digitalWrite(windowsKitchenPin, HIGH);
     }else {
      stateButtonShutter=0;
      digitalWrite(windowsKitchenPin, LOW);
     }         
   }      
 }
}


void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);

  pinMode(sensorTemperature,INPUT);
  pinMode(sensorLight,INPUT);
  pinMode(lampKitchenPin,OUTPUT);
  pinMode(windowsKitchenPin,OUTPUT);
  pinMode(radiatorKitchenPin,OUTPUT);
  pinMode(buttonWindowsKitchenPin,INPUT_PULLUP);
  pinMode(buttonLampKitchenPin,INPUT_PULLUP);
  pinMode(buttonRadiatorKitchenPin,INPUT_PULLUP);

  
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {

      client.subscribe(LampKitchen);
      client.subscribe(WindowsShutterKitchen);
      client.subscribe(RadiatorKitchen);

    }

}

void loop() {

  char temperature[10] = "0";
  char light[10] = "0";

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if(millis() - lastTimeout-2 >= 1000){
   int buttonRadiator = digitalRead(buttonRadiatorKitchenPin);
   if(buttonRadiator==1){
      if (stateButtonRadiator==0){
        stateButtonRadiator=1;
        digitalWrite(radiatorKitchenPin, HIGH);
        client.publish("kitchen/sensor/radiator/state","1");
      } else {
        stateButtonRadiator=0;
        digitalWrite(radiatorKitchenPin, LOW);
        client.publish("kitchen/sensor/radiator/state","0");
      }
      delay(500);
    }
  }

  if(millis() - lastTimeout-3 >= 1000){
    int buttonLamp = digitalRead(buttonLampKitchenPin);
    if(buttonLamp==1){
      if (stateButtonLamp==0){
        stateButtonLamp=1;
        digitalWrite(lampKitchenPin, HIGH);
        client.publish("kitchen/sensor/lamp/state","1");
      } else {
        stateButtonLamp=0;
        digitalWrite(lampKitchenPin, LOW);
        client.publish("kitchen/sensor/lamp/state","0");
      }
      delay(500);
    }
  }

  if(millis() - lastTimeout-4 >= 1000){
    int buttonWindow = digitalRead(buttonWindowsKitchenPin);
    if(buttonWindow==1){
      if (stateButtonShutter==0){
        stateButtonShutter=1;
        digitalWrite(windowsKitchenPin, HIGH);
        client.publish("kitchen/sensor/shutter/state","1");
      } else {
        stateButtonShutter=0;
        digitalWrite(windowsKitchenPin, LOW);
        client.publish("kitchen/sensor/shutter/state","0");
      }
      delay(500);
    }
  }  
  

  if(millis() - lastTimeout-5 >= 1000){

      int analogValue = analogRead(sensorTemperature);
      float millivolts = (analogValue/1024.0) * 3300; //3300 is the voltage provided by NodeMCU
      float celsius = millivolts/15;  

      if(celsius - stateTemperature > 0.5){
        stateTemperature = celsius;
        dtostrf(celsius, 4, 4, temperature);
        client.publish("kitchen/sensor/temperature/state", temperature);
      }
      
      delay(500);
      lastTimeout = millis();
   }
   
  if(millis() - lastTimeout-10 >= 1000){

      int sensorValue = analogRead(sensorLight);
      // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
      float lightvoltage = sensorValue * (5.0 / 1023.0);  
      float percentualLight = (lightvoltage * 100) / 5;

      if(percentualLight != stateLight ){
        stateLight = percentualLight;
        dtostrf(percentualLight, 4, 4, light);
        client.publish("kitchen/sensor/light/state",light);
      }
      
      delay(500);
      lastTimeout = millis();
   
   }   
}
