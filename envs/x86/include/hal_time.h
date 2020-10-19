#ifndef _TIME_H_
#define _TIME_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Declarations
 ******************************************************************************/

/**
 * Initializes timers. Right now just initializes startup timer, if more are
 * added they will be initialized here. Only call once at startup.
 */
void initTimers();

/**
 * Gets time since startup. Requires initTimers() to be called at startup.
 * @return time since startup in microseconds.
 */
int timeSinceStartup();

#endif
