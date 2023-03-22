/**
 * @file fifo.h
 * @brief Thread-safe FIFO implementation with array_t
 */

#ifndef FIFO_H
#define FIFO_H

/**
 * @brief Array structure for FIFO buffer
 */
typedef struct {
    int* array; /**< A pointer to the array. */
    int size; /**< The size of the array. */
    int direction; /**< The direction of the array. */
} array_t;

/**
 * @brief FIFO buffer structure
 */
typedef struct {
    array_t* buffer; /**< Pointer to the buffer array. */
    int start; /**< Index of the start of the buffer. */
    int end; /**< Index of the end of the buffer. */
    int count; /**< Number of items in the buffer. */
    int buf_size; /**< Size of the buffer. */
    pthread_mutex_t lock; /**< Mutex for thread safety. */
    pthread_cond_t not_empty; /**< Condition variable for not empty. */
    pthread_cond_t not_full; /**< Condition variable for not full. */
} fifo_t;

/**
 * @brief Initializes the FIFO buffer.
 * 
 * @param fifo Pointer to the FIFO buffer.
 * @param buffer_size Size of the buffer.
 */
void fifo_init(fifo_t *fifo, int buffer_size);

/**
 * @brief Adds an array to the FIFO buffer.
 * 
 * @param fifo Pointer to the FIFO buffer.
 * @param array Pointer to the array.
 */
void fifo_push(fifo_t *fifo, array_t* array);

/**
 * @brief Removes an array from the FIFO buffer.
 * 
 * @param fifo Pointer to the FIFO buffer.
 * @return The array removed from the buffer.
 */
array_t fifo_pop(fifo_t *fifo);

/**
 * @brief Destroys the FIFO buffer.
 * 
 * @param fifo Pointer to the FIFO buffer.
 */
void fifo_destroy(fifo_t *fifo);

#endif /* FIFO_H */