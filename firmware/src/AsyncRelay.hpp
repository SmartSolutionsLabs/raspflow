#ifndef ASYNC_RELAY_INC
#define ASYNC_RELAY_INC

#include <Arduino.h>

#include "Module.hpp"

class AsyncRelay : public Module {
	public:
		AsyncRelay(byte pin, int uuid);

		void run(void* data);
};

#endif
