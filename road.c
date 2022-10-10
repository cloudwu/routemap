#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

#define MAX_EDGE 3
#define INVALID_EDGE 0xffff

typedef unsigned short edge_t;
typedef unsigned short length_t;

struct road {
	length_t len;
	edge_t e[MAX_EDGE];
};

static inline void
road_init(struct road r[], int n) {
	int i,j;
	for (i=0;i<n;i++) {
		r[i].len = 0;
		for (j=0;j<MAX_EDGE;j++) {
			r[i].e[j] = INVALID_EDGE;
		}
	}
}

static inline void
road_new(struct road r[], edge_t id, length_t len) {
	r[id].len = len;
}

static inline void
road_connect(struct road r[], edge_t from, edge_t to) {
	edge_t *e = r[from].e;
	int i;
	for (i=0;i<MAX_EDGE;i++) {
		if (e[i] == INVALID_EDGE) {
			e[i] = to;
			return;
		}
	}
	assert(0);
}

struct route {
	int len;
	edge_t id;
	edge_t from;
	edge_t index;
	unsigned char shortest;
};

static inline struct route *
route_fetch(struct route *r, edge_t id) {
	edge_t index = r[id].index;
	if (index == INVALID_EDGE) {
		return NULL;
	}
	assert(r[index].id == id);
	return &r[index];
}

static inline int
route_insert_(struct road m[], struct route r[], int sz, int n, edge_t from, int len) {
	edge_t * edge = m[from].e;
	int i;
	for (i=0;i<MAX_EDGE && edge[i] != INVALID_EDGE;i++) {
		edge_t e = edge[i];
		int newlen = len + m[e].len;
		struct route * current = route_fetch(r, e);
		if (current == NULL) {
			assert(n < sz);
			current = &r[n];
			current->len = newlen;
			current->id = e;
			current->from = from;
			r[e].index = (edge_t)n;
			++n;
		} else if (newlen < current->len) {
			current->len = newlen;
			current->from = from;			
		}
	}
	return n;
}

static inline struct route *
find_shortest_(struct route *r, int n) {
	int i;
	int min_index;
	int min_length = INT_MAX;
	for (i=0;i<n;i++) {
		if (!r[i].shortest && r[i].len < min_length) {
			min_length = r[i].len;
			min_index = i;
		}
	}
	if (min_length < INT_MAX) {
		struct route * s = &r[min_index];
		s->shortest = 1;
		return s;
	}
	return NULL;
}

static inline void
route_find(struct road m[], struct route r[], int sz, edge_t from, edge_t to) {
	int i;
	for (i=0;i<sz;i++) {
		r[i].shortest = 0;
		r[i].index = INVALID_EDGE;
	}
	int n = route_insert_(m, r, sz, 0, from, 0);
	struct route * current;
	while ((current = find_shortest_(r, n))) {
		if (current->id == to) {
			return;
		}
		struct road *c = &m[current->id];
		n = route_insert_(m, r, sz, n, current->id, c->len);
	}
}

/*
         0,4 (100)
	+-------------+                 -> 0  |     |  4 <- 
	|             |                 3     v	    v     5     1 5
	| 3,7 (30)    | 1,5 (10)        ^     1	    7     ^     | ^      6 <-+ 
	|             |                 |  2 <-     -> 6  |     +-+      2 --+
	+-------------+
         2,6 (20)
*/

int
main() {
	struct road m[8];
	road_init(m, 8);
	road_new(m, 0, 100);
	road_new(m, 1, 10);
	road_new(m, 2, 20);
	road_new(m, 3, 30);

	road_new(m, 4, 100);
	road_new(m, 5, 10);
	road_new(m, 6, 20);
	road_new(m, 7, 30);

	road_connect(m, 0, 1);
	road_connect(m, 1, 2);
	road_connect(m, 2, 3);
	road_connect(m, 3, 0);

	road_connect(m, 4, 7);
	road_connect(m, 7, 6);
	road_connect(m, 6, 5);
	road_connect(m, 5, 4);
	
	road_connect(m, 1, 5);
	road_connect(m, 2, 6);	// turn around

	struct route tmp[128];
	route_find(m, tmp, 128, 0, 4);

	// print path
	edge_t from = 0;
	edge_t to = 4;
	edge_t checkpoint = to;
	while (checkpoint != from) {
		struct route *e = route_fetch(tmp, checkpoint);
		if (e == NULL)
			return 1;
		printf("%d < ", checkpoint);
		checkpoint = e->from;
	}
	printf("%d \n", from);
	return 0;
}
