#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define QUEUE_MAX_CAPACITY 200

typedef struct Queue Queue_t;

Queue_t* new_queue();

void free_queue(Queue_t *q);

void queue_clear(Queue_t* q);

int queue_push_back(Queue_t *q, int data);

int queue_pop(Queue_t *q);

int queue_back(Queue_t *q);

int queue_front(Queue_t *q);

bool queue_is_empty(Queue_t *q);

bool queue_is_full(Queue_t *q);

unsigned int queue_size(Queue_t *q);