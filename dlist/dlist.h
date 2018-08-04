/*
 * =====================================================================================
 *
 *       Filename:  dlist.h
 *
 *    Description:  Double Linked List API
 *
 *        Version:  1.0
 *        Created:  2015年05月10日 20时52分45秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dercury (Jim), dercury@qq.com
 *   Organization:  Perfect World
 *
 * =====================================================================================
 */

#ifndef DLIST_H
#define DLIST_H

struct tagDList;
typedef struct tagDList DList;

typedef enum tagBool
{
    FALSE = 0,
    TRUE = 1
} Bool;

typedef void (*FreeFunc)(void** data);
typedef int (*CmpFunc)(void* data0, void* data1);
typedef char* (*ToStrFunc)(void* data);
typedef void (*VisitFunc)(void* data);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  dlist_create
 *  Description:  create a dlist
 * =====================================================================================
 */
DList* dlist_create(FreeFunc data_free_func);
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  dlist_destroy
 *  Description:  destroy specified dlist
 * =====================================================================================
 */
void dlist_destroy(DList** list);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  dlist_get_size
 *  Description:  get size of specified dlist
 * =====================================================================================
 */
size_t dlist_get_size(DList* list);
FreeFunc dlist_get_data_free_func(DList* list);
void dlist_set_data_free_func(DList* list, FreeFunc data_free_func);

void dlist_insert_head(DList* list, void* data);
void dlist_remove_head(DList* list, Bool need_free, void* data);

void dlist_append_tail(DList* list, void* data);
void dlist_remove_tail(DList* list, Bool need_free, void* data);

void dlist_insert_by_value(DList* list, void* data, CmpFunc data_cmp_func);
void dlist_append_by_value(DList* list, void* data, CmpFunc data_cmp_func);
void dlist_remove_by_value(DList* list, Bool need_free, void* data, CmpFunc data_cmp_func);
void dlist_get_by_value(DList* list, void* keyfield, CmpFunc data_cmp_func);
void dlist_set_by_value(DList* list, void* keyfield, void* new_data, CmpFunc data_cmp_func);

void dlist_foreach(DList* list, VisitFunc data_visit_func);
void dlist_to_string(DList* list, ToStrFunc data_to_str_func, char* str_buff, size_t* str_len);

void dlist_sort(DList* list, CmpFunc data_cmp_func);

#endif /* DLIST_H */

