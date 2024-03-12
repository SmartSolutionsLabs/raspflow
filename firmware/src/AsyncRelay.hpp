#ifndef ASYNC_RELAY_INC
#define ASYNC_RELAY_INC

#include <Arduino.h>

class AsyncRelay {
	private:
		TaskHandle_t taskHandler;
		const int taskCore = 0;
		const int taskPriority = 1;

		byte pins[5] = {2,4,13,14,15};
		byte pin = 0;
		int uuid = 0;
		uint16_t _delay = 0;

		unsigned long timer = 0;

		static void runTask(void* data);

	public:
		AsyncRelay(byte pin,int uuid);

		int getDelay();
		int getUuid();
		//~ unsigned long getTimer();

		void setDelay(uint16_t delay);

		void set(byte state);

		void start(void);

		void stop();

		/**
		 * @brief Body of the task to execute.
		 *
		 * This function must be implemented in the subclass that represents the actual task to run.
		 * When a task is started by calling start(), this is the code that is executed in the
		 * newly created task.
		 */
		/*virtual*/ void run(void* data) /*= 0*/; // Make run pure virtual
};

#endif
