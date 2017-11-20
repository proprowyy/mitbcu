#ifndef BCU_COMM_LINK_H
#define BCU_COMM_LINK_H
			#include<stdlib.h>
			#include<string.h>
			#include <assert.h>

				#define DEBUG diag_printf

				typedef struct
				{
					unsigned char License_plate;
					unsigned char iph_id;
					unsigned char state;
				}node_data;

				typedef struct list_node
				{

				  node_data property;
				  struct list_node *next;

				}list_node_t;

				#define INIT_LIST_HEAD (NULL)
				#define LIST_HEAD(name)  list_node_t* name=INIT_LIST_HEAD
				#define NEW_NODE(name)   list_node_t*  name = (list_node_t*)malloc(sizeof( list_node_t) );
				extern list_node_t * create_empty_list(void);
				extern int insert_new_node(list_node_t *head,list_node_t *node);
				extern int delete_list_node(list_node_t *head,list_node_t *node);
				extern list_node_t * search_list_node(list_node_t *head,unsigned char  License_plate,unsigned char iph_id);
				extern int update_list_node_state(list_node_t *head,unsigned state,unsigned char  License_plate,unsigned char iph_id);
				extern int empty_list_node(list_node_t *head);
				extern int new_node_fill(list_node_t *node,unsigned char  License_plate,unsigned char iph_id);





#endif
