#include "rpi.h"
#include <stdlib.h>

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

void RemoveNListTimer(ListTimer* lst, int n) {
  ListTimerItem *l, *tmp;
  l = lst->head;

  for (int i = 0; i < n; i++) {
    tmp = l;
    l = l->next;
    free(tmp);
    lst->count--;
  }
  lst->head = l;
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

TIMER* TimerAt(ListTimer* lst, int index) {
  ListTimerItem* l = lst->head;
  for (int i = 0; i < index; i++) {
    l = l->next;
  }
  return &(l->timer);
}

TIMER* TimerForId(ListTimer* lst, unsigned int id) {
  ListTimerItem* l = lst->head;
  for (l = lst->head; NULL != l; l = l->next) {
    if (l->timer.id == id) {
      return &(l->timer);
    }
  }
  return NULL;
}

int CountListTimer(ListTimer* lst) { return lst->count; }
