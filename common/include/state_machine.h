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

/**
 * All possible states the rocket can be in. 
 */
typedef enum state_t
{
    stateError, 
    stateFueling,
    stateIgnition,
    stateAscent,
    stateRecovery
};

/**
 * Possible returns for the state transition functions
 */
typedef enum stateRet_t
{
    stateRetPass,
    stateRetRepeat,
    stateRetAbort
};

/**
 * A struct describing the input to the state transitions. 
 * 
 * Note that because we want an array of state transitions, the parameter type
 * must be the same for all of them (we could do some hackery with void
 * pointers, but this whole thing is supposed to make things simpler, not more
 * complicated). For this reason, all possible inputs have to be encoded in this
 * one struct. This means some inputs won't be used for some transitions, e.g.
 * altitude for fueling. 
 */
typedef struct stateInput_t
{
    double vertVelocity;
    double vertPosition;
    /* TODO: Add actual data */
};



/*******************************************************************************
 * Declarations
 ******************************************************************************/


/**
 * Gets the current state. 
 * 
 * @return the current state
 */
state_t getState(void);

/**
 * Calls the current transition function, and if necessary changes the state to
 * the next one according to the inputted data. 
 * 
 * @param input the data to make state change decision based on
 * @return the current state after input is taken into account
 */
state_t setNextState(stateInput_t *input);

#endif