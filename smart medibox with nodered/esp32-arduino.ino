#include <WiFi.h>
#include "DHTesp.h"
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESP32Servo.h> //include the servo motor library

//define the pin values
#define DHT_PIN 15
#define BUZZER 12
#define LDR_1 32
#define LDR_2 33
#define SERVO 4

WiFiClient espClient;
PubSubClient mqttClient(espClient);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

DHTesp dhtSensor;

Servo servoMotor; //servo object

bool isScheduledON = false;
unsigned long scheduledOnTime;

char tempAr[6];

char ldrAr[5]; //Mapped highest intensity value amoung 2 ldr

int offsetAngle = 30; //minimum angle
float conFactor = 0.75; //controlling factor

void setup() {
  Serial.begin(115200);
  setupWifi();
  setupMqtt();

  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

  timeClient.begin();
  timeClient.setTimeOffset(5.5*3600);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  //Initializing the 2 ldr as inputs
  pinMode(LDR_1, INPUT);
  pinMode(LDR_2, INPUT);

  servoMotor.attach(SERVO);
}

void loop() {
  if(!mqttClient.connected()){
    connectToBroker();
  }

  mqttClient.loop();

  updateTemperature();
  Serial.println(tempAr);
  mqttClient.publish("MEDIBOX-ADMIN-TEMP", tempAr);

  checkSchedule();

  delay(1000);

  updateLDR();
  Serial.println(ldrAr);
  mqttClient.publish("MEDIBOX-LDR", ldrAr);

  rotateServo(); 

}

void buzzerOn(bool on){
  if(on){
    tone(BUZZER, 256);
  }else{
    noTone(BUZZER);
  }
}

void setupWifi(){
  WiFi.begin("Wokwi-GUEST", "");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFI Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupMqtt(){
  mqttClient.setServer("test.mosquitto.org", 1883);
  mqttClient.setCallback(receiveCallback);
}

void receiveCallback(char*topic, byte*payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]");

  char payloadCharAr[length];
  for(int i=0; i<length; i++){
    Serial.print((char)payload[i]);
    payloadCharAr[i] = (char)payload[i];
  }

  Serial.println();

  if(strcmp(topic, "MEDIBOX-ADMIN-MAIN-ON-OFF") == 0){
    buzzerOn(payloadCharAr[0] == '1');
  }else if(strcmp(topic, "MEDIBOX-ADMIN-SCH-ON") == 0){
    if(payloadCharAr[0]=='N'){
      isScheduledON = false;
    }else{
      isScheduledON = true;
      scheduledOnTime = atol(payloadCharAr);
    }
  }else if (strcmp(topic, "MEDIBOX-MIN-ANGLE") == 0) {
    int sliderValue_1 = atoi(payloadCharAr);
    offsetAngle = sliderValue_1;
  }else if (strcmp(topic, "MEDIBOX-CON-FACTOR") == 0) {
    float sliderValue_2 = atof(payloadCharAr);
    conFactor = sliderValue_2;
  }
}

void connectToBroker(){
  while(!mqttClient.connected()){
    Serial.print("Attempting MQTT connection...");
    if(mqttClient.connect("ESP#32-615163131651")){
      Serial.println("Connected");
      mqttClient.subscribe("MEDIBOX-ADMIN-MAIN-ON-OFF");
      mqttClient.subscribe("MEDIBOX-ADMIN-SCH-ON");
      mqttClient.subscribe("MEDIBOX-MIN-ANGLE");
      mqttClient.subscribe("MEDIBOX-CON-FACTOR");
    }else{
      Serial.println("Failed");
      Serial.print(mqttClient.state());
      delay(5000);
    };
  }
}

void updateTemperature(){
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  String(data.temperature, 2).toCharArray(tempAr, 6);
}

unsigned long getTime(){
  timeClient.update();
  return timeClient.getEpochTime();
}

void checkSchedule(){
  if(isScheduledON){
    unsigned long currentTime = getTime();
    if(currentTime > scheduledOnTime){
      buzzerOn(true);
      isScheduledON = false;
      mqttClient.publish("MEDIBOX-ADMIN-MAIN-ON-OFF-ESP", "1");
      mqttClient.publish("MEDIBOX-ADMIN-SCH-ESP-ON", "0");
      Serial.println("Schedule ON");
    }
  }
}

float ldrValue() {
  int ldr_1 = analogRead(LDR_1);
  int ldr_2 = analogRead(LDR_2);  

  int maxLDR = max(ldr_1, ldr_2); //get the max intensity amoung two ldr

  // Map the maxLDR value from the range (32, 4063) to (0, 1023) 
  float mappedValue = map(maxLDR, 32, 4063, 0, 1023);

  // Normalize the mapped value to the range (0, 1)
  float normalizedValue = mappedValue / 1023.0;

  return normalizedValue;
}

void updateLDR(){
  float finalLDR = ldrValue();
  String(finalLDR, 2).toCharArray(ldrAr, 5); //Convert the float to an array in order to send to node-Red
}

void rotateServo(){
  Serial.print("Min Angle: ");
  Serial.println(offsetAngle);
  Serial.print("Cotrol Factor: ");
  Serial.println(conFactor);
  int angle = 0;
  

  int ldr_1 = analogRead(LDR_1);
  int ldr_2 = analogRead(LDR_2);
  float d;  

  if(ldr_1 > ldr_2){
    d = 1.5;
  }else{
    d = 0.5;
  }

  float maxIntensity = ldrValue();

  int calc = (int)((offsetAngle * d) + (180 - offsetAngle) * maxIntensity * conFactor);

  int motorAngle = min(calc, 180);

  for(angle = 0; angle < motorAngle; angle++){
    servoMotor.write(angle);
    delay(50);
  }
}

