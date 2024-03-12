#include "AsyncRelay.hpp"


AsyncRelay::AsyncRelay(byte pin, int uuid) {
	Serial.println("new AsyncRelay");
	taskHandler = nullptr;
	this->pin = pins[pin];
	this->uuid = uuid;
}

void AsyncRelay::run(void* data) {
	char UUID[6];
	itoa(this->_delay, UUID, 10);
	const uint16_t d = this->_delay;
	Serial.print("Starting: ");
	Serial.println(UUID);

	TickType_t xDelay = d / portTICK_PERIOD_MS;
	//~ while(true) {
		//~ esp_task_wdt_reset();
		Serial.println("antes del delay");
		vTaskDelay(xDelay);
		Serial.println("despues del delay");
		//~ break;
		/*if((xTaskGetTickCount() - this->timer) > this->_delay) {
			Serial.print("{Miguel}: ");
			Serial.println(UUID);
			break;
		}*/
	//~ }

	Serial.print("Ending: ");
	Serial.println(UUID);

	//~ this->stop();
	//Wait a little before killing task
	vTaskDelete( NULL );
}

