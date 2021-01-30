#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

/**
 * This is a design for a state machine, mainly based on the techniques here: 
 * https://stackoverflow.com/questions/1371460/state-machines-tutorials
 * 
 * Essentially it consists of a set of defined states with an associated
 * transition function for each. When it is time to change states, these
 * functions are called and they return a value depending on their input, which
 * in turn is interpreted using a transition table. 
 * 
 * This way all the possible states and transitions between them is extremely
 * clearly layed out, which will hopefully minimize any potential confustion. 
 */

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

typedef struct stateInput_t {
    double vertVelocity;
    double vertPosition;
    /* TODO: Add actual data */
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
};

stateRet_t stateTransitionFueling(stateInput_t);
stateRet_t stateTransitionIgnition(stateInput_t);
stateRet_t stateTransitionAscent(stateInput_t);
stateRet_t stateTransitionRecovery(stateInput_t);

stateRet_t (* stateFunctions[])(stateInput_t) = {
    stateTransitionFueling, 
    stateTransitionIgnition, 
    stateTransitionAscent, 
    stateTransitionRecovery
};

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