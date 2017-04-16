#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"
#include "schema.h"	 
#include <assert.h>
#include "enum.h"
#include "tuple.h"

void tuple_print_attr(tuple_t t, enum_list_t el, char * atr);

void
cli_graph_shortpath(char *cmdline, int *pos)
{
	printf("first shortest path algo"); //S1 is source, S2 is destination, S3 is attribute of schema on which SSSP is to be calculated 
	char s1[BUFSIZE], s2[BUFSIZE], s3[BUFSIZE], s4[BUFSIZE];
	schema_type_t st;
	int i, n;
	printf("%c %s %d",pos,pos,pos);
	memset(s1, 0, BUFSIZE);
	nextarg(cmdline, pos, " ", s1);
	memset(s2, 0, BUFSIZE);
	nextarg(cmdline, pos, " ", s2);
	memset(s3, 0, BUFSIZE);
	nextarg(cmdline, pos, " ", s3);
	memset(s4, 0, BUFSIZE);
	nextarg(cmdline, pos, " ", s4);
//#if _DEBUG
	printf("s1=[%s] s2=[%s] s3=[%s], s4=[%s]\n", s1, s2, s3, s4);
//#endif
	vertex_t s,d;
	vertexid_t id1,id2;
	attribute_t attribute;
	printf("work1");
	id1 = (vertexid_t) atoi(s1);
	printf("work2");
	id2 = (vertexid_t) atoi(s2);
	s = graph_find_vertex_by_id(current, id1);
	d = graph_find_vertex_by_id(current, id2);
	if(s == NULL || d == NULL){
		printf("Illegal vertex ID\n");
		return;
	}
	edge_t e;
//	assert (current != NULL);
	for (e = current->e; e != NULL; e = e->next)
		if (e->id1 == id1){
			while(e != NULL)
			{
			//edge_print(e);
			attribute_t attr;
			int i, offset, val;
			float fval;
			double dval;
			tuple_t t;
			t=e->tuple;
			tuple_print_attr(t, current->el,s3);
			attr = schema_find_attr_by_name(t->s, s3);
			if (attr == NULL) {
				printf("Attribute %s not found\n", s2);
				return;
			}
			if(e->id2 == id2)
				break;
			e = e -> next;
			}
				break;
			}

}

void
tuple_print_attr(tuple_t t, enum_list_t el, char* atr)
{
	attribute_t attr;
	int i, offset, val;
	float fval;
	double dval;

	assert (t != NULL);
	assert (t->buf != NULL);

	printf("[");

	for (attr = t->s->attrlist; attr != NULL; attr = attr->next) {
		if(strcmp(attr->name,atr)==0){
		offset = tuple_get_offset(t, attr->name);
		if (offset >= 0) {
			switch (attr->bt) {
			case INTEGER:
				printf("%s",attr->name);
				i = tuple_get_int(t->buf + offset);
				printf("%d", i);
				break;
			}
		}
		}
		if (attr->next != NULL)
			printf(",");

	}
	printf("]");
}
/*
			for (attr = t->s->attrlist; attr != NULL; attr = attr->next) {
				offset = tuple_get_offset(t, attr->name);
				if (offset >= 0) {
					//schema_attribute_print(attr);
					i = tuple_get_int(t->buf + offset);
					//printf("%d", i);									
				}
			}




*/
