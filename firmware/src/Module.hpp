/**
 * For handling tasks in peripherals.
 */
#ifndef MODULE_INC
#define MODULE_INC

#include <Arduino.h>

class Module {
	protected:
		TaskHandle_t taskHandler;
		const int taskCore = 0;
		const int taskPriority = 1;

		int uuid = 0;

		uint16_t _delay = 0;
		unsigned long timer = 0;

		byte pin = 0;

	public:
		/**
		 * Accessed with with position index.
		 */
		static int pins[5];

		static void runTask(void* data);

		void setDelay(uint16_t delay);

		uint16_t getDelay();

		int getUuid();

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
		virtual void run(void* data) = 0; // Make run pure virtual
};

#endif
