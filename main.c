#define TDIR_NEXT 1
#define TDIR_PREV 2

#define TDEBUGINFO "[DEBUGINFO] "
#define TWARNING   "[WARN] "
#define TERROR     "[ERROR] "

#ifdef TDEBUG
#define INSERT_NODE_D(n) insert_node_d(n) 
#define SET_ORIGIN(n)       origin_node_d = n
#define INSERT_ORIGIN()    INSERT_NODE_D(origin_node_d)
// Debug-only fwd declares
struct node;
void _show_refs(struct node *, struct node *);

static struct node *origin_node_d = 0;
#else
#define INSERT_NODE_D(n)  // NOP
#define SET_ORIGIN(n)     // NOP
#define INSERT_ORIGIN()   // NOP
#endif

#define INSERT_NODE(b, n)  insert_node(b, n); \
		  	   INSERT_NODE_D(n)

#define SFMTGRN "\x1B[32m"
#define SFMTBLU "\x1B[34m"
#define SFMTRST "\x1B[0m"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct node {
	uint32_t data;
	struct node *prev;
	struct node *next;
	int refc;
};

void traverse_graph_fwd(struct node *start) {
	do {
		printf("%d", start->data);
		start = start->next;
	} while(start);
	printf("\n");
}	

#ifdef TDEBUG
void insert_node_d(struct node *new) {
	if(!origin_node_d) {
		fprintf(stderr, TERROR TDEBUGINFO "Origin node was not set before INSERT_NODE macro\n");
		exit(-1);
	}
	_show_refs(new, origin_node_d);
}
#endif

void insert_node(struct node *before, struct node *new) {
	struct node *n_tmp = before->next;
	before->next = new;
	new->prev = before;
	if(n_tmp) {
		n_tmp->prev = new;
		new->next = n_tmp;
	}
	new->refc++;
	before->refc++;
}

void _show_refs(struct node *n, struct node *orig) {
	int refs = 0;
	// Traverse nodes forward from origin, to find where nodes reference the node n
	// Note that this function can only find transitive references for the given graph
	printf("\n---Showing reference trace for node containing data: %zu---\n", (size_t)n->data);
	do {
		if(orig->next == n) {
			printf(TDEBUGINFO SFMTGRN "%p (%zu)" SFMTRST " precedes " SFMTBLU "%p (%zu)" SFMTRST "\n", (void *)orig, (size_t)orig->data, (void *)n, (size_t)n->data);
			refs++;
		}
		if(orig->prev == n) {
			printf(TDEBUGINFO SFMTGRN "%p (%zu)" SFMTRST " succeeds " SFMTBLU "%p (%zu)" SFMTRST "\n", (void *)orig, (size_t)orig->data, (void *)n, (size_t)n->data);
			refs++;
		}
		orig = orig->next;
	} while(orig);

	if(!refs) {
		printf("\tNo refs yet; first node in graph?\n");
	}
}

void remove_node(struct node *n) {
	if(n->next) {
		n->next->prev = n->prev;
		n->next->refc--;
	}
	if(n->prev) {
		n->prev->next = n->next;
		n->prev->refc--;
	}
	n->next = n->prev = 0;
	n->refc -= 2;
}

void _delete_node(struct node *n) {
	if(n->refc) {
		fprintf(stderr, "[NT][WARN] Trying to delete node with non-zero reference count\n");
	}
	// This is just a helper to ensure that if a heap-based node need be deleted, we first verify that ref count is non-zero
}

int main() {
	struct node first = {
		.data = 1 };
	struct node second = {
		.data = 2 };
	struct node third = {
		.data = 3 };
	struct node fourth = {
		.data = 4 };
	struct node fifth = {
		.data = 5 };
	
	SET_ORIGIN(&first);

	INSERT_ORIGIN();
	INSERT_NODE(&first, &third);
	INSERT_NODE(&first, &second);
	INSERT_NODE(&third, &fourth);
	INSERT_NODE(&fourth, &fifth);

	traverse_graph_fwd(&first);

	return 0;
}
