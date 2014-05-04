#include <stdlib.h>
#include <string.h>
#include "sort.h"
#include <math.h>
#include <stdio.h>
#define M 64


struct buffer { // FIFO queue
    void *data;
    size_t nmemb; // max count of elements
    size_t size;  // size of one element
    size_t head;
    size_t tail;
};


struct funnel {
    struct buffer **in; // input arrays represented as buffers
    size_t in_count; // count of input arrays (buffers)

    struct buffer **buffers;
    struct funnel **bottom;
    size_t bb_count; // count of bottom funnels = count of buffers

    size_t size; // size of one element
    struct funnel *top;
    struct buffer *out;
};


struct funnel *
funnel_new(struct buffer **in, struct buffer *out, const size_t size, const size_t in_count, const size_t nmemb)
{
    struct funnel *funnel = (struct funnel *)
               malloc(sizeof(struct funnel));

    funnel->in = in;
    funnel->in_count = in_count;
    funnel->out = out;
 
    if (nmemb * size < M/4.0) { // maybe not M/4?
        return funnel;
    }

    /*
       TODO:
       create top and bottom funnels, intermediate buffers
    */   
    return funnel;
}


struct buffer *buffer_create(void *data, const size_t nmemb, const size_t size, const size_t head, const size_t tail)
{
    struct buffer *buffer = (struct buffer *)
               malloc(sizeof(struct buffer));
    buffer->data = data;
    buffer->nmemb = nmemb;
    buffer->size = size;
    buffer->head = head;
    buffer->tail = tail;
    return buffer;
}


struct buffer *buffer_new(const size_t nmemb, const size_t size)
{ // create empty buffer (initialize FIFO queue)
    void *data = (void *) malloc(nmemb*size);
    return buffer_create(data, nmemb, size, 0, -1);
}


struct buffer **buffers_create(void *data, const size_t nmemb, const size_t size,
                                     const size_t count, const size_t len, const size_t extra)
{ // divide data into count (+extra) buffers
    
    struct buffer **buffers = (struct buffer **)malloc(sizeof(struct buffer*)*(count + extra)); 
    size_t i, d = len*size;
    void *p = data;
    for (i=0; i<count; i++) {
        buffers[i] = buffer_create(p, len, size, 0, len);
        p += d;
    }
    if (extra > 0) {
        size_t rest = nmemb - len * count;
        buffers[i] = buffer_create(p, rest, size, 0, rest);
    }
    return buffers;
}


struct funnel *
funnel_create(void *data, const size_t nmemb, const size_t size, const size_t count, const size_t len, const size_t extra)
{
    struct buffer **in_buffers = buffers_create(data, nmemb, size, count, len, extra);
    struct buffer *out_buffer = buffer_new(nmemb, size);
    return funnel_new(in_buffers, out_buffer, size, count, nmemb);
}


void
sort(void *ptr, const size_t nmemb, const size_t size, const cmp_t cmp)
{
    if (nmemb * size < M/4.0) { // maybe not M/4?
        qsort(ptr, nmemb, size, cmp); 
    }
    else {
           double n = pow(nmemb, 1/3.0); // n = nmemb^(1/3), count of parts
           size_t len = nmemb / n; // nmemb in each part
           size_t d = len*size;
           void *p = ptr;
           int i;
           for (i=0; i<n; i++) {
               sort(p, len, size, cmp);
               p += d;
           }

           size_t extra = 0;
           size_t rest = nmemb - n * len;
           if (rest > 0) {
               sort(p, rest, size, cmp);
               extra++;
           }

           struct funnel *funnel = funnel_create(ptr, nmemb, size, n, len, extra); 
           printf("%d", (int)funnel->in_count);
           printf("%c", ' ');
            /*
               TODO:
               merge
            */
    }
}
