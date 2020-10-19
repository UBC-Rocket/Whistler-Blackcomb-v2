#include <hal_time.h>

void initTimers() {
	lptmr_config_t lptmrConfig;
	LPTMR_GetDefaultConfig(&lptmrConfig);
	LPTMR_Init(STARTUP_LPTMR_BASE, &lptmrConfig);

	/* Sets the timer length to extremely high to not expire */
	/* TODO: make this actual maximum possible, not sure exactly what that is */
	LPTMR_SetTimerPeriod(STARTUP_LPTMR_BASE,
			USEC_TO_COUNT((100000000), LPTMR_SOURCE_CLOCK));
	LPTMR_StartTimer(STARTUP_LPTMR_BASE);
}

int timeSinceStartup() {
	return COUNT_TO_USEC(LPTMR_GetCurrentTimerCount(STARTUP_LPTMR_BASE),
			LPTMR_SOURCE_CLOCK);
}
