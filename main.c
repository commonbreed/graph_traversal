#define TDIR_NEXT 1
#define TDIR_PREV 2

#define TDEBUGINFO L"[DEBUGINFO] "
#define TWARNING   L"[WARN] "
#define TERROR     L"[ERROR] "

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

#define WCHARWRITE(ADDR, UCODE)     snprintf(ADDR, 2, "\u" #UCODE)
#define WNCHARWRITE(ADDR, UCODE, N) for(int _i = 0; _i < N; _i+=2) WCHARWRITE(ADDR+_i, UCODE)
#define WNSTRWRITE(ADDR, FMT, DATA, N) snprintf(ADDR, N, FMT, DATA)

#define SFMTGRN L"\x1B[32m"
#define SFMTBLU L"\x1B[34m"
#define SFMTRST L"\x1B[0m"
#define GT_ARROW_RIGHT L"-->"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <locale.h>
#include <wchar.h>
#include <signal.h>
#include <unistd.h>

#include "litmus/litmus_unicode.h"
#include "util/box_drawing.h"
#include "util/drawing.h"

struct node {
	uint32_t data;
	struct node *prev;
	struct node *next;
	int refc;
};

size_t _wrap_data_ascii(struct node *n, wchar_t *so, int arrow);

void traverse_graph_fwd(struct node *start) {
	do { 
		wchar_t so[100] = {0}, *buffer;
		size_t nlens;
		nlens = _wrap_data_ascii(start, so, (int)start->next);
		__before_place_next_node(nlens);

		// Handle new-lines using __node_cr
		wchar_t *token;
		token = wcstok(so, L"\n", &buffer);
		while(token != NULL) {
			wprintf(L"%ls", token);
			token = wcstok(NULL, L"\n", &buffer);
			__node_cr(nlens);
		}

		wprintf(L"%ls", so);
		start = start->next;
	} while(start);
	wprintf(L"\n");
}	

#ifdef TDEBUG
void insert_node_d(struct node *new) {
	if(!origin_node_d) {
		wprintf(TERROR TDEBUGINFO L"Origin node was not set before INSERT_NODE macro\n");
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
		n_tmp->refc++;
		new->refc++;
	}
	new->refc++;
	before->refc++;
}

void _show_refs(struct node *n, struct node *orig) {
	int refs = 0;
	// Traverse nodes forward from origin, to find where nodes reference the node n
	// Note that this function can only find transitive references for the given graph
	wprintf(L"\n---Showing reference trace for node containing data: %zu---\n", (size_t)n->data);
	wprintf(L"\tThis node self-reports %d inbound reference(s).\n", n->refc);
	do {
		if(orig->next == n) {
			wprintf(L"\t" TDEBUGINFO SFMTGRN "%p (%zu)" SFMTRST " precedes " SFMTBLU "%p (%zu)" SFMTRST "\n", (void *)orig, (size_t)orig->data, (void *)n, (size_t)n->data);
			refs++;
		}
		if(orig->prev == n) {
			wprintf(L"\t" TDEBUGINFO SFMTGRN "%p (%zu)" SFMTRST " succeeds " SFMTBLU "%p (%zu)" SFMTRST "\n", (void *)orig, (size_t)orig->data, (void *)n, (size_t)n->data);
			refs++;
		}
		orig = orig->next;
	} while(orig);

	if(!refs) {
		wprintf(L"\tNo refs yet; first node in graph?\n");
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
		wprintf(L"[NT][WARN] Trying to delete node with non-zero reference count\n");
	}
	// This is just a helper to ensure that if a heap-based node need be deleted, we first verify that ref count is non-zero
}

char *_string_builder() {

}

// Wraps the data inside a node in an ASCII "circle"
size_t _wrap_data_ascii(struct node *n, wchar_t *so, int arrow) {
	int cidx = 0;
#define WINSERT(val) so[cidx] = val; cidx++
#define WNFILL(val) for(int _i = 0; _i < nlens; _i++)\
				so[cidx+_i] = val;\
				cidx+=nlens
#define WSTRINSERT() cidx+=swprintf(&so[cidx], nlens+1, L"%zu", (size_t)n->data)
	wchar_t s[10];
	swprintf(s, 10, L"%zu", (size_t)n->data);
	size_t nlens = wcslen(s);
	
	WINSERT(GT_BD_TOPLEFT); 	// Top-left light box-drawing
	WNFILL(GT_BD_HORIZONTAL);	// Horizontal light box-drawing
	WINSERT(GT_BD_TOPRIGHT);
	WINSERT(L'\n');
	WINSERT(GT_BD_VERTICAL); // Vertical light box-drawing
	
	// Insert data
	WSTRINSERT();

	WINSERT(GT_BD_VERTICAL);
	if(arrow) {
		WINSERT(L'-');
		WINSERT(L'-');
		WINSERT(L'>');
	}
	WINSERT(L'\n');
	WINSERT(GT_BD_BOTTOMLEFT);
	WNFILL(GT_BD_HORIZONTAL);
	WINSERT(GT_BD_BOTTOMRIGHT);

//raise(SIGTRAP);
	return nlens;
}

int main() {
	__init_ncurses();
	struct node first = {
		.data = 1 };
	struct node second = {
		.data = 2 };
	struct node third = {
		.data = 3 };
	struct node fourth = {
		.data = 43 };
	struct node fifth = {
		.data = 5 };
	
	SET_ORIGIN(&first);

	INSERT_ORIGIN();
	INSERT_NODE(&first, &fifth);
	INSERT_NODE(&first, &fourth);
	INSERT_NODE(&first, &third);
	INSERT_NODE(&first, &second);

	traverse_graph_fwd(&first);
	sleep(5);
	__end_ncurses();
	return 0;
}
