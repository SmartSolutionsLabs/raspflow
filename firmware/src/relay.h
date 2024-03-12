#include <Arduino.h>

#ifndef RELAY_INC
#define RELAY_INC

class Relay {
	private:
		byte pins[5]={2,4,13,14,15};
		byte pin = 0;
		int uuid = 0;
		int delay = 0;
		unsigned long timer = 0;

	public:
		Relay(byte pin,int uuid);

		Relay(const Relay *r);

		void setDelay(int delay);

		int getuuid();

		void config();

		void set(byte state);

		bool isdone();
};

#endif
