#include "Module.hpp"

int Module::pins[5] = {2, 4, 13, 14, 15};

void Module::runTask(void *_this) {
	Module * module = (Module *) _this;
	module->run(_this);
	//~ (breakpoint);
	//~ asyncRelay->stop();
}

void Module::setDelay(uint16_t delay) {
	this->_delay = delay;
}

uint16_t Module::getDelay() {
	return this->_delay;
}

int Module::getUuid(){
	return this->uuid;
}

void Module::set(byte state) {
	Serial.println("Setting time: ");
	char text[5];
	this->timer = xTaskGetTickCount();
	sprintf(text, "%04d", this->timer);
	Serial.println(text);
	digitalWrite(this->pin, state);
}

void Module::start(void) {
	if(this->taskHandler != nullptr) {
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


void Module::stop() {
	if(this->taskHandler == nullptr) {
		return;
	}

	xTaskHandle temp = this->taskHandler;
	taskHandler = nullptr;
	vTaskDelete(temp);
}
