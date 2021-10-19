/* *****************************************************************************
 * @Summary Timer Library Header File
 * 
 * @author  Matthew Spinks
 * 
 * Date: Dec. 6, 2019   Original creation
 * 
 * @File Timer.h
 * 
 * @Description
 * 
*******************************************************************************/

#ifndef TIMER_H
#define	TIMER_H

// ***** Includes **************************************************************

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct Timer Timer;

/*  callback function pointer. The context is so that you can know which timer 
    initiated the callback. This is so that you can service multiple timer 
    callbacks with the same function if you desire. */
typedef void (*TimerCallbackFunc)(Timer *timerContext);

// Free timer (with bit field)
struct Timer
{
    uint16_t period;
    uint16_t count;
    
    TimerCallbackFunc timerCallbackFunc;
    
    // bit field
    union {
        struct {
            unsigned start      :1;
            unsigned active     :1;
            unsigned expired    :1;
            unsigned            :0; // fill to nearest byte
        };
    } flags;
};

/* These variable should be treated as private. You should only access them   
 * with the use of a function.
 * 
 * period   The period of the timer. When the count reaches this number, a flag
 *          will be set.
 * 
 * count    The current value of the timer
 * 
 * start    When this flag is set, the timer will begin counting down.
 *          The active bit will be set as well
 * 
 * active   This bit is 1 whenever the timer is running. Clear this bit to stop
 *          the timer
 * 
 * expired  This flag is set whenever the timer period reaches the specified 
 *          count. You must clear this flag yourself
 * 
 */

// ***** Function Prototypes ***************************************************

void Timer_InitMs(Timer *self, uint16_t periodMs, uint16_t tickMs);
void Timer_Start(Timer *self);
void Timer_Stop(Timer *self);
void Timer_Tick(Timer *self);
uint16_t Timer_GetCount(Timer *self);
uint16_t Timer_GetPeriod(Timer *self);
bool Timer_IsRunning(Timer *self);
bool Timer_IsFinished(Timer *self);
void Timer_ClearFlag(Timer *self);
void Timer_SetFinishedCallback(Timer *self, TimerCallbackFunc);

#endif	/* TIMER_H */