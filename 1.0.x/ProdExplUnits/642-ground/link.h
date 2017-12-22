/*
 * link.h
 *
 *  Created on: 19 Dec, 2016
 *      Author: benty
 */

#ifndef LINK_H_
#define LINK_H_

typedef struct linkstruct
{

		unsigned int vehicle_number ;

		unsigned int devices_id;

		char devices_name[10];

		unsigned char current_state;

		struct linkstruct *next;

}Node;


extern Node *PCURequsthead;
extern Node *BCURequsthead;

extern Node	*creat_linked_list();/*creat a link*/

extern Node *insert_list(Node *head,Node *node);

extern Node *query_list(Node* head, int p_vehicle_number,int p_device_id, Node **precursor );

extern Node *create_node();


extern int dispalys(Node *head);

extern int dispalys_no_recv(Node*head);

extern Node* deletes_list( Node *head, int p_device_no, int p_vehicle_number);

Node* update_list(Node  *head,int p_vehicle_number,int p_device_id,char p_current_state);
#endif /* LINK_H_ */
