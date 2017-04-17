#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"
#include "schema.h"	 
#include <assert.h>
#include "enum.h"
#include "tuple.h"
#include <limits.h>
typedef struct {
    int vertex;
    int weight;
} edge_dij;
 
typedef struct {
    edge_dij **edges;
    int edges_len;
    int edges_size;
    int dist;
    int prev;
    int visited;
} vertex_dij;
 
typedef struct {
    vertex_dij **vertices;
    int vertices_len;
    int vertices_size;
} graph_dij;
 
typedef struct {
    int *data;
    int *prio;
    int *index;
    int len;
    int size;
} heap_t;
 
int tuple_print_attr(tuple_t t, enum_list_t el, char * atr);
void print_path (graph_dij *g, int i);
void add_vertex (graph_dij *g, int i);
void add_edge_dij(graph_dij *g, int a, int b, int w);
heap_t *create_heap (int n);
void push_heap (heap_t *h, int v, int p);
int min (heap_t *h, int i, int j, int k);
int pop_heap (heap_t *h);
void dijkstra (graph_dij *g, int a, int b);

void
cli_graph_shortpath(char *cmdline, int *pos)
{
	char s1[BUFSIZE], s2[BUFSIZE], s3[BUFSIZE], s4[BUFSIZE];
	memset(s1, 0, BUFSIZE);
	nextarg(cmdline, pos, " ", s1);
	memset(s2, 0, BUFSIZE);
	nextarg(cmdline, pos, " ", s2);
	memset(s3, 0, BUFSIZE);
	nextarg(cmdline, pos, " ", s3);
	memset(s4, 0, BUFSIZE);
	nextarg(cmdline, pos, " ", s4);
#if _DEBUG
	printf("s1=[%s] s2=[%s] s3=[%s], s4=[%s]\n", s1, s2, s3, s4);
#endif
	vertex_t s,d;
	vertexid_t id1,id2;
	id1 = (vertexid_t) atoi(s1);
	id2 = (vertexid_t) atoi(s2);
	s = graph_find_vertex_by_id(current, id1);
	d = graph_find_vertex_by_id(current, id2);
	if(s == NULL || d == NULL){
		printf("Illegal vertex ID\n");
		return;
	}
	edge_t e;
	int f=0;
	graph_dij *g = calloc(1, sizeof (graph_dij));
//	assert (current != NULL);
	for (e = current->e; e != NULL; e = e->next)
		{
		if (e->id1 == id1){
			edge_t temp = e;
			while(e != NULL)
			{
				//edge_print(e);
				attribute_t attr;
				int wt;
				tuple_t t;
				t=e->tuple;
				wt=tuple_print_attr(t, current->el,s3);
				attr = schema_find_attr_by_name(t->s, s3);
				add_edge_dij(g, e->id1, e->id2, wt);
				if (attr == NULL) {
					printf("Attribute %s not found\n", s2);
					return;
				}
				if(e->id2 == id2){
					if(temp->next!=NULL){
						e=temp->next;f=1;
						break; }
					else{ 	
					break;}
				}
				e = e -> next;
			}
				if(f==1){f=0;
					continue;}
				else
					break;
		   }

		}
	dijkstra(g, id1, id2);
	print_path(g, id2);


}

int
tuple_print_attr(tuple_t t, enum_list_t el, char* atr)
{
	attribute_t attr;
	int i, offset;
	assert (t != NULL);
	assert (t->buf != NULL);

	int reval=0;
	for (attr = t->s->attrlist; attr != NULL; attr = attr->next) {
		if(strcmp(attr->name,atr)==0){
		offset = tuple_get_offset(t, attr->name);
		if (offset >= 0) {
			switch (attr->bt) {
			case INTEGER:
				i = tuple_get_int(t->buf + offset);
				reval=i;
				break;
			case CHARACTER:
			case VARCHAR:
			case BOOLEAN:
			case ENUM:
			case FLOAT:
			case DOUBLE:
			case DATE:
			case TIME:
			case BASE_TYPES_MAX:
				printf("\nWeight type not supported to calculate shortest path\n");
			}
		}
		}
	
	}
	return reval;
}
void add_vertex (graph_dij *g, int i) {
    if (g->vertices_size < i + 1) {
        int size = g->vertices_size * 2 > i ? g->vertices_size * 2 : i + 4;
        g->vertices = realloc(g->vertices, size * sizeof (vertex_dij *));
        for (int j = g->vertices_size; j < size; j++)
            g->vertices[j] = NULL;
        g->vertices_size = size;
    }
    if (!g->vertices[i]) {
        g->vertices[i] = calloc(1, sizeof (vertex_dij));
        g->vertices_len++;
    }
}
 
