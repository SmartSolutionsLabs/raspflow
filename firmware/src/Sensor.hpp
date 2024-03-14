#ifndef SENSOR_INC
#define SENSOR_INC

#include <Arduino.h>

#include "Module.hpp"

class Sensor : public Module {
	public:
		Sensor(byte pin, int uuid);

		/**
		 * Read analog port and send data out.
		 */
		void run(void * data);
};

#endif
