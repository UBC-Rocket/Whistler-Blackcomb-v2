
/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <state_machine.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define CLEAR 0

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
/*   Source State           Return Code     Desination State    */
    {stateStartup,          stateRetRepeat, stateStartup        },
    {stateStartup,          stateRetPass,   stateFueling        },

    {stateFueling,          stateRetRepeat, stateFueling        },
    {stateFueling,          stateRetPass,   stateStandby        },
    {stateFueling,          stateRetAbort,  stateGroundAbort    },

    {stateStandby,          stateRetRepeat, stateStandby        },
    {stateStandby,          stateRetPass,   stateIgnition       },
    {stateStandby,          stateRetRevert, stateFueling,       },
    {stateStandby,          stateRetAbort,  stateGroundAbort    },

    {stateGroundAbort,      stateRetRepeat, stateGroundAbort    },
    {stateGroundAbort,      stateRetPass,   stateFueling        },

    {stateIgnition,         stateRetRepeat, stateIgnition       },
    {stateIgnition,         stateRetPass,   statePoweredAscent  },
    {stateIgnition,         stateRetAbort,  stateGroundAbort    },
    
    {statePoweredAscent,    stateRetRepeat, statePoweredAscent  },
    {statePoweredAscent,    stateRetPass,   stateUnpoweredFlight},
    {statePoweredAscent,    stateRetAbort,  stateFlightAbort    },

    {stateUnpoweredFlight,  stateRetRepeat, stateUnpoweredFlight},
    {stateUnpoweredFlight,  stateRetPass,   stateBallutes       },

    {stateBallutes,         stateRetRepeat, stateBallutes       },
    {stateBallutes,         stateRetPass,   stateMainParachutes },

    {stateMainParachutes,   stateRetRepeat, stateMainParachutes },
    {stateMainParachutes,   stateRetPass,   stateLanded         },

    {stateFlightAbort,      stateRetRepeat, stateFlightAbort    },
    {stateFlightAbort,      stateRetPass,   stateUnpoweredFlight},

    {stateLanded,           stateRetRepeat, stateLanded         }
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
static stateRet_t stateTransitionStartup(stateInput_t *input);
static stateRet_t stateTransitionFueling(stateInput_t *input);
static stateRet_t stateTransitionStandby(stateInput_t *input);
static stateRet_t stateTransitionIgnition(stateInput_t *input);
static stateRet_t stateTransitionPoweredAscent(stateInput_t *input);
static stateRet_t stateTransitionUnpoweredFlight(stateInput_t *input);
static stateRet_t stateTransitionBallutes(stateInput_t *input);
static stateRet_t stateTransitionMainParachutes(stateInput_t *input);
static stateRet_t stateTransitionLanded(stateInput_t *input);

static stateRet_t stateTransitionGroundAbort(stateInput_t *input);
static stateRet_t stateTransitionFlightAbort(stateInput_t *input);


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
static state_t curState = stateStartup;
/**
 * Array of state transition function pointers. 
 */
stateRet_t (*stateFunctions[])(stateInput_t*) = {
    stateTransitionError,
    stateTransitionStartup,
    stateTransitionFueling,
    stateTransitionStandby,
    stateTransitionIgnition,
    stateTransitionPoweredAscent,
    stateTransitionUnpoweredFlight,
    stateTransitionBallutes,
    stateTransitionMainParachutes,
    stateTransitionLanded,
    
    stateTransitionGroundAbort,
    stateTransitionFlightAbort};

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

state_t testSetState(state_t state){
    curState=state;
    return curState;
}

static stateRet_t stateTransitionError(stateInput_t *input){
    return stateRetRepeat;
}

static stateRet_t stateTransitionStartup(stateInput_t *input){
    if(input->HMI_triggerFueling){
        input->HMI_triggerFueling = CLEAR;
        return stateRetPass;
    }
    else{
        return stateRetRepeat;
    }
    
}

static stateRet_t stateTransitionFueling(stateInput_t *input){
    if(input->HMI_triggerGroundAbort){
        input->HMI_triggerGroundAbort = CLEAR;
        return stateRetAbort;
    } 
    else if(input->HMI_triggerStandby){
        input->HMI_triggerStandby = CLEAR;
        return stateRetPass;
    } 
    else{
        return stateRetRepeat;
    }
}

static stateRet_t stateTransitionStandby(stateInput_t *input){
    if(input->HMI_triggerGroundAbort){
        input->HMI_triggerGroundAbort = CLEAR;
        return stateRetAbort;
    }
    else if(input->HMI_triggerFueling){
        input->HMI_triggerFueling = CLEAR;
        return stateRetRevert;
    }
    else if(input->GSE_triggerIgnition){
        input->GSE_triggerIgnition = CLEAR;
        return stateRetPass;
    }
    else{
        return stateRetRepeat;
    }
}

static stateRet_t stateTransitionIgnition(stateInput_t *input){
    if(input->HMI_triggerGroundAbort){
        input->HMI_triggerGroundAbort=CLEAR;
        return stateRetAbort;
    }
    //else if(1/*this will eventuall be "all good"*/){
    //    return stateRetPass;
    //}
    else{
        return stateRetRepeat;
    }
}

static stateRet_t stateTransitionPoweredAscent(stateInput_t *input){
    return stateRetRepeat;
}

static stateRet_t stateTransitionUnpoweredFlight(stateInput_t *input){
    return stateRetRepeat;
}

static stateRet_t stateTransitionBallutes(stateInput_t *input){
    return stateRetRepeat;
}

static stateRet_t stateTransitionMainParachutes(stateInput_t *input){
    return stateRetRepeat;
}

static stateRet_t stateTransitionLanded(stateInput_t *input){
    return stateRetRepeat;
}

static stateRet_t stateTransitionGroundAbort(stateInput_t *input){
    if(input->HMI_triggerFueling){
        return stateRetPass;
    }
    else{
        return stateRetRepeat;
    }
}

static stateRet_t stateTransitionFlightAbort(stateInput_t *input){
    return stateRetRepeat;
}

