#ifndef INCLUDES_FUNNELSORT_FUNNEL_H
#define INCLUDES_FUNNELSORT_FUNNEL_H
#include "buffer.h"
struct Funnel {
    struct Buffer **in; // input arrays represented as buffers
    size_t in_count; // count of input arrays (buffers)

    struct Buffer **buffers;
    struct Funnel **bottom;
    size_t bb_count; // count of bottom funnels = count of buffers

    size_t size; // size of one element
    struct Funnel *top;
    struct Buffer *out;
};
#endif /* INCLUDES_FUNNELSORT_FUNNEL_H */
