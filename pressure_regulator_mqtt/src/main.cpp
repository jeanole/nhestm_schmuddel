#include <Arduino.h>
#include <AIO.h>
#include "DAC.h"
#include <JC_Button.h>

using namespace AIO;

const uint8_t DAC_CS = 48;
const uint8_t DAC_NLDAC = 47;
const uint8_t POT_PIN = P8;
const uint8_t WASH_PIN = P7;

const uint8_t TRIGGER_1 = P6;
const uint8_t TRIGGER_2 = P5;
const uint8_t TRIGGER_3 = P4;
const uint8_t TRIGGER_4 = P3;

const uint8_t VALVE_1 = X1;
const uint8_t VALVE_2 = X2;
const uint8_t VALVE_3 = X3;
const uint8_t VALVE_4 = X4;

const uint8_t valves[] = {VALVE_3, VALVE_4,VALVE_1, VALVE_2};
const uint8_t DURATION_BUTTON_PRESS = 50;


DAC dac;
Button trigger_1(TRIGGER_1);
Button trigger_2(TRIGGER_2);
Button trigger_3(TRIGGER_3);
Button trigger_4(TRIGGER_4);

Button trigger[] = {trigger_1, trigger_2, trigger_3, trigger_4};
Button button(WASH_PIN);

void set_reg1024(float percent)
{
    uint16_t outvalue = min(percent / 1024.0 * 4095, 4095);

    dac.set_value(DAC::CHANNEL_A, outvalue, DAC::GAIN_2X);
    dac.sync_ldac();
     Serial.print("REG_in=");
     Serial.println(percent);
}

void set_regulator(float percent)
{
    uint16_t outvalue = min(percent / 100.0 * 4095, 4095);

    dac.set_value(DAC::CHANNEL_A, outvalue, DAC::GAIN_2X);
    dac.sync_ldac();
    // Serial.print("REG=");
    // Serial.println(outvalue);
}

void unclog()
{
    set_regulator(100);
    delay(1000);

    // Unclogger
    digitalWrite(VALVE_1, HIGH);
    digitalWrite(VALVE_2, HIGH);
    digitalWrite(VALVE_3, HIGH);                                                                  
    digitalWrite(VALVE_4, HIGH);
    delay(150);
    digitalWrite(VALVE_1, LOW);
    digitalWrite(VALVE_2, LOW);
    digitalWrite(VALVE_3, LOW);
    digitalWrite(VALVE_4, LOW);
    delay(200);
}

void handtrigger(){
    int timestamp = 0;
    int potVal = analogRead(POT_PIN);

    if(trigger[0].isPressed()){
        timestamp = millis();
        for(int n=0;n<4;n++){
            digitalWrite(valves[n],HIGH);
        }
        Serial.println("//start//");
        while(millis()-timestamp <= 5000000){
            potVal = analogRead(POT_PIN);
            potVal = map(potVal,0,1023,0,50);
            set_regulator(potVal);
            Serial.println(potVal);
        }
        Serial.println("//stop//");
        for(int n=0;n<4;n++){
            digitalWrite(valves[n],LOW);
        }
    }

    for(int i=0; i<4; i++){
         potVal = analogRead(POT_PIN);
        potVal = map(potVal,0,1023,0,100);
        set_regulator(potVal);
       // Serial.println(i+" --> "+potVal);

        trigger[i].read();
        if(trigger[i].isPressed()){
            digitalWrite(valves[i],HIGH); 
        }else{
            digitalWrite(valves[i], LOW);
        }
    }
}

void sequence(uint8_t valve)
{
    static uint16_t lower_threshold = 0;
    static uint16_t higher_threshold = 0;

    digitalWrite(valve, LOW);

    //injected by sps
    set_regulator(100);
    delay(200);
    digitalWrite(valve, HIGH);
    delay(90);
    digitalWrite(valve, LOW);
    delay(3000);
    set_regulator(50);

    //digitalWrite(valve, HIGH);

    for (uint8_t i=0 ; i < 20 ; ++i) {
        // lower_threshold = analogRead(P8);
        // lower_threshold = map(lower_threshold, 0, 1023, 0, 40);
        // higher_threshold = abs(lower_threshold + 20);
        // Serial.print(lower_threshold);
        // Serial.print(" - ");
        // Serial.println(higher_threshold);
        // set_regulator(random(lower_threshold, higher_threshold));
        // delay(3000);
         delay(200);
        digitalWrite(valve, HIGH);
        delay(90);
        digitalWrite(valve, LOW);
        delay(2000);
    }
    digitalWrite(valve, LOW);
}


