#include "relay.h"

Relay::Relay(byte pin,int uuid) {
	this->pin = pins[pin];
	this->uuid = uuid;
}

Relay::Relay(const Relay *r) {
	this->pin   = r->pin;
	this->uuid  = r->uuid;
	this->delay = r->delay;
	this->timer = r->timer;
}

void Relay::setDelay(int delay) {
	this->delay = delay;
}

int Relay::getuuid() {
	return this->uuid;
}

void Relay::config() {
	pinMode(this->pin,OUTPUT);
}

void Relay::set(byte state) {
	this->timer = millis();
	digitalWrite(this->pin,state);
}

bool Relay::isdone() {
	if(millis()-timer>this->delay) {
		return true;
	}

	return false;
}
