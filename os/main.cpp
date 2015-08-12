#include "tasks.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>
#include <tinythread.h>

class PeriodicAlarm {
public:
	PeriodicAlarm(void (*function)(void), unsigned long period_ms, unsigned long delay_ms = 0lu) :
		m_Function(function),
		m_Period_ms(period_ms),
		m_Delay_ms(delay_ms),
		m_Thread(loop, this)
	{
	}

	~PeriodicAlarm() {
		m_Thread.join();
	}

private:
	void (*m_Function)(void);
	unsigned long m_Period_ms;
	unsigned long m_Delay_ms;
	tthread::thread m_Thread;

	static void loop(void * arg);
};

void PeriodicAlarm::loop(void * arg) {
	PeriodicAlarm * alarm = reinterpret_cast<PeriodicAlarm*>(arg);
	assert(alarm != NULL);

	if (alarm->m_Delay_ms != 0) {
		usleep(1000lu * alarm->m_Delay_ms);
	}

	unsigned long time_us = 1000lu * alarm->m_Period_ms;
	unsigned long long start_us = 0LLU;
	struct timeval tv;
	if( gettimeofday(&tv, NULL) == 0 ) {
		start_us = (tv.tv_sec * 1000000llu) + (tv.tv_usec);
	}

	while (true) {
		alarm->m_Function();
		if( gettimeofday(&tv, NULL) == 0 ) {
			unsigned long long current_us = (tv.tv_sec * 1000000llu) + (tv.tv_usec);
			unsigned long elapsed_us = (current_us - start_us);
			if (elapsed_us < time_us) {
				usleep(time_us - (current_us - start_us) );
				start_us += time_us;
			} else {
				start_us += time_us * (elapsed_us / time_us);
			}
		}
	}
}

int main(int argc, const char * argv[]) {
	PeriodicAlarm periodic_alarm_100ms (&task_100ms, 100);
	PeriodicAlarm periodic_alarm_10ms  (&task_10ms,   10);

	printf("+\n");
	while (true) {
		usleep(1000000lu);
		printf("*\n");
	}
	return EXIT_SUCCESS;
}
