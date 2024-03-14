#ifndef MOTOR_INC
#define MOTOR_INC

#include <Arduino.h>

#include "Module.hpp"

class Motor : public Module {
	public:
		Motor(byte pin, int uuid);

		void run(void* data);
};

#endif
