#include <ArduinoJson.h>
#import "Relay.hpp"

#define CONNECTION 0
#define DO 1
#define READ 2
#define STATUS 3

#define END 0
#define START 1
#define RELAY 2
#define MOTOR 3
#define SENSOR 4

bool firstConnection = false;
StaticJsonDocument<512> req;
StaticJsonDocument<512> res;
bool first_Connection = false;
int command;
int realtime;
int uuid_delay_size = 1;
int *uuid_delay = new int[uuid_delay_size];
int RelayPin[5] = {2,4,14,15,16};
int RelayDelay;
unsigned long RelayTimer;
int id,r_uuid;

void setup() {
  pinMode(15,OUTPUT);
  pinMode(2,OUTPUT);
  digitalWrite(15,LOW);
  Serial.begin(115200);
  Serial.flush();
  realtime = millis();
}

int size_ = 0,tipo;
String payload;
bool exist = false;
unsigned long randomtime =0 ;
int tr = 0;

void loop() {
  vTaskDelay(1);
  if(Serial.available()){
    payload = Serial.readStringUntil('\n');
    DeserializationError   error = deserializeJson(req, payload);
    if (error) {
      //Serial.println(error.c_str());
      return;
    }
    realtime = millis();
    command = req["cmd"];
    //Serial.println(command);
    switch(command){
    case CONNECTION:
        res.clear();
        res["cmd"] = 0;
        digitalWrite(2,!digitalRead(2));
        serializeJson(res, Serial);
        Serial.println();
        res.clear();
        //first_Connection = true;
      break;
    case DO:
        tipo = req["data"]["type"];
        switch (tipo){
            case END:
                res.clear();
                res["cmd"]  = "status";
                res["data"]["type"] = req["data"]["type"];
                res["data"]["uuid"] = req["data"]["uuid"];
                res["data"]["status"] = "ok";
                serializeJson(res, Serial);
                Serial.println();
                req.clear();req.clear();
                break;
            case START:
                res.clear();
                res["cmd"]  = "status";
                res["data"]["type"] = req["data"]["type"];
                res["data"]["uuid"] = req["data"]["uuid"];
                res["data"]["status"] = "ok";
                serializeJson(res, Serial);
                Serial.println();
                req.clear();
                break;
            case RELAY: {
                id = req["data"]["id"];
                RelayDelay =req["data"]["delay"];
                r_uuid = req["data"]["uuid"];
                  Relay relay(id,r_uuid);
                  relay.set(req["data"]["set"]);
                  relay.setDelay(req["data"]["delay"]);
                  relay.start();
                req.clear();
                break;
			}
            case MOTOR:
                if(req["data"]["set"] != ""){
                  id = req["data"]["id"];
                  digitalWrite(RelayPin[id],req["data"]["set"]);
                  res.clear();
                  res["cmd"]  = "status";
                  res["data"]["type"] = req["data"]["type"];
                  res["data"]["uuid"] = req["data"]["uuid"];
                  res["data"]["status"] = "ok";
                  serializeJson(res, Serial);
                  Serial.println();
                  req.clear();
                }
                break;
            default:
                res.clear();
                res["cmd"]  = "status";
                res["data"]["type"] = "null";
                res["data"]["uuid"] = 0;
                res["data"]["status"] = "pruebalo pues";
                serializeJson(res, Serial);
                Serial.println();
                req.clear();
                break;
            }
      break;

      case READ:
          tipo = req["data"]["type"];
          if(req["data"]["type"] == SENSOR){
            if(req["data"]["id"] == 1){
              if(req["data"]["set"] != ""){
                digitalWrite(2,req["data"]["set"]);
                res.clear();
                res["cmd"]  = "status";
                res["data"]["type"] = req["data"]["type"];
                res["data"]["uuid"] = req["data"]["uuid"];
                res["data"]["status"] = "ok";
                serializeJson(res, Serial);
                Serial.println();
                req.clear();
              }
            }
          }
        break;
      default:
        res.clear();
        res["cmd"]  = STATUS;
        res["data"]["type"] = "default";
        res["data"]["status"] = "WRONG COMMAND";
        serializeJson(res, Serial);
        Serial.println();
        req.clear();
        break;
      }
   }
}

//{"cmd":1,"data":{"type":"Start","uuid":1}}
