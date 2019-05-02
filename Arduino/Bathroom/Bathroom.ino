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
int stateButtonMotion=1;

int sensorTemperature = A0;
int sensorLight= 16;
int lampKitchenPin = 5;
int windowsKitchenPin = 4;
int radiatorKitchenPin = 13;
int buttonMotionKitchenPin = 14;



const char* ssid = "SmartHome";
const char* password = "6165869092FD";
const char* mqtt_server = "192.168.0.100";

const char* LampKitchen= "bathroom/command/lamp/set";
const char* WindowsShutterKitchen= "bathroom/command/shutter/set";
const char* RadiatorKitchen = "bathroom/command/radiator/set";


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
   if (strcmp(topic,"bathroom/command/radiator/set") == 0) {
     if (message == "1") {
      stateButtonRadiator=1;
      digitalWrite(radiatorKitchenPin, HIGH);
     }else {
      stateButtonRadiator=0;
      digitalWrite(radiatorKitchenPin, LOW);
     }         
   }
   if (strcmp(topic,"bathroom/command/lamp/set") == 0) {    
     if (message == "1") {
      stateButtonLamp=1;
      digitalWrite(lampKitchenPin, HIGH);
     }else {
      stateButtonLamp=0;
      digitalWrite(lampKitchenPin, LOW);
     }         
   }
   if (strcmp(topic,"bathroom/command/shutter/set") == 0) {
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
  pinMode(buttonMotionKitchenPin,INPUT_PULLUP);

  
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
   int buttonMotion = digitalRead(buttonMotionKitchenPin);
   if(buttonMotion==1){
      if (stateButtonMotion==0){
        stateButtonMotion=1;
        client.publish("bathroom/sensor/motion/state","1");
      } else {
        stateButtonMotion=0;
        client.publish("bathroom/sensor/motion/state","0");
      }
      delay(500);
    }
  }


   
  if(millis() - lastTimeout-10 >= 1000){

      int sensorValue = analogRead(sensorLight);
      // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
      float lightvoltage = sensorValue * (5.0 / 1023.0);  
      float percentualLight = (lightvoltage * 100) / 5;

      if(percentualLight != stateLight ){
        stateLight = percentualLight;
        dtostrf(percentualLight, 4, 4, light);
        client.publish("bathroom/sensor/light/state",light);
      }
      
      delay(500);
      lastTimeout = millis();
   
   }   
}
