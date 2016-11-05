#pragma once

#define LIST(type, name) \
  struct { \
    type *prev; \
    type *next; \
  } name;

#define LIST_HEAD_INIT(name) \
  name.prev = name.next = 0;

#define LIST_INIT(self, list) \
    self->list.prev = self->list.next = 0;

#define LIST_APPEND(head, self, list) \
  do { \
    self->list.prev = head.prev; \
    self->list.next = 0; \
    if(self->list.prev) { \
      self->list.prev->list.next = self; \
    } \
    head.prev = self; \
    if(!head.next){head.next = head.prev;} \
  } while(0);

#define LIST_REMOVE(head, self, list) \
  do { \
    if(head.next == self) { \
      head.next = self->list.next; \
    } \
    if(head.prev == self) { \
      head.prev = self->list.prev; \
    } \
    if(self->list.next) { \
      self->list.next->list.prev = self->list.prev; \
    } \
    if(self->list.prev) { \
      self->list.prev->list.next = self->list.next; \
    } \
    self->list.prev = self->list.next = 0; \
  } while(0);

#define LIST_EMPTY(head) ((head.next) == 0)
#define LIST_FIRST(head) (head.next)

#define LIST_FOREACH(head, type, name, list) \
  for(type *name = head.next; name; name = name->list.next)

#define LIST_INSERT_BEFORE(head, self, list, after) \
  do { \
    self->list.next = after; \
    self->list.next = after->list.prev; \
    after->list.prev = self; \
    if(self->list.prev) \
      self->list.prev->list.next = self; \
    if(head.next == after) \
      head.next = self; \
  } while(0);

#define LIST_INSERT_AFTER(head, self, list, before) \
  do { \
    self->list.prev = before; \
    self->list.next = before->list.next; \
    before->list.next = self; \
    if(self->list.next) \
      self->list.next->list.prev = self; \
    if(head->prev == before) \
      head.prev = self; \
  } while(0);