void add_edge_dij(graph_dij *g, int a, int b, int w) {
    //a = a - 'a';
    //b = b - 'a';
    add_vertex(g, a);
    add_vertex(g, b);
    vertex_dij *v = g->vertices[a];
    if (v->edges_len >= v->edges_size) {
        v->edges_size = v->edges_size ? v->edges_size * 2 : 4;
        v->edges = realloc(v->edges, v->edges_size * sizeof (edge_dij *));
    }
	
    edge_dij *e = calloc(1, sizeof (edge_dij));
    e->vertex = b;
    e->weight = w;
    v->edges[v->edges_len++] = e;

}
 
heap_t *create_heap (int n) {
    heap_t *h = calloc(1, sizeof (heap_t));
    h->data = calloc(n + 1, sizeof (int));
    h->prio = calloc(n + 1, sizeof (int));
    h->index = calloc(n, sizeof (int));
    return h;
}
 
void push_heap (heap_t *h, int v, int p) {
    int i = h->index[v] == 0 ? ++h->len : h->index[v];
    int j = i / 2;
    while (i > 1) {
        if (h->prio[j] < p)
            break;
        h->data[i] = h->data[j];
        h->prio[i] = h->prio[j];
        h->index[h->data[i]] = i;
        i = j;
        j = j / 2;
    }
    h->data[i] = v;
    h->prio[i] = p;
    h->index[v] = i;
}
 
int min (heap_t *h, int i, int j, int k) {
    int m = i;
    if (j <= h->len && h->prio[j] < h->prio[m])
        m = j;
    if (k <= h->len && h->prio[k] < h->prio[m])
        m = k;
    return m;
}
 
int pop_heap (heap_t *h) {
    int v = h->data[1];
    int i = 1;
    while (1) {
        int j = min(h, h->len, 2 * i, 2 * i + 1);
        if (j == h->len)
            break;
        h->data[i] = h->data[j];
        h->prio[i] = h->prio[j];
        h->index[h->data[i]] = i;
        i = j;
    }
    h->data[i] = h->data[h->len];
    h->prio[i] = h->prio[h->len];
    h->index[h->data[i]] = i;
    h->len--;
    return v;
}
 
void dijkstra (graph_dij *g, int a, int b) {
    int i, j;
    //a = a - 'a';
    //b = b - 'a';

    for (i = 1; i <= g->vertices_len; i++) {
        vertex_dij *v = g->vertices[i];
        v->dist = INT_MAX;
        v->prev = 0;
        v->visited = 0;
    }
    vertex_dij *v = g->vertices[a];
    v->dist = 0;
    heap_t *h = create_heap(g->vertices_len);
    push_heap(h, a, v->dist);
    while (h->len) {
	i = pop_heap(h);
        if (i == b)
            break;
        v = g->vertices[i];
        v->visited = 1;
        for (j = 0; j < v->edges_len; j++) {
            edge_dij *e = v->edges[j];
            vertex_dij *u = g->vertices[e->vertex];
            if (!u->visited && v->dist + e->weight <= u->dist) {
                u->prev = i;
                u->dist = v->dist + e->weight;
                push_heap(h, e->vertex, u->dist);
            }
        }
    }
}
 
void print_path (graph_dij *g, int i) {
    int n, j,k;
    vertex_dij *v, *u;
    //i = i - 'a';
    v = g->vertices[i];
    if (v->dist == INT_MAX) {
        printf("no path\n");
        return;
    }
    for (n = 1, u = v; u->dist; u = g->vertices[u->prev], n++);
    int *path = malloc(n+1);
    path[n ] = i; //'a' + i;
    for (j = 1, u = v; u->dist; u = g->vertices[u->prev], j++){
        path[n - j] = u->prev; //'a' + u->prev;
	}
    printf("\nShortest distance = %d \nNumber of nodes = %d  \n", v->dist, n);
int len=sizeof(path)/sizeof(path[0]);
printf("\nPath\n");
for(k=1;k<=n;k++)
printf("%d ",path[k]);
printf("\n");
}

