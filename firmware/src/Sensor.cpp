#include "Sensor.hpp"

#include <ArduinoJson.h>

Sensor::Sensor(byte pin, int uuid) : Module(pin, uuid) {

}

void Sensor::run(void * data) {
	int uuid = this->getUuid();
	const uint16_t d = this->_delay;

	const int potPin = 13;

	// Reading potentiometer value
	int potValue = analogRead(potPin);

	JsonDocument doc;
	doc["cmd"] = "status";
	doc["data"]["type"] = "Sensor";
	doc["data"]["uuid"] = uuid;
	doc["data"]["id"] = 2;
	doc["data"]["value"] = potValue;
	doc["data"]["status"] = "ok";

	serializeJson(doc, Serial);

	vTaskDelete( NULL );
}
