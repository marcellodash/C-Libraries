/* *****************************************************************************
 * @Summary Timer Library
 * 
 * @author  Matthew Spinks
 * 
 * Date: Dec. 6, 2019   Original creation
 * 
 * @File Timer.c
 * 
 * @Description
 *      A generic, free-running timer to do whatever you need. You must 
 * initialize it with an expected update rate in milliseconds (how often you
 * call the timer tick function) and the period for the timer in milliseconds.
 * Once the timer finishes, the "expire" flag is set. But the timer will 
 * continue to run as long as the "active" flag is set. The "expired" flag will 
 * not get cleared automatically. This is for you to check and decide what to
 * do. How you choose to call the "Timer_Tick" function is up to you. Just be
 * sure to call it periodically and make sure to initialize the timer with the
 * expected rate. This lets the timer know how far it must count. At the bottom
 * I've provided a timer finished callback function. The function pointer
 * you create must follow the prototype listed in Timer.h. It must have a Timer
 * object as the return type. The idea is that when you set up the callback 
 * function, you tell it which Timer object will be calling it. This provides 
 * context for you so that if you have multiple timers, you can tell which one 
 * executed the callback function. This way you can have multiple timers 
 * pointing to the same callback function if you desire. Then you could look
 * at the Timer object to see which one called it and decide what to do.
 * 
*******************************************************************************/

#include "Timer.h"

// ***** Defines ***************************************************************


// ***** Function Prototypes ***************************************************


// ***** Global Variables ******************************************************


// ----- Initialize ------------------------------------------------------------

void Timer_InitMs(Timer *self, uint16_t periodMs, uint16_t tickMs)
{
    if(tickMs != 0)
        self->period = periodMs / tickMs;
}

// -----------------------------------------------------------------------------

void Timer_Start(Timer *self)
{
    if(self->period != 0)
    {
        self->flags.start = 1;
    }
}

// -----------------------------------------------------------------------------

void Timer_Stop(Timer *self)
{
    self->flags.start = 0;
    self->flags.active = 0;
}

// -----------------------------------------------------------------------------

void Timer_Tick(Timer *self)
{
    // Check to see if timer is active and ready to start
    if(self->flags.start && self->period != 0)
    {
        self->flags.start = 0;
        self->count = self->period;
        self->flags.active = 1;
    }
    
    // Update active timers
    if(self->flags.active)
    {
        self->count--;
        
        if(self->count == 0)
        {
            self->flags.active = 0;
            self->flags.expired = 1;
            
            if(self->timerCallbackFunc)
            {
                self->timerCallbackFunc(self);
            }
        }
    }
}

// -----------------------------------------------------------------------------

uint16_t Timer_GetCount(Timer *self)
{
    return self->count;
}

// -----------------------------------------------------------------------------

uint16_t Timer_GetPeriod(Timer *self)
{
    return self->period;
}

// -----------------------------------------------------------------------------

bool Timer_IsRunning(Timer *self)
{
    if(self->flags.active)
        return true;
    else
        return false;
}

// -----------------------------------------------------------------------------

bool Timer_IsFinished(Timer *self)
{
    if(self->flags.expired)
        return true;
    else
        return false;
}

// -----------------------------------------------------------------------------

void Timer_ClearFlag(Timer *self)
{
    self->flags.expired = 0;
}

// -----------------------------------------------------------------------------

void Timer_SetFinishedCallback(Timer *self, TimerCallbackFunc Function)
{
    self->timerCallbackFunc = Function;
}

/*
 End of File
 */