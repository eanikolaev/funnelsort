#include <stdlib.h>
#include <string.h>
#include "sort.h"
#include <math.h>
#include <stdio.h>
#define M 128


struct buffer { // FIFO queue
    void *data;
    size_t nmemb; // max count of elements
    size_t size;  // size of one element
    size_t head;
    size_t tail;
    size_t count; // current count of elements
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


void
print_buffer(struct buffer *buffer, int shift)
{
    size_t i;
    printf("%*s" "%s\n", shift, " ", "Buffer");
    printf("%*s" "%s %d\n", shift+2, " ", "nmemb:", (int)buffer->nmemb);       
    printf("%*s" "%s %d\n", shift+2, " ", "head:", (int)buffer->head);       
    printf("%*s" "%s %d\n", shift+2, " ", "tail:", (int)buffer->tail);       
    printf("%*s" "%s %d\n", shift+2, " ", "count:", (int)buffer->count);       
    printf("%*s" "%s", shift+2, " ", "data: ");
    for (i=0; i<buffer->nmemb; i++) {
        printf("%d", (int)*(int*)(buffer->data + i*buffer->size));
        printf("%c", ' ');
    }
    printf("%s", "\n");
}


void
print_funnel(struct funnel *funnel, int shift)
{
    if (funnel == NULL) {
        printf("%*s" "%s\n", shift+2, " ", "NULL");
        return;
    }
    printf("%*s" "\n%s\n", shift, " ", "***FUNNEL***");

    printf("%*s" "%s\n", shift+2, " ", "OUTPUT BUFFER");
    print_buffer(funnel->out, shift+2);

    printf("%*s" "%s\n", shift+2, " ", "TOP FUNNEL");
    print_funnel(funnel->top, shift+4);

    printf("INTERMEDIATE BUFFERS, BOTTOM FUNNELS:\n");    
    size_t i;
    for (i=0; i<funnel->bb_count; i++) {
        printf("%*s" "%s %d:\n", shift, " ", "intermediate buffer", (int)i);
        print_buffer(funnel->buffers[i], shift+2);

        printf("%*s" "%s %d:\n", shift, " ", "bottom funnel", (int)i);
        print_funnel(funnel->bottom[i], shift+4);        
    }

    printf("%*s" "%s\n", shift+2, " ", "INPUT BUFFERS:");
    for (i=0; i<funnel->in_count; i++) {
        print_buffer(funnel->in[i], shift+2);
    }
}


void
enqueue(struct buffer* buffer, void *el)
{
    memcpy(buffer->data + buffer->size * buffer->tail, el, buffer->size);
    buffer->tail = (buffer->tail + 1) % buffer->nmemb;
    buffer->count++;
}


void *
dequeue(struct buffer* buffer)
{
    size_t head = buffer->head;
    buffer->head = (buffer->head + 1) % buffer->nmemb;
    buffer->count--;
    return buffer->data + head * buffer->size;
}


int
buffer_empty(struct buffer *buffer)
{
    return !buffer->count;
}


int
buffer_full(struct buffer *buffer)
{
    return (buffer->count == buffer->nmemb);
}


int
buffers_nonempty(struct buffer **in, size_t in_count)
{
    size_t i;
    for (i=0; i<in_count; i++) {
        if (buffer_empty(in[i])) {
           return 0;
        }
    }
    return 1;
}


int
buffers_empty(struct buffer **in, size_t in_count)
{
    size_t i;
    for (i=0; i<in_count; i++) {
        if (!buffer_empty(in[i])) {
           return 0;
        }
    }
    return 1;
}


void *
buffer_head(struct buffer *buffer)
{
    return buffer->data + buffer->head * buffer->size;
}


size_t
get_best_buffer_num(struct buffer **in, size_t in_count, const cmp_t cmp)
{
    size_t i,j, best;
    for (i=0; i<in_count; i++) {        
        if (!buffer_empty(in[i])) {
            best = i;
            break;
        }
    }
    for (j=i+1; j<in_count; j++) {        
        if (!buffer_empty(in[j]) && cmp(buffer_head(in[j]), buffer_head(in[best])) < 0) {
            best = j;
        }
    }
    return best;
}


int
get_buffer_nmemb(const int bot_nmemb, const int top_nmemb)
{
    if (top_nmemb) {
        return 2 * pow(top_nmemb, 1.5);
    }
    else {
        return 2 * pow(bot_nmemb, 1.5);
    }
}


struct buffer *
buffer_create(void *data, const size_t nmemb, const size_t size, const size_t head, const size_t tail, const size_t count)
{
    struct buffer *buffer = (struct buffer *)
               malloc(sizeof(struct buffer));
    buffer->data = data;
    buffer->nmemb = nmemb;
    buffer->size = size;
    buffer->head = head;
    buffer->tail = tail;
    buffer->count = count;
    return buffer;
}


struct buffer *
buffer_new(const size_t nmemb, const size_t size)
{ // create empty buffer (initialize FIFO queue)
    void *data = (void *) malloc(nmemb*size);
    return buffer_create(data, nmemb, size, 0, 0, 0);
}


struct funnel *
funnel_new(struct buffer **in, struct buffer *out, const size_t size, const size_t in_count)
{
    struct funnel *funnel = (struct funnel *)
               malloc(sizeof(struct funnel));
    funnel->in = in;
    funnel->in_count = in_count;
    funnel->out = out;
    funnel->size = size;
    if (in_count * size < M/4.0) { // maybe not M/4?
        funnel->bb_count = 0;
        funnel->bottom = NULL;
        funnel->top = NULL;
        return funnel;
    }
    int i;
    double root = sqrt(in_count);
    struct buffer **curr = in;
    int froot = floor(root);

    if (froot*froot < (int)in_count) { // if in_count isn't square
        int croot = ceil(root);
        int big_count = in_count/(double)croot; // count buffers of size croot
        int rest = in_count - big_count*croot;
        int small_count = ceil(rest/froot); // count buffers of size froot
        int all_count = big_count + small_count; // size of top funnel
        int big_buff_nmemb   = get_buffer_nmemb(croot, all_count);
        int small_buff_nmemb = get_buffer_nmemb(froot, all_count);
        struct buffer **buffers = (struct buffer**)
                  malloc(sizeof(struct buffer *)*all_count); // intermediate buffers

        struct funnel **bfunnels = (struct funnel **)
                  malloc(sizeof(struct funnel *)*all_count); // bottom funnels

        for (i=0; i<big_count; i++) {
            buffers[i] = buffer_new(big_buff_nmemb, size);
            bfunnels[i] = funnel_new(curr, buffers[i], size, croot);             
            curr += croot;
        }

        for (i=big_count; i<all_count; i++) {
            buffers[i] = buffer_new(small_buff_nmemb, size);
            bfunnels[i] = funnel_new(curr, buffers[i], size, froot);             
            curr += croot;
        }

        funnel->bb_count = all_count;
        funnel->buffers = buffers;
        funnel->bottom = bfunnels;

        struct funnel *tfunnel = (struct funnel *)
                  malloc(sizeof(struct funnel *));
        tfunnel = funnel_new(buffers, out, size, all_count);
        funnel->top = tfunnel;
    }
    else {
        funnel->bb_count = froot;
        struct buffer **buffers = (struct buffer**)
                  malloc(sizeof(struct buffer *)*froot); // intermediate buffers

        struct funnel **bfunnels = (struct funnel **)
                  malloc(sizeof(struct funnel *)*froot); // bottom funnels
        int buff_nmemb = 2 * pow(froot, 1.5);
        
        for (i=0; i<froot; i++) {
            buffers[i] = buffer_new(buff_nmemb, size);
            bfunnels[i] = funnel_new(curr, buffers[i], size, froot);
            curr += froot;
        }
        
        funnel->buffers = buffers;
        funnel->bottom = bfunnels;

        struct funnel *tfunnel = (struct funnel *)
                  malloc(sizeof(struct funnel *));
        tfunnel = funnel_new(buffers, out, size, froot);
        funnel->top = tfunnel;
    }
    return funnel;
}


struct buffer **
buffers_create(void *data, const size_t nmemb, const size_t size,
                                     const size_t count, const size_t len, const size_t extra)
{ // divide data into count (+extra) buffers
    
    struct buffer **buffers = (struct buffer **)malloc(sizeof(struct buffer*)*(count + extra)); 
    size_t i, d = len*size;
    void *p = data;
    for (i=0; i<count; i++) {
        buffers[i] = buffer_create(p, len, size, 0, 0, len);
        p += d;
    }
    if (extra > 0) {
        size_t rest = nmemb - len * count;
        buffers[i] = buffer_create(p, rest, size, 0, 0, rest);
    }
    
    return buffers;
}


struct funnel *
funnel_create(void *data, const size_t nmemb, const size_t size, const size_t count, const size_t len, const size_t extra)
{
    struct buffer **in_buffers = buffers_create(data, nmemb, size, count, len, extra);
    struct buffer *out_buffer = buffer_new(nmemb, size);
    return funnel_new(in_buffers, out_buffer, size, count+extra);
}


void
funnel_fill(struct funnel *funnel, const cmp_t cmp)
{
    if (buffers_nonempty(funnel->in, funnel->in_count)) {
        while (!buffer_full(funnel->out)){// && !buffers_empty(funnel->in, funnel->in_count)) {
            size_t best_num = get_best_buffer_num(funnel->in, funnel->in_count, cmp);
            enqueue(funnel->out, dequeue(funnel->in[best_num])); 
/*	    need fill in?
            if (buffer_empty(funnel->in[best_num])) {
            }
*/
        }
    }
    else {
        return;
    }
}


void
funnel_warmup(struct funnel *funnel, const cmp_t cmp)
{
    if (funnel->bb_count) {
        size_t i;
        for (i=0; i<funnel->bb_count; i++) {
            funnel_warmup(funnel->bottom[i], cmp);
        }
    }
    funnel_fill(funnel, cmp);
}


void
sort(void *ptr, const size_t nmemb, const size_t size, const cmp_t cmp)
{
    if (nmemb * size < M/4) { // maybe not M/4?
        qsort(ptr, nmemb, size, cmp); 
    }
    else {
           int n = pow(nmemb, 1/3.0); // n = nmemb^(1/3), count of parts
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
//         ??  funnel_warmup(funnel, cmp);
          funnel_fill(funnel, cmp);
          memcpy(ptr, funnel->out->data, nmemb*size);
    } 
}
