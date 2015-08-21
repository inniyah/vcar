#ifndef HOOKS_H_
#define HOOKS_H_

#ifdef __cplusplus
extern "C" {
#endif

	// Task Functions
	extern void task_100ms(void);
	extern void task_10ms(void);

	// Interrupt Service Routines (ISRs)
	extern void isr_canMsgReceived(void);
	extern void isr_canMsgSent(void);

#ifdef __cplusplus
}
#endif

#endif
