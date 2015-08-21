#ifndef TASKS_H_
#define TASKS_H_

extern "C" {
	// Task Functions
	extern void task_100ms(void);
	extern void task_10ms(void);

	// Interrupt Service Routines (ISRs)
	extern void isr_canMsgRcv(void);
}

#endif
