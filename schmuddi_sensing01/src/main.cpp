// https://github.com/256dpi/arduino-mqtt/blob/master/examples/ArduinoEthernetShield/ArduinoEthernetShield.ino

#include <Arduino.h>
#include <AIO.h>
#include <Ethernet.h>
#include <MQTT.h>
#include <ArduinoJson.h>

using namespace AIO;

const uint8_t P_PIN = A0;
const uint8_t FLOW_PIN = A1;

const uint8_t VALVE_1 = X1;
const uint8_t VALVE_2 = X2;
const uint8_t VALVE_3 = X3;
const uint8_t VALVE_4 = X4;

boolean valveState[4] ={};

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

IPAddress ip(192, 168, 100, 177);
IPAddress myDns(192,168,100,1);

EthernetClient EtherClient;
MQTTClient client;

unsigned long lastMillis = 0;

uint16_t pressureValue = 0;
uint16_t flowValue = 0;

const char *mqqt_name = "jeandino";
const char *mqtt_key = "public";//"schmuddi"; //(username)
const char *mqtt_secret = "public";//"5fLqmx0zcps1wYbT";
IPAddress mqtt_ip(192,168,100,155);
const int mqtt_port=  1883;
const char *mqtt_subscription_topic = "schmuddel/#";


void connect() {
  Serial.print("connecting...");
  while (!client.connect(mqqt_name)) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

Serial.print("subscribing to topic: ");
Serial.println(mqtt_subscription_topic);
while(!client.subscribe(mqtt_subscription_topic)){
  Serial.print("#");
}
  // client.unsubscribe("/hello");
  Serial.println("connecting done");
}

void messageReceived(String &topic, String &payload) {
  //Serial.println("incoming: " + topic + " - " + payload);
  if(topic == "schmuddel/r1"){
    if(payload=="true"){
      Serial.println("R1_on");
      digitalWrite(VALVE_1,HIGH);
      valveState[0]=true;
      client.publish("schmuddel/relais/R1", String(1024));
    }else if (payload =="false")
    {
      digitalWrite(VALVE_1,LOW);
      valveState[0]=false;
      client.publish("schmuddel/relais/R1", String(0));
      Serial.print("R1_off: ");
      Serial.println(payload);
    } 
  }
  if(topic == "schmuddel/r2"){
    if(payload=="true"){
      Serial.println("R2_on");
      digitalWrite(VALVE_2,HIGH);
      valveState[1]=true;
      client.publish("schmuddel/relais/R2", String(1024));
    }else if (payload =="false")
    {
      digitalWrite(VALVE_2,LOW);
      valveState[1]=false;
      client.publish("schmuddel/relais/R2", String(0));
      Serial.print("R2_off: ");
      Serial.println(payload);
    } 
  }
  if(topic == "schmuddel/r3"){
    if(payload=="true"){
      Serial.println("R3_on");
      digitalWrite(VALVE_3,HIGH);
      valveState[2]=true;
      client.publish("schmuddel/relais/R3", String(1024));
    }else if (payload =="false")
    {
      digitalWrite(VALVE_3,LOW);
      valveState[2]=false;
      client.publish("schmuddel/relais/R3", String(0));
      Serial.print("R3_off: ");
      Serial.println(payload);
    } 
  }
  if(topic == "schmuddel/r4"){
    if(payload=="true"){
      Serial.println("R4_on");
      digitalWrite(VALVE_4,HIGH);
      valveState[3]=true;
      client.publish("schmuddel/relais/R4", String(1024));
    }else if (payload =="false")
    {
      digitalWrite(VALVE_4,LOW);
      valveState[1]=false;
      client.publish("schmuddel/relais/R4", String(0));
      Serial.print("R1_off: ");
      Serial.println(payload);
    } 
  }

}

void setup() {
  Serial.begin(115200);
  
 Serial.println("Initialize Ethernet with DHCP:");
 Ethernet.init(ETH_CS_PIN);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
client.begin(mqtt_ip,mqtt_port, EtherClient);
  client.onMessage(messageReceived);

  connect();

}

String bolsch(boolean input){
if (input == true)
{
  return "true";
}else{
  return "false";
}
}

void loop() { 
  //Serial.println("bummsnase");
  pressureValue = analogRead(P_PIN);
  flowValue = analogRead(FLOW_PIN);
//json
String pressureVal = String(pressureValue);
pressureVal = " \"pressure\" : " + pressureVal + ", ";
String flowVal = String(flowValue);
flowVal = " \"flow\" : " + flowVal + ", ";
String r1 = " \"valve1\" : " + String(valveState[0]) + ", ";
String r2 = " \"valve2\" : " + String(valveState[1]) + ", ";
String r3 = " \"valve3\" : " + String(valveState[2]) + ", ";
String r4 = " \"valve4\" : " + String(valveState[3]) + " ";
String buffer = "{" + pressureVal + flowVal + r1 +r2 +r3 +r4 + "}";
//json

  client.loop();
  if (!client.connected()){
    connect();
  }
  if (millis() - lastMillis > 100) {
    lastMillis = millis();
    //client.publish("schmuddel/pressure", String(pressureValue)); //
    //client.publish("schmuddel/flow", String(flowValue));
    client.publish("schmuddel/m", buffer);
    Serial.println(buffer);

  }
}