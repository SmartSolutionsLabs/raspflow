#include "AsyncRelay.hpp"


AsyncRelay::AsyncRelay(byte pin, int uuid) {
	Serial.println("new AsyncRelay");
	taskHandler = nullptr;
	this->pin = pins[pin];
	this->uuid = uuid;
}

void AsyncRelay::setDelay(uint16_t delay) {
	this->_delay = delay;
}

int AsyncRelay::getDelay() {
	return this->_delay;
}

void AsyncRelay::set(byte state) {
	Serial.println("Setting time: ");
	char text[5];
	this->timer = xTaskGetTickCount();
	sprintf(text, "%04d", this->timer);
	Serial.println(text);
	digitalWrite(this->pin, state);
}

int AsyncRelay::getUuid(){
	return this->uuid;
}

void AsyncRelay::start(void) {
	if(taskHandler != nullptr) {
		Serial.println("Task::start - There might be a task already running!");
	}

	char task_uuid[4];
	itoa(uuid, task_uuid,10);
	Serial.print("Iniciando: ");
	Serial.println(task_uuid);
	//Start Task with input parameter set to "this" class
	xTaskCreatePinnedToCore(
		&runTask,          //Function to implement the task
		task_uuid,         //Name of the task
		15000,              //Stack size in words
		this,              //Task input parameter
		this->taskPriority,//Priority of the task
		&taskHandler,      //Task handle.
		this->taskCore);   //Core where the task should run
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
		break;
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

void AsyncRelay::stop() {
	if(taskHandler == nullptr) {
		return;
	}

	xTaskHandle temp = taskHandler;
	taskHandler = nullptr;
	vTaskDelete(temp);
}

void AsyncRelay::runTask(void *_this) {
	AsyncRelay * asyncRelay = (AsyncRelay *) _this;
	asyncRelay->run(_this);
	//~ (breakpoint);
	//~ asyncRelay->stop();
}
