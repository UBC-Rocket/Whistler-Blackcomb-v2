#include "buffer.h"



//circular buffer with waste slot

//"hidden" definiton of the circular buffer structure

 
struct cbuf_t
{
    uint8_t **buffer;
    size_t head;
    size_t tail;
    size_t max;
    bool full;
    SemaphoreHandle_t semaphore;
    

};

cbufHandle_t cbufInit(size_t size)
{
    assert(size);
    uint8_t **buffer = pvPortMalloc(size * sizeof(uint8_t*));
    cbufHandle_t cbuf = pvPortMalloc(sizeof(cbuf_t));
    assert(cbuf);
    
    cbuf->semaphore = xSemaphoreCreateBinary();
    cbuf->buffer = buffer;
    cbuf->max = size;
    cbufReset(cbuf);

    assert(cbufCheckEmpty(cbuf));
    

    return cbuf;
}

void cbufReset(cbufHandle_t cbuf)
{
    xSemaphoreTake(cbuf->semaphore,portMAX_DELAY);
    
    assert(cbuf);
    
    cbuf->head = 0;
    cbuf->tail = 0;
    cbuf->full = false;
    xSemaphoreGive(cbuf->semaphore);
}

void cbufFree(cbufHandle_t cbuf)
{
    xSemaphoreTake(cbuf->semaphore,portMAX_DELAY);

    assert(cbuf);
    
    vPortFree(cbuf->buffer);
    vPortFree(cbuf);
    xSemaphoreGive(cbuf->semaphore);

}

bool cbufCheckFull(cbufHandle_t cbuf)
{
    xSemaphoreTake(cbuf->semaphore,portMAX_DELAY);
    assert(cbuf);
    xSemaphoreGive(cbuf->semaphore);
    return cbuf->full;
}

bool cbufCheckEmpty(cbufHandle_t cbuf)
{
    assert(cbuf);
    xSemaphoreTake(cbuf->semaphore,portMAX_DELAY);
    return (!cbuf->full && (cbuf->head == cbuf->tail));
    xSemaphoreGive(cbuf->semaphore);
}

size_t cbufGetCapacity(cbufHandle_t cbuf)
{
    xSemaphoreTake(cbuf->semaphore,portMAX_DELAY);

    assert(cbuf);
    size_t max = cbuf->max;
    xSemaphoreGive(cbuf->semaphore);
    return max;

    
}


//should be a lock here I think 
size_t cbufGetSize(cbufHandle_t cbuf)
{
    xSemaphoreTake(cbuf->semaphore,portMAX_DELAY);

    assert(cbuf);
    size_t size = cbuf->max;

    if (!cbuf->full)
    {
        if (cbuf->head >= cbuf->tail)
        {
            size = (cbuf->head - cbuf->tail);
        }
        else
        {
            size = (cbuf->max + cbuf->head - cbuf->tail);
        }
    }

    xSemaphoreGive(cbuf->semaphore);

    return size;
}

static void advance_pointer(cbufHandle_t cbuf)
{
    xSemaphoreTake(cbuf->semaphore,portMAX_DELAY);

    assert(cbuf);

    if (cbuf->full)
    {
        if (++(cbuf->tail) == cbuf->max)
        {
            cbuf->tail = 0;
        }
    }

    if (++(cbuf->head) == cbuf->max)
    {
        cbuf->head = 0;
    }
    cbuf->full = (cbuf->head == cbuf->tail);

    xSemaphoreGive(cbuf->semaphore);
}

static void retreat_pointer(cbufHandle_t cbuf)
{
    xSemaphoreTake(cbuf->semaphore,portMAX_DELAY);

    assert(cbuf);

    cbuf->full = false;
    if (++(cbuf->tail) == cbuf->max)
    {
        cbuf->tail = 0;
    }

    xSemaphoreGive(cbuf->semaphore);

}

// void cbufForcePut(cbufHandle_t cbuf, uint8_t length, uint8_t *data)
// {
//     assert(cbuf && cbuf->buffer);

//     cbuf->buffer[cbuf->head] = length;
//     advance_pointer(cbuf);

//     for(int index=0;index<length;index++){
//         cbuf->buffer[cbuf->head] = data[index];
//         advance_pointer(cbuf);
//     }    
// }

// int cbufGentlePut(cbufHandle_t cbuf, uint8_t data)
// {
//     int r = -1;

//     assert(cbuf && cbuf->buffer);

//     if (!cbufCheckFull(cbuf))
//     {
//         cbuf->buffer[cbuf->head] = data;
//         advance_pointer(cbuf);
//         r = 0;
//     }

//     return r;
// }

int cbufPut(cbufHandle_t cbuf, uint8_t length, uint8_t *data)
{
    SemaphoreHandle_t testSemaphore = xSemaphoreCreateMutex();
    xSemaphoreGive(testSemaphore);
    xSemaphoreTake(testSemaphore,portMAX_DELAY);

    assert(cbuf && cbuf->buffer);
    uint8_t * message = pvPortMalloc((length+1)*sizeof(uint8_t));
    message[0]=length;
    for(int index=1;index<=length;index++){
        message[index]=data[index-1];
    }

    cbuf->buffer[cbuf->head] = message;
    
    advance_pointer(cbuf);
    
    xSemaphoreGive(testSemaphore);

    return 0;


}

int cbufGet(cbufHandle_t cbuf, uint8_t *data)
{
    xSemaphoreTake(cbuf->semaphore,portMAX_DELAY);

    //assert(cbuf && data && cbuf->buffer);

    uint8_t length = 0;

    if (!cbufCheckEmpty(cbuf))
    {
        uint8_t * message = cbuf->buffer[cbuf->tail];
        length = message[0];
        for(int index=0;index<length;index++){
            data[index] = message[index+1];
        }
        
        retreat_pointer(cbuf);

        vPortFree(message);
    }

    xSemaphoreGive(cbuf->semaphore);

    return length;
}