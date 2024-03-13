#include "Relay.hpp"

#include <ArduinoJson.h>

Relay::Relay(byte pin, int uuid) {
	Serial.println("new AsyncRelay");
	taskHandler = nullptr;
	this->pin = pins[pin];
	this->uuid = uuid;
}

void Relay::run(void* data) {
	int uuid = this->getUuid();
	const uint16_t d = this->_delay;

	Serial.println("Will run UUID: ");
	Serial.println(uuid);

	JsonDocument doc;
	doc["cmd"] = "status";
	doc["data"]["type"] = "Relay";
	doc["data"]["uuid"] = uuid;
	doc["data"]["status"] = "ok";


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

	Serial.print("Still UUID: ");
	Serial.print(uuid);
	Serial.print(" with ");
	Serial.println(xDelay);

	serializeJson(doc, Serial);

	//~ this->stop();
	//Wait a little before killing task
	vTaskDelete( NULL );
}