void inject_bubble(uint8_t valve, uint16_t duration)
{
        digitalWrite(valve, HIGH);
        delay(duration);
        digitalWrite(valve, LOW);
}

void flush_and_clean(uint16_t duration, uint8_t pressure, uint8_t valve)
{
    set_regulator(100);
    inject_bubble(valves[valve],100);
    delay(200);
    inject_bubble(valves[valve],100);
    delay(200);
    set_regulator(pressure);
    inject_bubble(valves[valve], duration);
    delay(200);
}

void setup()
{
    Serial.begin(115200);

    baseboard_init();
    pinMode(X1, OUTPUT);
    pinMode(X2, OUTPUT);
    pinMode(X3, OUTPUT);
    pinMode(X4, OUTPUT);
    pinMode(X5, OUTPUT);
    pinMode(X6, OUTPUT);
    pinMode(X7, OUTPUT);
    pinMode(X8, OUTPUT);

    dac.begin(DAC_CS, DAC_NLDAC);
    button.begin();
    trigger_1.begin();
    trigger_2.begin();
    trigger_3.begin();
    trigger_4.begin();

    Serial.println("Started");

}

void leopard_pattern(uint8_t valve)
{
   
    set_regulator(90);
    delay(100);
    inject_bubble(valve,150);
    set_regulator(40);
    delay(100);
    inject_bubble(valve,100);
            int potVal = analogRead(POT_PIN);
        potVal = map(potVal,0,1023,0,100);
        set_regulator(potVal);
//        Serial.println(" --> " + potVal);

//    set_regulator(40);
    delay(100);
    inject_bubble(valve, 10000);
    //delay(3000);

    
}


void colon_pattern(uint8_t valve)
{
    set_regulator(100);
    delay(100);
    inject_bubble(valve, 200);
    set_regulator(80);
    inject_bubble(valve, 4000);
    delay(1000);
}


bool check_button()
{
    button.read();

    if (button.isPressed()) {
        Serial.println("Button pressed");
        return true;
    }
    return false;
}

void ausblasgang(){
set_regulator(80);
delay(40000);
set_regulator(0);
delay(20000);

    for(float i = 0;i<=10;i++){
        set_reg1024(8*i);
        delay(1000);
    }
    for(float i = 100;i<=350;i=i+0.1){
        set_reg1024(i);
        delay(1000);
    }
}

void loop()
{
    Serial.println("bla");
    // uint8_t idx = random(4);

    // unclog();

    // sequence(valves[idx]);
    // delay(1000);

    // for(uint8_t i = 0; i<4; ++i)
    // {
    //     inject_bubble(valves[i], 250);
    //     inject_bubble(valves[i], 250);
    //     inject_bubble(valves[i], 250);
    //     inject_bubble(valves[i], 250);
    //     flush_and_clean(30000, 100,valves[i]);
    //     colon_pattern(valves[i]);
    // }
    
    
    //
    
    if (false)//check_button())
    {
        for(uint8_t i = 0; i<4; ++i)
        {
           //flush_and_clean(8000, 100, i);
           Serial.println(i);
        }
    }
    // for(float i=0;i<60;i=i+0.1){
    //     set_regulator(i);
    //     Serial.println(i);
    //     delay(2000);
    // }
handtrigger();
ausblasgang();
/*
    uint8_t valve = valves[0];
    
    for(uint8_t i=0; i<4; ++i)
    {
        valve = valves[i];
        //colon_pattern(valve);
        //delay(4000);
        leopard_pattern(valve);
        delay(2500);
       
    }
*/
}
