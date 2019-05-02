#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <dht11.h>

dht11 DHT11;

WiFiClient espClient;
PubSubClient client(espClient);

float stateTemperature = 0.0;
float stateLight = 0.0;

long lastTimeout = 0;
int stateButtonMotion=1;

int sensorTemperature = 2;
int sensorLight= 16;
int buttonMotionKitchenPin = 14;



const char* ssid = "SmartHome";
const char* password = "6165869092FD";
const char* mqtt_server = "192.168.0.100";


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
  pinMode(buttonMotionKitchenPin,INPUT_PULLUP);

  
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      
      Serial.println("Connected");

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
        client.publish("garden/sensor/motion/state","1");
      } else {
        stateButtonMotion=0;
        client.publish("garden/sensor/motion/state","0");
      }
      delay(500);
    }
  }


  if(millis() - lastTimeout-5 >= 1000){

      uint8_t sensTemp = DHT11.read(sensorTemperature);
      float celsius = DHT11.temperature;  
      
      if(celsius - stateTemperature > 0.5){
        stateTemperature = celsius;
        dtostrf(celsius, 4, 4, temperature);
        client.publish("garden/sensor/temperature/state", temperature);
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
        client.publish("garden/sensor/light/state",light);
      }
      
      delay(500);
      lastTimeout = millis();
   
   }   
}
