#include "Motor.hpp"

#include <ArduinoJson.h>

Motor::Motor(byte pin, int uuid) : Module(pin, uuid) {

}

void Motor::run(void * data) {
	int uuid = this->getUuid();
	const uint16_t d = this->_delay;
	int pin = Module::pins[this->pin];
	int value = 1;

	digitalWrite(pin, value);

	JsonDocument doc;
	doc["cmd"] = "status";
	doc["data"]["uuid"] = uuid;
	doc["data"]["status"] = "ok";

	serializeJson(doc, Serial);

	vTaskDelete( NULL );
}
