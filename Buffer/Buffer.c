/*******************************************************************************
 * @Summary: Basic Ring Buffer
 * 
 * @author Matthew Spinks
 * 
 * Date: Dec. 6, 2019   Original creation
 * 
 * @File Buffer.c
 * 
 * @Description
 *      A basic 8-bit ring buffer. To create a buffer, the minimum you will 
 *      need is a buffer object, an array pointer, and the size of the array.
 * 
 * ****************************************************************************/

#include "Buffer.h"

// ***** Defines ***************************************************************

/*  I'm going to use a simple check to go around the ring buffer. In the past, 
    I would use a logical AND type of modulo division. It worked really quickly, 
    but it restricted the buffer size to powers of two only. */
#define CircularIncrement(i, size) i == (size - 1) ? 0 : i + 1

// ***** Function Prototypes ***************************************************


// ***** Global Variables ******************************************************

// local function pointer
void (*Buffer_OverflowCallback)(void);

/*******************************************************************************
 * Initializes a Buffer object
 * <p>
 * Sets up pointers to the buffer. Does not allow the buffer to overwrite
 * values by default
 * 
 * @param self  pointer to the Buffer that you are going to use
 * 
 * @param arrayIn  pointer to the array that you are going to use
 * 
 * @param arrayInSize  the size of said array 
 * 
 * @return none
 */
void Buffer_Init(Buffer *self, uint8_t *arrayIn, uint8_t arrayInSize)
{
    Buffer_InitWithOverwrite(self, arrayIn, arrayInSize, false);
}

/*******************************************************************************
 * This is an extension of the normal Buffer_Init function.
 * <p>
 * Gives you a boolean that, when initialized as true, will allow the buffer 
 * to overwrite data once it is full.
 * <p>
 * If you are using this as a transmit buffer and you need to check for
 * space in the buffer, you should do it beforehand. I've used a while-loop to 
 * wait for space in the buffer before. It works well for microcontrollers that 
 * have interrupts which run automatically, but doesn't work well for everyone.
 * <p>
 * If you using it to transmit out of, you should probably not have the
 * overwrite boolean enabled.
 * 
 * @param self  pointer to the Buffer that you are going to use
 * 
 * @param arrayIn  pointer to the array that you are going to use
 * 
 * @param arrayInSize  the size of said array
 * 
 * @param overwrite  enable overwrite of buffer data if true
 * 
 * @return none
 */
void Buffer_InitWithOverwrite(Buffer *self, uint8_t *arrayIn, uint8_t arrayInSize, bool overwrite)
{
    self->private.buffer = arrayIn;
    self->private.size = arrayInSize;
    self->enableOverwrite = overwrite;
    self->count = 0;
}

/*******************************************************************************
 * Puts a char into the buffer. Updates the head.
 * 
 * @param self  pointer to the Buffer that you are using
 * 
 * @param receivedChar  the char to store in the buffer
 * 
 * @return none
 */
void Buffer_WriteChar(Buffer *self, uint8_t receivedChar)
{
    uint8_t tempHead = CircularIncrement(self->private.head, self->private.size);
    
    if(tempHead != self->private.tail)
    {
        // There is space in the buffer
        self->private.buffer[self->private.head] = receivedChar;
        self->private.head = tempHead;
        self->count++;
    }
    else if(self->enableOverwrite)
    {
        // There is no space in the buffer and overwrite is enabled
        self->private.buffer[self->private.head] = receivedChar;
        self->private.head = tempHead; // Mark the next space to be overwritten
        CircularIncrement(self->private.tail, self->private.size); // Move the tail up one
        self->overflow = true;
    }
    else
    {
        // There is no space in the buffer and overwrite is disabled
        if(self->overflow == false)
        {
            // We are about to overflow. Go ahead and store the last char
            self->private.buffer[self->private.head] = receivedChar;
        }
        
        self->overflow = true; // Notify of overflow
        
        if(Buffer_OverflowCallback)
        {
            Buffer_OverflowCallback;
        }
    }
}

/*******************************************************************************
 * Reads a char from the buffer. Updates the tail.
 * <p>
 * Right now, I have it set to return zero if the buffer is empty.
 * It is your responsibility to check if the buffer has data beforehand.
 * I've provided the function Buffer_IsFull for you to use.
 * 
 * @param self  pointer to the Buffer that you are using
 * 
 * @return a char read from the buffer
 */
uint8_t Buffer_ReadChar(Buffer *self)
{
    uint8_t dataToReturn = 0;
    
    if(self->private.head != self->private.tail)
    {
        // The buffer is not empty
        dataToReturn =  self->private.buffer[self->private.tail];
        CircularIncrement(self->private.tail, self->private.size);
        self->count--;
        self->overflow = false;
    }
    return dataToReturn;
}

/*******************************************************************************
 * Gets the amount of data stored in the buffer
 * 
 * @param self  pointer to the Buffer that you are using
 *
 * @return number of bytes in the buffer
 */
uint8_t Buffer_GetCount(Buffer *self)
{
/*  For now, I'm using a simple counter. If I choose to limit by buffer
    to powers of two, then I can do away with the counter and get the size 
    based on the head and tail with modulo division. Example below
    return (uint8_t)(self->private.size + self->private.head - self->private.tail) & (self->private.size - 1); 
    */

    return self->count;
}

/*******************************************************************************
 * A convenience function that tells you if the buffer is full.
 * 
 * @param self  pointer to the Buffer type that you are using
 * 
 * @return true if buffer is full
 */
bool Buffer_IsFull(Buffer *self)
{
    uint8_t tempHead = CircularIncrement(self->private.head, self->private.size);
    
    if(tempHead == self->private.tail)
        return true;
    else
        return false;
}

/*******************************************************************************
 * A convenience function that tells you if there is something in the buffer
 * <p>
 * Useful for transmit buffers
 * 
 * @param self  pointer to the Buffer type that you are using
 * 
 * @return true if buffer is not empty
 */
bool Buffer_IsNotEmpty(Buffer *self)
{
    if(self->count != 0)
        return true;
    else
        return false;
}

/*******************************************************************************
 * A convenience function that tells you if the buffer overflowed
 * 
 * The overflow flag is cleared when you call this function. It is also
 * cleared automatically when space appears in the buffer. 
 * 
 * @param self  pointer to the Buffer type that you are using
 * 
 * @return true if buffer did overflow
 */
bool Buffer_DidOverflow(Buffer *self)
{
    // Automatically clear the flag
    bool temp = self->overflow;
    self->overflow = false;
    return temp;
}

/*******************************************************************************
 * A function pointer that is called when the buffer overflows.
 * 
 * Only works if you have overwrite disabled. If you set this function pointer, 
 * your function will automatically be called whenever the buffer tries to 
 * overwrite data. The overflow boolean is also set.
 * 
 * @param self  pointer to the Buffer type that you are using
 * 
 * @param Function  format: void SomeFunction(void)
 */
void Buffer_SetOverflowCallback(void (*Function)(void))
{
    Buffer_OverflowCallback = Function;
}

