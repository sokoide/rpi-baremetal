#include "rpi.h"
#include <stdlib.h>

ListTimer listTimer;

void InitListTimer(ListTimer* lst) {
  lst->head = NULL;
  lst->count = 0;
}

// insert based on timer->timeout
int InsertListTimer(ListTimer* lst, TIMER* timer) {
  ListTimerItem* item = (ListTimerItem*)malloc(sizeof(ListTimerItem));
  item->timer = *timer;
  item->timer.id = lst->count;
  item->next = NULL;
  lst->count++;

  if (NULL == lst->head) {
    item->next = NULL;
    lst->head = item;
    return item->timer.id;
  }

  ListTimerItem *l, *prev;
  for (l = lst->head, prev = NULL; NULL != l; l = l->next) {
    if (l->timer.timeout >= item->timer.timeout) {
      if (NULL == prev) {
        lst->head = item;
        item->next = l;
      } else {
        prev->next = item;
        item->next = l;
      }
      return item->timer.id;
    }
    prev = l;
  }
  prev->next = item;
  return item->timer.id;
}

bool RemoveListTimer(ListTimer* lst, int id) {
  ListTimerItem *l, *prev;

  for (l = lst->head, prev = NULL; NULL != l; l = l->next) {
    if (l->timer.id == id) {
      if (NULL == prev) {
        lst->head = l->next;
      } else {
        prev->next = l->next;
      }
      lst->count--;
      free(l);
      return true;
    }
    prev = l;
  }
  return false;
}

void CleanupListTimer(ListTimer* lst) {
  ListTimerItem* l = lst->head;
  ListTimerItem* prev = NULL;

  for (; NULL != l; l = l->next) {
    if (NULL != prev) {
      free(prev);
    }
    prev = l;
  }
  if (NULL != prev) {
    free(prev);
  }
  lst->count = 0;
}

TIMER TimerAt(ListTimer* lst, int index) {
  ListTimerItem* l = lst->head;
  for (int i = 0; i < index; i++) {
    l = l->next;
  }
  return l->timer;
}

int CountListTimer(ListTimer* lst) {
  return lst->count;
  /* int count = 0; */
  /* ListTimerItem* l = lst->head; */

  /* if (NULL == l) { */
  /*   return 0; */
  /* } */
  /* count++; */

  /* for (; NULL != (l->next); l = l->next) { */
  /*   count++; */
  /* } */
  /* return count; */
}