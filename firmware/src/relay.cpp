#include "Relay.hpp"

#include <ArduinoJson.h>

Relay::Relay(byte pin, int uuid) {
	Serial.println("new AsyncRelay");
	taskHandler = nullptr;
	this->pin = pins[pin];
	this->uuid = uuid;
}

void Relay::run(void* data) {
	char UUID[6];
	itoa(this->_delay, UUID, 10);
	const uint16_t d = this->_delay;

	TickType_t xDelay = d / portTICK_PERIOD_MS;
	//~ while(true) {
		//~ Serial.println("antes del delay");
		vTaskDelay(xDelay);
		//~ Serial.println("despues del delay");
		//~ break;
		/*if((xTaskGetTickCount() - this->timer) > this->_delay) {
			break;
		}*/
	//~ }

	JsonDocument doc;

	doc["cmd"]  = "status";
	doc["data"]["type"] = "Relay";
	doc["data"]["uuid"] = this->getUuid();
	doc["data"]["status"] = "ok";

	serializeJson(doc, Serial);

	//~ this->stop();
	//Wait a little before killing task
	vTaskDelete( NULL );
}
