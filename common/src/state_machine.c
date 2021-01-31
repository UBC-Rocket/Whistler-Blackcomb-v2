
/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "state_machine.h"


/*******************************************************************************
 * Declarations
 ******************************************************************************/

/**
 * A helper struct representing a single transition, going from srcState to
 * dstState when retCode is returned from a transition function.
 */
struct stateTransition
{
    state_t srcState;
    stateRet_t retCode;
    state_t dstState;
};

/**
 * State transition table. This is extremely important, all possible changes of 
 * state originated from this table. 
 * 
 * If a state change isn't in this table it IT SHOULD NOT HAPPEN. If it ever 
 * does then something has gone very wrong. 
 * 
 * Also note that there are slightly more efficient ways of expressing this 
 * table (O(1) instead of O(n) for lookup), but generally it's small enough
 * that we don't have to worry about it
 */
static struct stateTransition stateTransitions[] = {
    {stateFueling, stateRetPass, stateIgnition},
    {stateFueling, stateRetRepeat, stateFueling},
    {stateIgnition, stateRetRepeat, stateIgnition}
    /* TODO: continue once we have actual states */
};

/**
 * Declarations for all of the state transition functions. 
 * 
 * These functions are declared here because they are directly put into the
 * stateFunctions variable and should never again be referenced by name. 
 * 
 * @param input the state input that will be used to compute the next state
 * @return the stateRet_t that represents the outcome of the state transition
 */
static stateRet_t stateTransitionError(stateInput_t *input);
static stateRet_t stateTransitionFueling(stateInput_t *input);
static stateRet_t stateTransitionIgnition(stateInput_t *input);
static stateRet_t stateTransitionAscent(stateInput_t *input);
static stateRet_t stateTransitionRecovery(stateInput_t *input);


/**
 * Lookup function to get next state
 * 
 * @param state current state
 * @param retCode the return code given by transition function
 * @return the next state to transition to
 */
static state_t stateLookup(state_t state, stateRet_t retCode);


/*******************************************************************************
 * Variables
 ******************************************************************************/

/* The current state, possibly the most important variable in the project */
static state_t curState = stateFueling;
/**
 * Array of state transition function pointers. 
 */
stateRet_t (*stateFunctions[])(stateInput_t*) = {
    stateTransitionError,
    stateTransitionFueling,
    stateTransitionIgnition,
    stateTransitionAscent,
    stateTransitionRecovery};

/*******************************************************************************
 * Implementations
 ******************************************************************************/

static state_t stateLookup(state_t state, stateRet_t retCode){
    for(long unsigned int i = 0; i < sizeof(stateTransitions) / sizeof(stateTransitions[0]); ++i){
        if(stateTransitions[i].srcState == state && stateTransitions[i].retCode == retCode){
            return stateTransitions[i].dstState;
        }
    }
    return stateError;
}

state_t getState(void){
    return curState;
}

state_t setNextState(stateInput_t *input){
    stateRet_t retCode = stateFunctions[curState](input);    
    curState = stateLookup(curState, retCode);
    return curState;
}

static stateRet_t stateTransitionError(stateInput_t *input){
    return stateRetRepeat;
}

static stateRet_t stateTransitionFueling(stateInput_t *input){
    return stateRetPass;
}

static stateRet_t stateTransitionIgnition(stateInput_t *input){
    return stateRetRepeat;
}

static stateRet_t stateTransitionAscent(stateInput_t *input){
    return stateRetRepeat;
}

static stateRet_t stateTransitionRecovery(stateInput_t *input){
    return stateRetRepeat;
}


