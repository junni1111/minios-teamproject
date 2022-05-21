#include "main.h"
#include <stdio.h>
#include <stdlib.h>

// stack function
int is_empty(Stack *dirStack)
{
    if (dirStack->TopNode == NULL)
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

int push(Stack *dirStack, char *dirName)
{
    // variables
    StackNode *dirNode = (StackNode *)malloc(sizeof(StackNode));

    if (dirStack == NULL)
    {
        printf("error occurred, dirStack.\n");
        return -1;
    }
    if (dirNode == NULL)
    {
        printf("error occurred, dirNode.\n");
        return -1;
    }
    // set dirNode
    strncpy(dirNode->name, dirName, MAX_NAME_SIZE);
    dirNode->LinkNode = dirStack->TopNode;
    // set dirStack
    dirStack->TopNode = dirNode;

    return 0;
}

char *pop(Stack *dirStack)
{
    // variables
    StackNode *returnNode = NULL;
    // error
    if (dirStack == NULL)
    {
        printf("error occurred, dirStack.\n");
        return NULL;
    }
    if (is_empty(dirStack) == -1)
    {
        printf("Stack Empty.\n");
        return NULL;
    }
    // set returnNode
    returnNode = dirStack->TopNode;
    // set dirStack
    dirStack->TopNode = returnNode->LinkNode;

    return returnNode->name;
}
