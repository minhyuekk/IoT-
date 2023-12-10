/*
  AWS IoT WiFi

  This sketch securely connects to an AWS IoT using MQTT over WiFi.
  It uses a private key stored in the ATECC508A and a public
  certificate for SSL/TLS authetication.

  It publishes a message every 5 seconds to arduino/outgoing
  topic and subscribes to messages on the arduino/incoming
  topic.

  The circuit:
  - Arduino MKR WiFi 1010 or MKR1000

  The following tutorial on Arduino Project Hub can be used
  to setup your AWS account and the MKR board:

  https://create.arduino.cc/projecthub/132016/securely-connecting-an-arduino-mkr-wifi-1010-to-aws-iot-core-a9f365

  This example code is in the public domain.
*/

#include <ArduinoBearSSL.h>
#include <ArduinoECCX08.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h> // change to #include <WiFi101.h> for MKR1000
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <PulseSensorPlayground.h>

#include "arduino_secrets.h"
  
#include "DHT.h"
#include "Pan.h"
#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
#define PAN_RELAY 4

DHT dht(DHTPIN, DHTTYPE);
Pan pan1(PAN_RELAY);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
byte samplesUntilReport;
// PulseSensorPlayground pulseSensor;


/////// Enter your sensitive data in arduino_secrets.h
const char ssid[]        = SECRET_SSID;
const char pass[]        = SECRET_PASS;
const char broker[]      = SECRET_BROKER;
const char* certificate  = SECRET_CERTIFICATE;

const int LED = LED_BUILTIN;          // The on-board Arduino LED, close to PIN 13.

WiFiClient    wifiClient;            // Used for the TCP socket connection
BearSSLClient sslClient(wifiClient); // Used for SSL/TLS connection, integrates with ECC508
MqttClient    mqttClient(sslClient);

unsigned long fanOffStartTime = 0;  // 팬을 끈 시작 시간
const unsigned long fanOffDuration = 60000;  // 팬을 끄는 기간 (여기서는 60,000 밀리초 또는 60초)
bool isFanOn = false;  // 팬 상태 추적

unsigned long lastMillis = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  // if (!ECCX08.begin()) {
  //   Serial.println("No ECCX08 present!");
  //   while (1);
  // }

  // // // Set a callback to get the current time
  // // // used to validate the servers certificate
  ArduinoBearSSL.onGetTime(getTime);

  // // // Set the ECCX08 slot to use for the private key
  // // // and the accompanying public certificate for it
  sslClient.setEccSlot(0, certificate);

  // // // Optional, set the client id used for MQTT,
  // // // each device that is connected to the broker
  // // // must have a unique client id. The MQTTClient will generate
  // // // a client id for you based on the millis() value if not set
  // // //
  // // mqttClient.setId("clientId");

  // // // Set the message callback, this function is
  // // // called when the MQTTClient receives a message
  mqttClient.onMessage(onMessageReceived);

  dht.begin();
  mlx.begin();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqttClient.connected()) {
    // MQTT client is disconnected, connect
    connectMQTT();
  }

  // poll for new MQTT messages and send keep alives
  mqttClient.poll();

  float t = dht.readTemperature();
  int p = analogRead(A0) / 10;

  // publish a message roughly every 5 seconds.
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    char payload[512];
    getDeviceStatus(payload);
    // Serial.println(payload);
    sendMessage(payload);
  }

  // 현재 팬 상태 확인
  bool isPanOn = pan1.getState() == PAN_ON;
  // 실내 온도가 23.0도 미만이거나 심박도가 90이 넘어가면 pan off
  if (t < 23.0 || p > 90) {
    if (isPanOn) {
      // 팬을 켜놓고 60초 동안 온도가 23도 미만이거나 심박수가 90을 넘으면 팬을 끔
      if (millis() - fanOffStartTime > fanOffDuration) {
        pan1.off();
        fanOffStartTime = 0;  // 팬이 꺼졌으므로 초기화
        isFanOn = false;
      } else {
      }
    }
  } else {
    if (!isFanOn) {
      pan1.on();
      fanOffStartTime = millis();  // 팬을 다시 켰으므로 시작 시간 기록
      isFanOn = true;
    }
  }
}

unsigned long getTime() {
  // get the current time from the WiFi module  
  return WiFi.getTime();
}

void connectWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.print(ssid);
  Serial.print(" ");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println();

  Serial.println("You're connected to the network");
  Serial.println();
}

void connectMQTT() {
  Serial.print("Attempting to MQTT broker: ");
  Serial.print(broker);
  Serial.println(" ");

  while (!mqttClient.connect(broker, 8883)) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println();

  Serial.println("You're connected to the MQTT broker");
  Serial.println();

  // subscribe to a topic
  mqttClient.subscribe("$aws/things/MyMKRWiFi1010/shadow/update/delta");
}

void getDeviceStatus(char* payload) {
  // Read temperature as Celsius (the default)
  float bt = mlx.readAmbientTempC();
  float t = dht.readTemperature();
  // Read cooling pan status
  const char* pan = (pan1.getState() == PAN_ON)? "ON" : "OFF";
  int p = analogRead(A0) / 10;

  // make payload for the device update topic ($aws/things/MyMKRWiFi1010/shadow/update)
  sprintf(payload,"{\"state\":{\"reported\":{\"temperature\":\"%0.2f\",\"body_temperature\":\"%.2f\",\"pulse\":\"%d\",\"PAN\":\"%s\"}}}",t,bt,p,pan);
}

void sendMessage(char* payload) {
  char TOPIC_NAME[]= "$aws/things/MyMKRWiFi1010/shadow/update";
  
  Serial.print("Publishing send message:");
  Serial.println(payload);
  mqttClient.beginMessage(TOPIC_NAME);
  mqttClient.print(payload);
  mqttClient.endMessage();
}


void onMessageReceived(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // store the message received to the buffer
  char buffer[512] ;
  int count=0;
  while (mqttClient.available()) {
     buffer[count++] = (char)mqttClient.read();
  }
  buffer[count]='\0'; // 버퍼의 마지막에 null 캐릭터 삽입
  Serial.println(buffer);
  Serial.println();

  // JSon 형식의 문자열인 buffer를 파싱하여 필요한 값을 얻어옴.
  // 디바이스가 구독한 토픽이 $aws/things/MyMKRWiFi1010/shadow/update/delta 이므로,
  // JSon 문자열 형식은 다음과 같다.
  // {
  //    "version":391,
  //    "timestamp":1572784097,
  //    "state":{
  //        "LED":"ON"
  //    },
  //    "metadata":{
  //        "LED":{
  //          "timestamp":15727840
  //         }
  //    }
  // }
  //
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, buffer);
  JsonObject root = doc.as<JsonObject>();
  JsonObject state = root["state"];
  const char* pan = state["PAN"];
  Serial.println(pan);
  
  char payload[512];
  
  if (strcmp(pan,"ON")==0) {
    pan1.on();
    sprintf(payload,"{\"state\":{\"reported\":{\"PAN\":\"%s\"}}}","ON");
    sendMessage(payload);
    
  } else if (strcmp(pan,"OFF")==0) {
    pan1.off();
    sprintf(payload,"{\"state\":{\"reported\":{\"PAN\":\"%s\"}}}","OFF");
    sendMessage(payload);
  }
 
}
