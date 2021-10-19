/* *****************************************************************************
 * @Summary Button Library Header File
 * 
 * @author  Matthew Spinks
 * 
 * Date: Dec. 6, 2019   Original creation
 * 
 * @File Button.h
 * 
 * @Description
 * 
*******************************************************************************/

#ifndef BUTTON_H
#define	BUTTON_H

// ***** Includes **************************************************************

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct Button Button;
typedef enum ButtonState ButtonState;
typedef enum ButtonType ButtonType;

enum ButtonState
{
    BUTTON_UP,
    DEBOUNCE_PRESS,
    BUTTON_DOWN,
    DEBOUNCE_RELEASE,   
};

/* The button type changes the behavior of the long press and short press
 * actions. If a button does not have a long press enabled, then the short
 * press action will happen after the button is debounced. If a button has a
 * long press feature, then the short press action should happen on the release
 * of the button (if not being held down longer than the long press interval).
 * 
 * For the most part, you should be looking at the shortPress and longPress
 * events. However, I've also included a buttonDownEvent. This can be useful
 * in certain situations, like when you need to wake up a display when a button
 * is pressed down, but don't necessarily want to do the normal button action.
 */
enum ButtonType
{
    SHORT_PRESS_TYPE,
    LONG_PRESS_TYPE,
};

/*  callback function pointers */
//typedef void (*TimerCallbackFunc)(Timer *timerContext); // TODO

/* Button object with counters and flags (with bit field) */
struct Button
{
    uint16_t pressDebouncePeriod;
    uint16_t releaseDebouncePeriod;
    uint16_t longPressPeriod;
    uint16_t longPressCounter;
    uint16_t debounceCounter;
 
    ButtonState buttonState;
    ButtonType buttonType;
    //TimerCallbackFunc TimerCallbackFunc;
    
    // bit field for button events
    union {
        struct {
            unsigned buttonDownEvent    :1;
            unsigned shortPress         :1;
            unsigned longPress          :1;
            unsigned buttonUpEvent      :1;
            unsigned                    :0; // fill to nearest byte
        };
    } flags;
};

/* These variable should be treated as private. You should only access them   
 * with the use of a function.
 * 
 * expired  This flag is set whenever the timer period reaches the specified 
 *          count. You must clear this flag yourself
 * 
 */

// ***** Function Prototypes ***************************************************

void Button_InitMs(Button *self, uint16_t pressDebounceMs, uint16_t releaseDebounceMs, uint16_t tickMs);

void Button_InitWithLongPressMs(Button *self, uint16_t pressDebounceMs, uint16_t releaseDebounceMs, uint16_t longPressMs, uint16_t tickMs);

void Button_Tick(Button *self, bool isPressed);

bool Button_GetShortPress(Button *self);

bool Button_GetLongPress(Button *self);

void Button_ClearShortPressFlag(Button *self);

void Button_ClearLongPressFlag(Button *self);

bool Button_GetButtonDownEvent(Button *self);

bool Button_GetButtonUpEvent(Button *self);

void Button_ClearButtonDownFlag(Button *self);

void Button_ClearButtonUpFlag(Button *self);

//void Timer_SetFinishedCallback(TimerCallbackFunc);

#endif	/* BUTTON_H */