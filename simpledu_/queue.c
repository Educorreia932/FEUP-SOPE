#include "queue.h"

typedef struct node {
    int data;
    struct node * next;
} node_t;

struct Queue {
    node_t *front, *rear;
    unsigned int size;
};

Queue_t* new_queue() {
    Queue_t *q = (Queue_t*) calloc(1, sizeof(Queue_t));
    
    if (q == NULL) {
        printf("new_queue: Failed to create the new queue\n");
        return NULL;
    }

    q->size = 0;
    // front and rear are already NULL
    return q;
}

void free_queue(Queue_t* q) {
    queue_clear(q);
    free(q);
}

void queue_clear(Queue_t* q) {
    node_t *cur_node = q->front, *next_node;
    if (q->front == q->rear) {
        free(q->front);
    }
    else {
        while (cur_node != NULL) {
            next_node = cur_node->next;
            free(cur_node);
            cur_node = next_node;
        }
    }
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
}

int queue_push_back(Queue_t *q, int data) {
    node_t * newNode = NULL;
    // Check queue out of capacity error
    if (queue_is_full(q))
        return 1;

    // Create a new node of queue type
    newNode = (node_t *) malloc (sizeof(node_t));

    // Assign data to new node
    newNode->data = data;
    
    // Initially new node does not point anything
    newNode->next = NULL;
    
    // Link new node with existing last node 
    if (q->rear != NULL)
        q->rear->next = newNode;
    
    // Make sure newly created node is at rear
    q->rear = newNode;

    // Link first node to front if its NULL
    if ( q->front == NULL )
        q->front = q->rear;

    // Increment queue size
    q->size++;

    return 0;
}
/**
 * Dequeues/Removes an element from front of the queue.
 * It returns the element on success otherwise returns 
 * 0xFF as error code.
 */
int queue_pop(Queue_t *q)
{
    node_t *toDequque = NULL;
    int data = 0xFF;
    // Queue empty error
    if (queue_is_empty(q))
        return 0xFF;

    // Get element and data to dequeue
    toDequque = q->front;
    data = toDequque->data;

    // Move front ahead
    q->front = q->front->next;

    // Decrement size
    --q->size;

    // If the queue is now empty, set the rear to NULL
    if (q->size == 0)
        q->rear = NULL;

    // Clear dequeued element from memory
    free(toDequque);
    
    return data;
}

int queue_back(Queue_t *q) {
    // Return INT_MIN if queue is empty otherwise rear.
    return (queue_is_empty(q)) ? 0xFF : q->rear->data;
}

int queue_front(Queue_t *q) {
    // Return 0xFF if queue is empty otherwise front.
    return (queue_is_empty(q)) ? 0xFF : q->front->data;
}

bool queue_is_empty(Queue_t *q) {
    return (q->size <= 0);
}

bool queue_is_full(Queue_t *q) {
    return (q->size >= QUEUE_MAX_CAPACITY);
}

unsigned int queue_size(Queue_t *q) {
    return q->size;
}