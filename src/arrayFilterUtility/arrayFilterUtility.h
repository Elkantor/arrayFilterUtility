#ifndef FILTERED_ARRAY_H
#define FILTERED_ARRAY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define DEFINE_FILTERED_ARRAY(TYPE, NAME, INDEX_TYPE, NUM_STATES, CAPACITY)   \
typedef struct {                                                              \
    INDEX_TYPE cursors[(NUM_STATES) + 1];                                     \
    TYPE data[CAPACITY];                                                       \
} NAME;                                                                        \
                                                                               \
static inline void NAME##_init(NAME * restrict arr) {                         \
    for (size_t i = 0; i <= (NUM_STATES); i++) {                              \
        arr->cursors[i] = 0;                                                   \
    }                                                                          \
}                                                                              \
                                                                               \
static inline INDEX_TYPE NAME##_count(const NAME * restrict arr, size_t state) { \
    if (state >= (NUM_STATES)) return 0;                                       \
    return arr->cursors[state + 1] - arr->cursors[state];                      \
}                                                                              \
                                                                               \
static inline INDEX_TYPE NAME##_total_count(const NAME * restrict arr) {       \
    return arr->cursors[NUM_STATES];                                           \
}                                                                              \
                                                                               \
static inline TYPE* NAME##_get_range(NAME * restrict arr, size_t state, INDEX_TYPE *out_count) { \
    if (state >= (NUM_STATES)) {                                               \
        if (out_count) *out_count = 0;                                         \
        return NULL;                                                           \
    }                                                                          \
    if (out_count) {                                                           \
        *out_count = arr->cursors[state + 1] - arr->cursors[state];            \
    }                                                                          \
    return &arr->data[arr->cursors[state]];                                    \
}                                                                              \
                                                                               \
static inline TYPE* NAME##_get_at(NAME * restrict arr, size_t state, INDEX_TYPE rel_index) { \
    if (state >= (NUM_STATES)) return NULL;                                    \
    INDEX_TYPE count = arr->cursors[state + 1] - arr->cursors[state];          \
    if (rel_index >= count) return NULL;                                       \
    return &arr->data[arr->cursors[state] + rel_index];                        \
}                                                                              \
                                                                               \
/* Shifts boundaries to move an item between states in O(K) copies */          \
static inline bool NAME##_change_state(NAME * restrict arr, size_t src_state, INDEX_TYPE rel_index, size_t dst_state) { \
    if (src_state >= (NUM_STATES) || dst_state >= (NUM_STATES)) return false;  \
    INDEX_TYPE src_count = arr->cursors[src_state + 1] - arr->cursors[src_state]; \
    if (rel_index >= src_count) return false;                                  \
    if (src_state == dst_state) return true;                                   \
                                                                               \
    INDEX_TYPE curr_idx = arr->cursors[src_state] + rel_index;                 \
    TYPE target = arr->data[curr_idx];                                         \
                                                                               \
    /* Replace extracted item with the last item of src_state */               \
    INDEX_TYPE src_last = arr->cursors[src_state + 1] - 1;                     \
    arr->data[curr_idx] = arr->data[src_last];                                 \
    INDEX_TYPE hole = src_last;                                                \
                                                                               \
    if (src_state < dst_state) {                                               \
        /* Move right */                                                       \
        for (size_t k = src_state + 1; k <= dst_state; k++) {                  \
            INDEX_TYPE k_count = arr->cursors[k + 1] - arr->cursors[k];        \
            if (k_count == 1) {                                                \
                arr->data[hole] = arr->data[arr->cursors[k]];                  \
                hole = arr->cursors[k];                                        \
            } else if (k_count > 1) {                                          \
                arr->data[hole] = arr->data[arr->cursors[k]];                  \
                arr->data[arr->cursors[k]] = arr->data[arr->cursors[k + 1] - 1]; \
                hole = arr->cursors[k + 1] - 1;                                \
            }                                                                  \
            arr->cursors[k]--;                                                 \
        }                                                                      \
        arr->data[hole] = target;                                              \
    } else {                                                                   \
        /* Move left */                                                        \
        for (size_t k = src_state; k > dst_state; k--) {                       \
            INDEX_TYPE k_count = arr->cursors[k] - arr->cursors[k - 1];        \
            if (k_count == 1) {                                                \
                arr->data[hole] = arr->data[arr->cursors[k] - 1];              \
                hole = arr->cursors[k] - 1;                                    \
            } else if (k_count > 1) {                                          \
                arr->data[hole] = arr->data[arr->cursors[k] - 1];              \
                arr->data[arr->cursors[k] - 1] = arr->data[arr->cursors[k - 1]]; \
                hole = arr->cursors[k - 1];                                    \
            }                                                                  \
            arr->cursors[k]++;                                                 \
        }                                                                      \
        arr->data[hole] = target;                                              \
    }                                                                          \
    return true;                                                               \
}                                                                              \
                                                                               \
static inline bool NAME##_push(NAME * restrict arr, size_t state, TYPE item) { \
    if (state >= (NUM_STATES) || arr->cursors[NUM_STATES] >= (CAPACITY)) return false; \
    INDEX_TYPE last_idx = arr->cursors[NUM_STATES];                            \
    arr->data[last_idx] = item;                                                \
    arr->cursors[NUM_STATES]++;                                                \
    if (state < (NUM_STATES) - 1) {                                            \
        INDEX_TYPE rel_idx = (arr->cursors[NUM_STATES] - 1) - arr->cursors[(NUM_STATES) - 1]; \
        NAME##_change_state(arr, (NUM_STATES) - 1, rel_idx, state);            \
    }                                                                          \
    return true;                                                               \
}                                                                              \
                                                                               \
static inline bool NAME##_remove_at(NAME * restrict arr, size_t state, INDEX_TYPE rel_index) { \
    if (state >= (NUM_STATES)) return false;                                   \
    INDEX_TYPE count = arr->cursors[state + 1] - arr->cursors[state];          \
    if (rel_index >= count) return false;                                      \
    if (state < (NUM_STATES) - 1) {                                            \
        NAME##_change_state(arr, state, rel_index, (NUM_STATES) - 1);          \
    } else {                                                                   \
        INDEX_TYPE target_idx = arr->cursors[state] + rel_index;               \
        INDEX_TYPE last_idx = arr->cursors[NUM_STATES] - 1;                    \
        arr->data[target_idx] = arr->data[last_idx];                           \
    }                                                                          \
    arr->cursors[NUM_STATES]--;                                                \
    return true;                                                               \
}

#endif