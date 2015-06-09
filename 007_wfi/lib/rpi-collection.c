#include "rpi.h"
#include <stdlib.h>

ListTimer listTimer;

void InitListTimer(ListTimer* lst) { lst->head = NULL; }

// insert based on timer->timeout
void InsertListTimer(ListTimer* lst, TIMER* timer) {
  ListTimerItem* item = (ListTimerItem*)malloc(sizeof(ListTimerItem));
  item->timer = *timer;
  item->next = NULL;

  if (NULL == lst->head) {
    item->next = NULL;
    lst->head = item;
    return;
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
      return;
    }
    prev = l;
  }
  prev->next = item;
}

void RemoveListTimer(ListTimer* lst, TIMER* timer) {}

TIMER TimerAt(ListTimer* lst, int index) {
  ListTimerItem* l = lst->head;
  for (int i = 0; i < index; i++) {
    l = l->next;
  }
  return l->timer;
}

int CountListTimer(ListTimer* lst) {
  int count = 0;
  ListTimerItem* l = lst->head;

  if (NULL == l) {
    return 0;
  }
  count++;

  for (; NULL != (l->next); l = l->next) {
    count++;
  }
  return count;
}
