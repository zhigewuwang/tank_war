#ifndef LIST_H
#define LIST_H

struct list {
	void (*clean)(struct list **list);

	int (*len)(struct list *list);

	void (*add)(struct list *list, int add_pos, void *e);
	void *(*get)(struct list *list, int pos);
	void * (*del)(struct list *list, int pos);
};

struct list *new_list(int capacity);







#endif
