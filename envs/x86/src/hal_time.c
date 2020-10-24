
/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "hal_time.h"
#include <time.h>

/*******************************************************************************
 * Variables
 ******************************************************************************/

static struct timespec start_time;

/*******************************************************************************
 * Implementations
 ******************************************************************************/

void initTimers() {
	clock_gettime(CLOCK_MONOTONIC, &start_time);
}

int timeSinceStartup() {
	struct timespec spec;
	clock_gettime(CLOCK_MONOTONIC, &spec);
	return spec.tv_sec*1e6 + spec.tv_nsec/1e3 - start_time.tv_sec*1e6
			- start_time.tv_nsec/1e3;
}
