#include "main.h"
#include <stdio.h>
#include <stdlib.h>

// stack function
int is_empty(Stack *p_directoryStack)
{
    if (p_directoryStack->TopNode == NULL)
    {
        return -1;
    }
    return 0;
}

Stack *initialize_stack()
{
    // variables
    Stack *returnStack = (Stack *)malloc(sizeof(Stack));

    if (returnStack == NULL)
    {
        printf("error occurred, returnStack.\n");
        return NULL;
    }
    // initialize Stack
    // returnStack->ptr = 0;
    returnStack->TopNode = NULL;

    return returnStack;
}

int push(Stack *p_directoryStack, char *directoryName)
{
    // variables
    StackNode *dirNode = (StackNode *)malloc(sizeof(StackNode));

    if (p_directoryStack == NULL)
    {
        printf("error occurred, p_directoryStack.\n");
        return -1;
    }
    if (dirNode == NULL)
    {
        printf("error occurred, dirNode.\n");
        return -1;
    }
    // set dirNode
    strncpy(dirNode->name, directoryName, MAX_NAME_SIZE);
    dirNode->LinkNode = p_directoryStack->TopNode;
    // set p_directoryStack
    p_directoryStack->TopNode = dirNode;

    return 0;
}

char *pop(Stack *p_directoryStack)
{
    // variables
    StackNode *returnNode = NULL;
    // error
    if (p_directoryStack == NULL)
    {
        printf("error occurred, p_directoryStack.\n");
        return NULL;
    }
    if (is_empty(p_directoryStack) == -1)
    {
        printf("Stack Empty.\n");
        return NULL;
    }
    // set returnNode
    returnNode = p_directoryStack->TopNode;
    // set p_directoryStack
    p_directoryStack->TopNode = returnNode->LinkNode;

    return returnNode->name;
}
