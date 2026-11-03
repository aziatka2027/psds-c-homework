#include "cstack.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACKS 16

struct node
{
    struct node* prev;
    unsigned int size;
    char data[];
};

struct stack_info
{
    struct node* top;
    unsigned int count;
    int occupied;
};

static struct stack_info stacks[MAX_STACKS];

#define UNUSED(VAR) (void)(VAR)

hstack_t stack_new(void)
{
    for (int i = 0; i < MAX_STACKS; ++i)
    {
        if (!stacks[i].occupied)
        {
            stacks[i].occupied = 1;
            stacks[i].count = 0;
            stacks[i].top = NULL;
            return i;
        }
    }
    return -1;
}

void stack_free(const hstack_t hstack)
{
    if (!stack_valid_handler(hstack))
    {
        return;
    }
    while (stacks[hstack].top != NULL)
    {
        struct node* current = stacks[hstack].top;
        stacks[hstack].top = current->prev;
        free(current);
    }
    stacks[hstack].count = 0;
    stacks[hstack].occupied = 0;
}

void stack_free_all(void)
{
    for (int i = 0; i < MAX_STACKS; i++)
    {
        stack_free(i);
    }
}

int stack_valid_handler(const hstack_t hstack)
{
    if (hstack < 0 || hstack >= MAX_STACKS)
    {
        return 0;
    }
    return stacks[hstack].occupied;
}

unsigned int stack_size(const hstack_t hstack)
{
    if (!stack_valid_handler(hstack))
    {
        return 0;
    }
    return stacks[hstack].count;
}

void
stack_push(const hstack_t hstack, const void* data_in, const unsigned int size)
{
    if (!stack_valid_handler(hstack) || data_in == NULL || size == 0)
    {
        return;
    }
    struct node* new_node = malloc(sizeof(struct node) + size);
    if (new_node == NULL)
    {
        return;
    }
    new_node->size = size;
    memcpy(new_node->data, data_in, size);

    new_node->prev = stacks[hstack].top;
    stacks[hstack].top = new_node;

    ++stacks[hstack].count;
}

unsigned int
stack_pop(const hstack_t hstack, void* data_out, const unsigned int size)
{
    if (!stack_valid_handler(hstack) || data_out == NULL ||
        stacks[hstack].count == 0 || size < stacks[hstack].top->size)
    {
        return 0;
    }
    unsigned int current_size = stacks[hstack].top->size;
    struct node* current = stacks[hstack].top;
    memcpy(data_out, current->data, current_size);
    stacks[hstack].top = current->prev;
    free(current);
    --stacks[hstack].count;
    return current_size;
}

unsigned stack_top_size(const hstack_t hstack)
{
    if (!stack_valid_handler(hstack) || stacks[hstack].top == NULL)
    {
        return 0;
    }
    return stacks[hstack].top->size;
}

hstack_t stack_get_all(void)
{
    hstack_t new_stack = stack_new();

    if (new_stack == -1)
    {
        return -1;
    }

    for (int i = 0; i < MAX_STACKS; ++i)
    {
        if (stack_valid_handler(i) && i != new_stack)
        {
            unsigned int count = stack_size(new_stack);

            stack_push(new_stack, &i, sizeof(i));

            if (stack_size(new_stack) == count)
            {
                stack_free(new_stack);
                return -1;
            }
        }
    }

    return new_stack;
}
