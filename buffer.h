#ifndef INCLUDES_FUNNELSORT_BUFFER_H
#define INCLUDES_FUNNELSORT_BUFFER_H
struct Buffer { // FIFO queue
    void *data;
    size_t nmemb; // max count of elements
    size_t size;  // size of one element
    size_t head;
    size_t tail;
    size_t count; // current count of elements
};
#endif /* INCLUDES_FUNNELSORT_BUFFER_H */
