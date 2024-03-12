#ifndef ASYNC_RELAY_INC
#define ASYNC_RELAY_INC

#include <Arduino.h>

#include "Module.hpp"

class Relay : public Module {
	public:
		Relay(byte pin, int uuid);

		void run(void* data);
};

#endif
