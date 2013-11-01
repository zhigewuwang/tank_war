#include <stdlib.h>
#include <memory.h>
#include "list.h"

#define MIN_CAPACITY 16

struct listimpl {
	struct list sup;

	int capacity;
	int len;
	void **buf;
};

// --------------------private
static void expand(struct listimpl *impl, int add_len) { 
	int tocap = impl->capacity + add_len;

	while ((impl->capacity *= 2) < tocap) 
		;

	void **buf = (void **)malloc(sizeof(void *) * impl->capacity);
	memcpy(buf, impl->buf, impl->len * sizeof(void *));		
	free(impl->buf);
	impl->buf = buf;
}

// --------------------protected
static void clean(struct list **ins) {
	struct listimpl *impl = (struct listimpl *)*ins;
	free(impl->buf);	
	free(impl);	
	*ins = 0;
}
static int get_len(struct list *ins) {
	return ((struct listimpl *)ins)->len;
}

static void add(struct list *ins, int add_pos, void *data) {
	struct listimpl *impl = (struct listimpl *)ins;
	int i;

	if (impl->len + 1 > impl->capacity) 
		expand(impl, 1);
		
	for (i = impl->len + 1; i >= add_pos + 1; i--)
		impl->buf[i] = impl->buf[i - 1];

	impl->buf[add_pos] = data;

	impl->len++;
}

static void *get(struct list *list, int pos) {
	return ((struct listimpl *)list)->buf[pos];
}
static void * del(struct list *ins, int pos) {
	struct listimpl *impl = (struct listimpl *)ins;
	int i;

	if (pos < 0 || pos > impl->len) return;

	void *delData = impl->buf[pos];
	for (i = pos + 1; i < impl->len; i++) 
		impl->buf[i - 1] = impl->buf[i];

	impl->len--;
	impl->buf[impl->len] = 0;

	return delData;
}


// --------------------public 
struct list *new_list(int capacity) {
	struct listimpl *impl = (struct listimpl *)malloc(sizeof(struct listimpl));
	
	if (capacity < MIN_CAPACITY) capacity = MIN_CAPACITY;

	impl->buf = (void **)malloc(sizeof(void *) * capacity);
	impl->capacity = capacity;	
	impl->len = 0;	

	impl->sup.clean = clean;
	impl->sup.len = get_len;
	impl->sup.add = add;
	impl->sup.get = get;
	impl->sup.del = del;
	
	return (struct list*)impl;
}
