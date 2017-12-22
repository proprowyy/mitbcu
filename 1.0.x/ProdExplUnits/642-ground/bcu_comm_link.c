#include<bcu_comm_link.h>
list_node_t * create_empty_list( void ){

					NEW_NODE(node);
					if(node == NULL)
					{
						DEBUG("MALLOC err \n");
						exit(-1);
					}
					memset((void *)node,0,sizeof(list_node_t));
					node->next = NULL;
					return node;

}


int insert_new_node(list_node_t *head, list_node_t *node){

					int num = 0;
					list_node_t *swap_node_piont;
					assert(head!=NULL);
					assert(node!=NULL);
					for(swap_node_piont = head;swap_node_piont->next!=NULL;swap_node_piont=swap_node_piont->next)
					{
						 num=num+1;
					}
					swap_node_piont->next=node;
					node->next=NULL;
					return num+1;
}

int delete_list_node(list_node_t *head,list_node_t *node){

						int num = 0;
						assert(head != NULL);
						assert(node != NULL);
						if(head->next == NULL)
						{
							DEBUG("list is empty !\n");
							return -1;

						}

						list_node_t *swap_node_piont;
						list_node_t *record_node;

						for(swap_node_piont = head;swap_node_piont->next!=NULL;swap_node_piont=swap_node_piont->next)
						{
							record_node=swap_node_piont->next;

							if(record_node== node)
							{
							   swap_node_piont->next=record_node->next;
							   free(record_node);
							   break;
							}
						}
						for(swap_node_piont = head;swap_node_piont->next!=NULL;swap_node_piont=swap_node_piont->next)
						{
							num++;
						}

						return num;

}

list_node_t * search_list_node(list_node_t *head,unsigned char  License_plate,unsigned char iph_id){

							list_node_t *swap_node_piont;

							assert(head!=NULL);

							if(head->next == NULL)
							{
								DEBUG("list is empty ! return NULL .\n");
								return NULL;

							}
							swap_node_piont = head->next;

							while( swap_node_piont )
							{

								if(swap_node_piont->property.iph_id==iph_id&&swap_node_piont->property.License_plate==License_plate)
								{
								   DEBUG("search succeeful.\n");
								   break;
								}

								swap_node_piont=swap_node_piont->next;

							}
							if(swap_node_piont == NULL)
							{
								DEBUG("search fail . return NULL. \n");

							}
						  return swap_node_piont;
}

int update_list_node_state(list_node_t *head,unsigned state,unsigned char  License_plate,unsigned char iph_id){

								assert(head != NULL);
								list_node_t *swap_node_piont;
								if(head->next == NULL)
								{
									DEBUG("list is empty ! return NULL .\n");
									return NULL;

								}
								swap_node_piont=search_list_node(head,License_plate,iph_id);
								if(swap_node_piont != NULL)
								{

									swap_node_piont->property.state=state;

								}else
								{
									DEBUG("update fail ,There is no this table .\n");
									return -1;

								}
								return 0;

}
int empty_list_node(list_node_t *head){

								 assert(head != NULL);

								 if(head->next == NULL)
								 {
									 DEBUG("list is empty ! return NULL .\n");
									 return 0;

								 }
								 list_node_t *swap_node_piont;
								 list_node_t *record_node;

								 swap_node_piont = head->next;

								 while(swap_node_piont){

								  record_node=swap_node_piont->next;
								  free(swap_node_piont);

								   swap_node_piont=record_node;
								 }
								 head->next =NULL;
								 return 0;


}
int new_node_fill(list_node_t *node,unsigned char  License_plate,unsigned char iph_id){

								   node->property.iph_id = iph_id;
								   node->property.License_plate= License_plate;

}

