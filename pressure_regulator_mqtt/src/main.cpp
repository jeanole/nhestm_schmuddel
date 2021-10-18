#include <Arduino.h>
#include <AIO.h>
#include "DAC.h"
#include <JC_Button.h>
#include <Ethernet.h>
#include <MQTT.h>

using namespace AIO;

byte mac[] = {0x59, 0x2A, 0x28, 0x78, 0x89, 0xC8}; //59:2A:28:78:89:C8

IPAddress ip(192, 168, 100, 178);
IPAddress myDns(192,168,100,1);

EthernetClient EtherClient;
MQTTClient mqttclient;

const char *mqqt_name = "jean";
const char *mqtt_key = "public";//"schmuddi"; //(username)
const char *mqtt_secret = "public";//"5fLqmx0zcps1wYbT";
IPAddress mqtt_ip(192,168,100,155);
const int mqtt_port=  1883;
const char *mqtt_subscription_topic = "schmuddel/#";

const uint8_t DAC_CS = 48;
const uint8_t DAC_NLDAC = 47;

const uint8_t DURATION_BUTTON_PRESS = 50;

uint8_t basedeviderconst = 100;

DAC dac;

//sweep
float sweep_maxvalue = 200;
float sweep_minvalue = 0;
int sweep_duration = 1000; //ms
float sweep_currentflow = 0;
float startTime = 0;
float currentSetPressure = 0;

void set_regulator(float percent, float basedevider=1024)
{
    uint16_t outvalue = min(percent / basedevider * 4095, 4095);

    dac.set_value(DAC::CHANNEL_A, outvalue, DAC::GAIN_2X);
    dac.sync_ldac();

    currentSetPressure = percent;

    mqttclient.publish("schmuddel/pressureregulator/setval", String(percent));
    mqttclient.publish("schmuddel/pressureregulator/base", String(basedevider));
    Serial.print("set regulator to: ");
    Serial.print(outvalue );
    Serial.print( "@basedevider: ");
    Serial.println( basedevider);

}

void messageReceived(String &topic, String &payload) {
    Serial.println("got message:" + topic+ " -> " + payload);

    if(topic =="schmuddel/setregulatorvalue"){
        set_regulator(payload.toInt());
    }
    else if(topic == "schmuddel/setregulatorbase"){
       basedeviderconst = payload.toInt();
    }
    else if(topic == "schmuddel/sweep")
      {
        sweep(1);
        //Serial.println("couldnt interprete message:" + topic+ " -> " + payload);
    }
    else if(topic == "schmuddel/sweep/maxvalue"){
      sweep_maxvalue = payload.toFloat();
    }
    else if(topic == "schmuddel/sweep/minvalue"){
      sweep_minvalue = payload.toFloat();
    }
    else if(topic == "schmuddel/sweep/duration"){
      sweep_duration = payload.toInt();
    else if(topic == "schmuddel/sweep/currentflow"){
      sweep_currentflow = payload.toFloat();
      sweep();
    }

}

void sweep(int start = 0){
  if(start = 1){
    startTime = millis();

  }
   if(sweep_currentflow < sweep_minvalue){
     increasepressure();
   }
   else if(sweep_currentflow > sweep_maxvalue){
     godown = true;
     decreasepressure();
   }
   else if(sweep_currentflow < sweep_minvalue && godown){
     godown = false;
     increasepressure();
   }else if(sweep_currentflow< sweep_maxvalue && sweep_currentflow> sweep_minvalue){
     if(godown){
       decreasepressure();
     }else{
       increasepressure();
     }
   }
}

void decreasepressure(){
  float factor = (sweep_maxvalue - sweep_minvalue)/sweep_duration;
  set_regulator(currentSetPressure + factor)
}

void increasepressure(){
  float factor = (sweep_maxvalue - sweep_minvalue)/sweep_duration;
  set_regulator(currentSetPressure - factor)
}

void connect(){
  Serial.println("connecting...");
  while (!mqttclient.connect(mqqt_name)){
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\n connected!");
  Serial.print("subscribing to topic ");
  Serial.println(mqtt_subscription_topic);
  while(!mqttclient.subscribe(mqtt_subscription_topic)){
    Serial.print('#');
  }
}

void setup()
{
    Serial.begin(115200);
//init
    baseboard_init();
    dac.begin(DAC_CS, DAC_NLDAC);

//start Ethernet
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

//start mqtt client
    mqttclient.begin(mqtt_ip,mqtt_port, EtherClient);
    mqttclient.onMessage(messageReceived);
    connect();

    Serial.println("Started");

}


void loop()
{
  mqttclient.loop();
  if(!mqttclient.connected()){
    connect();
  }
    if (false)//check_button())
    {
        for(uint8_t i = 0; i<4; ++i)
        {
           Serial.println(i);
        }
    }
}
