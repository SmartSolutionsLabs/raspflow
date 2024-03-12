#include <Arduino.h>
class Relay{
  private:
    byte pins[5]={2,4,13,14,15};
    byte pin = 0;
    int uuid = 0; 
    int delay = 0;
    unsigned long timer = 0;
  public:
    Relay(byte pin,int uuid) {
      this->pin = pins[pin];
      this->uuid = uuid;
    }
    Relay operator=(const Relay *r) {
         this->pin   = r->pin;
         this->uuid  = r->uuid;
         this->delay = r->delay;
         this->timer = r->timer;
      }
    void setDelay(int delay){
      this->delay = delay;
    }
    int getuuid(){
      return this->uuid;
    }
    void config(){
      pinMode(this->pin,OUTPUT);
    }
    void set(byte state){
      this->timer = millis();
      digitalWrite(this->pin,state);
    }
    bool isdone(){
      if(millis()-timer>this->delay){
        return true;
      }
      return false;
    }
};