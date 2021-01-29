#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/


/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef enum state_t {
    stateFueling, 
    stateIgnition, 
    stateAscent, 
    stateRecovery
};

typedef enum stateRet_t {
    stateRetPass, 
    stateRetRepeat, 
    stateRetAbort
};

struct transition {
    state_t srcState;
    stateRet_t retCode;
    state_t dstState;
};

struct transition stateTransitions[] = {
    {stateFueling, stateRetPass, stateIgnition}, 
    {stateFueling, stateRetRepeat, stateFueling}, 
    /* TODO: continue once we have actual states */
}

/*******************************************************************************
 * Declarations
 ******************************************************************************/

/**
 * Locks the state machine for thread safety. This function should be called
 * before any operations on the state machine are comitted 
 */
void stateLock();

void stateUnlock();

#endif