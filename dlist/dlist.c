/*
 * =====================================================================================
 *
 *       Filename:  dlist.c
 *
 *    Description:  Double Linked List
 *
 *        Version:  1.0
 *        Created:  2015年05月10日 21时58分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dercury (Jim), dercury@qq.com
 *   Organization:  Perfect World
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dlist.h"

typedef struct tagDListNode
{
    struct tagDListNode* prev;
    struct tagDListNode* next;
    void*                data;
} DListNode;

struct tagDList
{
    DListNode* head;
    DListNode* tail;
    FreeFunc   data_free_func;
    size_t     size;
};

DList* dlist_create(FreeFunc data_free_func)
{
    DList* list = (DList*)malloc(sizeof(DList));
    if (NULL == list)
    {
        exit(EXIT_FAILURE);
    }
    
    (void)memset(list, 0, sizeof(DList));
    list->data_free_func = data_free_func;

    return list;
}

void dlist_destroy(DList** list)
{
    if (NULL == list || NULL == *list)
    {
        return;
    }

    while (0 < (*list)->size)
    {
        dlist_remove_head(*list, TRUE, NULL);
    }

    free(*list);
    *list = NULL;

    return;
}

int main(int argc, char** argv)
{
    return EXIT_SUCCESS;
}

