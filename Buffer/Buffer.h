/*******************************************************************************
 * @Summary Basic Ring Buffer Header
 * 
 * @author Matthew Spinks
 * 
 * Date: Dec. 6, 2019   Original creation
 * 
 * @File Buffer.h
 * 
 * @Description
 *      A basic 8-bit ring buffer. To create a buffer, the minimum you will 
 *      need is a buffer type, an array pointer, and the size of the array.
 * 
 *      This library lets you control the size of your ring buffer as well as 
 *      the way the buffer handles overflows. Multiple functions are provided 
 *      to get the status of the buffer. For each function, you will pass the 
 *      buffer that you wish to perform the operation on.
 * 
 *      There are two initializations: One has a boolean which will allow data 
 *      to be overwritten when placing data in the buffer. The default setting
 *      is false. 
 * 
 *      The buffer will check for overflow before overwriting any data. If 
 *      overflow is about to happen and you have overwrite disabled, you will 
 *      receive a callback function and boolean notification. If you don't 
 *      clear the notification it will be cleared for you when there is space
 *      in the buffer.
 * 
 * ****************************************************************************/

#ifndef BUFFER_H
#define	BUFFER_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct Buffer Buffer;

/*  Buffer Object. You shouldn't really need to access anything in here 
    directly. I've provided functions to do that for you. */
struct Buffer
{
    uint8_t count;
    bool overflow;
    bool enableOverwrite;
    
    struct
    {
        //  Yo dawg. I heard you liked structs...
        uint8_t *buffer;
        uint8_t size;
        uint8_t head;
        uint8_t tail;
    } private;
};

/* These variable should be treated as private. You should only access them   
 * with the use of a function.
 * 
 * buffer   A pointer to the array which will form your ring buffer
 * 
 * size     the size of your array
 * 
 * head     Keeps track of the current index of data being written into the
 *          buffer
 * 
 * tail     Keeps track of the current index of data being read out from the
 *          buffer
 */

// ***** Function Prototypes ***************************************************

void Buffer_Init(Buffer *self, uint8_t *arrayIn, uint8_t arrayInSize);

void Buffer_InitWithOverwrite(Buffer *self, uint8_t *arrayIn, uint8_t arrayInSize, bool overwrite);

void Buffer_WriteChar(Buffer *self, uint8_t receivedChar);

uint8_t Buffer_ReadChar(Buffer*);

uint8_t Buffer_GetCount(Buffer*);

bool Buffer_IsFull(Buffer*);

bool Buffer_IsNotEmpty(Buffer*);

bool Buffer_DidOverflow(Buffer*);

void Buffer_SetOverflowCallback(void (*Function)(void));

#endif	/* BUFFER_H */

