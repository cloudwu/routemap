#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

#define MAX_EDGE 3

typedef short edge_t;
typedef unsigned short length_t;

struct road {
	length_t len;
	edge_t e[MAX_EDGE*2];
};

static inline void
road_init(struct road r[], int n) {
	memset(r, 0, sizeof(r[0]) * n);
}

static inline void
road_new(struct road r[], edge_t id, length_t len) {
	assert(id > 0);
	r[id-1].len = len;
}

static inline edge_t *
edge_get_(struct road r[], edge_t from) {
	edge_t *e;
	assert(from != 0);
	if (from < 0) {
		from = -from - 1;
		e =	r[from].e + MAX_EDGE;
	} else {
		from--;
		e =	r[from].e;
	}
	return e;
}

static inline length_t
edge_len_(struct road r[], edge_t id) {
	if (id < 0)
		return r[-id-1].len;
	else
		return r[id-1].len;
}

static inline void
road_connect_(struct road r[], edge_t from, edge_t to) {
	edge_t *e = edge_get_(r, from);
	int i;
	for (i=0;i<MAX_EDGE;i++) {
		if (e[i] == 0) {
			e[i] = to;
			return;
		}
	}
	assert(0);
}

static inline void
road_connect(struct road r[], edge_t from, edge_t to) {
	road_connect_(r, from, to);
	road_connect_(r, -to, -from);
}

struct route {
	int len;
	edge_t id;
	edge_t from;
	unsigned short index;
	unsigned char shortest;
};

static inline unsigned short
edge_index_(edge_t id) {
	assert(id != 0);
	if (id < 0) {
		return (-id * 2) - 1;
	}
	return id * 2 - 2;
}

static inline struct route *
route_fetch(struct route *r, edge_t id) {
	int index = r[edge_index_(id)].index;
	if (index == 0) {
		return NULL;
	}
	return &r[index-1];
}

static inline int
route_insert_(struct road m[], struct route r[], int sz, int n, edge_t from, int len) {
	edge_t * edge = edge_get_(m, from);
	int i;
	for (i=0;i<MAX_EDGE && edge[i];i++) {
		edge_t e = edge[i];
		int newlen = len + edge_len_(m, e);
		struct route * current = route_fetch(r, e);
		if (current == NULL) {
			assert(n < sz);
			current = &r[n++];
			current->len = newlen;
			current->id = e;
			current->from = from;
			r[edge_index_(e)].index = (unsigned short)n;
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
	memset(r, 0, sizeof(struct route) * sz);
	int n = route_insert_(m, r, sz, 0, from, 0);
	struct route * current;
	while ((current = find_shortest_(r, n))) {
		if (current->id == to) {
			return;
		}
		struct road *c = &m[edge_index_(current->id)];
		n = route_insert_(m, r, sz, n, current->id, c->len);
	}
}

/*
         1 (100)
	+-------------+
	|             |
	| 4 (30)      | 2 (10)
	|             |
	+-------------+
         3 (20)
*/

int
main() {
	struct road m[4];
	road_init(m, 4);
	road_new(m, 1, 100);
	road_new(m, 2, 10);
	road_new(m, 3, 20);
	road_new(m, 4, 30);
	road_connect(m, 1, 2);
	road_connect(m, 2, 3);
	road_connect(m, 3, 4);
	road_connect(m, 4, 1);
	road_connect(m, 2, -2);	// turn around

	struct route tmp[128];
	route_find(m, tmp, 128, 1, 3);

	// print path
	edge_t from = 1;
	edge_t to = -1;
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
