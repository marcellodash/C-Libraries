/* *****************************************************************************
 * @Summary Button Library
 * 
 * @author  Matthew Spinks
 * 
 * Date: Dec. 6, 2019   Original creation
 * 
 * @File Button.c
 * 
 * @Description
 * 
*******************************************************************************/

#include "Button.h"

// ***** Defines ***************************************************************


// ***** Function Prototypes ***************************************************


// ***** Global Variables ******************************************************


// ----- Initialize ------------------------------------------------------------

void Button_InitMs(Button *self, uint16_t pressDebounceMs, uint16_t releaseDebounceMs, uint16_t tickMs)
{
    Button_InitWithLongPressMs(self, pressDebounceMs, releaseDebounceMs, 0, tickMs);
}

void Button_InitWithLongPressMs(Button *self, uint16_t pressDebounceMs, uint16_t releaseDebounceMs, uint16_t longPressMs, uint16_t tickMs)
{
    if(tickMs != 0)
    {
        self->pressDebouncePeriod = pressDebounceMs / tickMs;
        self->releaseDebouncePeriod = releaseDebounceMs / tickMs;
        self->longPressPeriod = longPressMs / tickMs;
    }
    
    // In case you accidentally initialize the long press period to zero or
    // a value less than one tick
    if(self->longPressPeriod == 0)
        self->buttonType = SHORT_PRESS_TYPE;
    else
        self->buttonType = LONG_PRESS_TYPE;
    
    self->buttonState = BUTTON_UP;
}

// -----------------------------------------------------------------------------

void Button_Tick(Button *self, bool isPressed)
{
    switch(self->buttonState)
    {
        case BUTTON_UP:
            if(isPressed)
            {
                if(self->pressDebouncePeriod == 0)
                {
                    // If the debounce period is zero, we assume that 
                    // debouncing is being done via hardware
                    if(self->buttonType == SHORT_PRESS_TYPE)
                    {
                        self->flags.shortPress = 1;
                        
                        // TODO callback function
                    }
                    self->flags.buttonDownEvent = 1;
                    self->buttonState = BUTTON_DOWN;
                    self->longPressCounter = 0;
                }
                else
                {
                    // Else, we need to debounce our button press
                    self->buttonState = DEBOUNCE_PRESS;
                    self->debounceCounter = 0;
                }
            }
            break;
        case DEBOUNCE_PRESS:
            self->debounceCounter++;
            if(self->debounceCounter == self->pressDebouncePeriod)
            {
                if(isPressed)
                {
                    // Button debounced successfully
                    if(self->buttonType == SHORT_PRESS_TYPE)
                    {
                        // We are finished.
                        self->flags.shortPress = 1;
                        
                        // TODO callback function
                    }
                    self->flags.buttonDownEvent = 1;
                    self->buttonState = BUTTON_DOWN;
                    self->longPressCounter = 0;
                }
                else
                {
                    // We finished debouncing, but the button isn't being
                    // pressed
                    self->buttonState = BUTTON_UP;
                }
            }
            break;
        case BUTTON_DOWN:
            // Update the long press timer
            if(isPressed && self->buttonType == LONG_PRESS_TYPE)
            {
                if(self->longPressCounter < self->longPressPeriod)
                {
                    self->longPressCounter++;
                    
                    if(self->longPressCounter == self->longPressPeriod)
                        self->flags.longPress = 1;
                }
            }
            
            // Check for the button release
            if(!isPressed)
            {
                if(self->releaseDebouncePeriod == 0)
                {
                    // If the debounce period is zero, we assume that 
                    // debouncing is being done via hardware
                    if(self->buttonType == LONG_PRESS_TYPE)
                    {
                        // If the button is a long press type, the short press
                        // event should happen on the release if the button
                        // is released before the timer expires.
                        if(self->longPressCounter < self->longPressPeriod)
                        {
                            self->flags.shortPress = 1;
                        }
                    }
                    self->flags.buttonUpEvent = 1;
                    self->buttonState = BUTTON_UP;
                }
                else
                {
                    // Else, we need to debounce our button releases
                    self->buttonState = DEBOUNCE_RELEASE;
                    self->debounceCounter = 0;
                }
            }
            break;
        case DEBOUNCE_RELEASE:
            self->debounceCounter++;
            if(self->debounceCounter == self->releaseDebouncePeriod)
            {
                if(!isPressed)
                {
                    // Button debounced successfully
                    if(self->buttonType == LONG_PRESS_TYPE)
                    {
                        // If the button is a long press type, the short press
                        // event should happen on the release if the button
                        // is released before the timer expires.
                        if(self->longPressCounter < self->longPressPeriod)
                        {
                            self->flags.shortPress = 1;
                        }
                    }
                    self->flags.buttonUpEvent = 1;
                    self->buttonState = BUTTON_UP;
                }
                else
                {
                    // We finished debouncing, but the button isn't being
                    // pressed
                    self->buttonState = BUTTON_UP;
                }
            }
            break;
        default:
            
            break;
    }
}

// -----------------------------------------------------------------------------

bool Button_GetShortPress(Button *self)
{
    if(self->flags.shortPress)
        return true;
    else
        return false;
}

// -----------------------------------------------------------------------------

bool Button_GetLongPress(Button *self)
{
    if(self->flags.longPress)
        return true;
    else
        return false;
}

// -----------------------------------------------------------------------------

void Button_ClearShortPressFlag(Button *self)
{
    self->flags.shortPress = 0;
}

// -----------------------------------------------------------------------------

void Button_ClearLongPressFlag(Button *self)
{
    self->flags.longPress = 0;
}

// -----------------------------------------------------------------------------

bool Button_GetButtonDownEvent(Button *self)
{
    if(self->flags.buttonDownEvent)
        return true;
    else
        return false;
}

// -----------------------------------------------------------------------------

bool Button_GetButtonUpEvent(Button *self)
{
    if(self->flags.buttonUpEvent)
        return true;
    else
        return false;
}

// -----------------------------------------------------------------------------

void Button_ClearButtonDownFlag(Button *self)
{
    self->flags.buttonDownEvent = 0;
}

// -----------------------------------------------------------------------------

void Button_ClearButtonUpFlag(Button *self)
{
    self->flags.buttonUpEvent = 0;
}

/*
 End of File
 */