/*
 * main.c
 *
 *  Created on: 19 Dec, 2016
 *      Author: benty
 */

#include"stdio.h"
#include"link.h"
#include<stdlib.h>
#include<string.h>
#include <cyg/infra/diag.h>


Node * creat_linked_list(){

	diag_printf("The %s !\n",__FUNCTION__);

	Node    *head = NULL;

	head=( Node *) malloc( sizeof( Node ) );

	head->next = NULL;

	return head;

}

Node * query_list(Node* head, int p_vehicle_number,int p_device_id, Node **precursor )
{

	diag_printf("The %s !\n",__FUNCTION__);

	Node  *temp;

	temp = head->next;

	*precursor = head;

	while(temp != NULL){

		if((temp->devices_id == p_device_id) && (temp->vehicle_number == p_vehicle_number))
		{

			  diag_printf("Successful lookup ! \n");

			  return temp;

		}
		else
		{
			 *precursor=temp;

			 temp=temp->next;
		}
	}

	return NULL;

}

Node* update_list(Node  *head,int p_vehicle_number,int p_device_id,char p_current_state){

	diag_printf("The %s !\n",__FUNCTION__);

	Node    *temp;

	Node    *temp1;

	temp=query_list(head, p_vehicle_number, p_device_id, &temp1);

	if(temp == NULL){

		diag_printf("Error lookup,Cannot update !\n");

		return NULL;
	}
	else
	{
		diag_printf("Update successful!");

		temp->current_state = p_current_state;
	}
	return head;
}



Node * insert_list(Node *head,Node *node)
{
	diag_printf("The %s !\n",__FUNCTION__);

	Node *temp1;

	temp1 = head;
	
	if(temp1->next == NULL)
	{
		diag_printf("This a empty list!\n");

		temp1->next = node;

	}

	else{

		if(query_list(head, node->vehicle_number, node->devices_id, &temp1) == NULL){


			while(temp1->next != NULL){

					temp1= temp1->next;

					}
					temp1->next =node;

			}
		else{

			free( node );

		}


	}

	return head;
}


Node *create_node()
{
	Node *node=(Node* )malloc( sizeof(Node) );

	memset( node, 0, sizeof(Node) );

	return  node;
}


int dispalys(Node*head)
{
	diag_printf("The %s !\n",__FUNCTION__);

	Node *temp = NULL;

	temp = head->next;

	int i = 0;

	if(head->next == NULL)
	{

		diag_printf("This is empty !\n");

	}
	while( temp ){


		diag_printf("Dispalys=%d,%d,%s\n",temp->vehicle_number,temp->devices_id,temp->devices_name);

		temp = temp->next;

		i = i + 1;
}

	return i;

}


int dispalys_no_recv(Node*head)
{
	diag_printf("The %s !\n",__FUNCTION__);

	Node *temp = NULL;

	temp = head->next;

	int i = 0;

	if(head->next == NULL)
	{

		diag_printf("This is empty !\n");

	}
	while( temp ){


		diag_printf("Dispalys=%d,%d,%s\n",temp->vehicle_number,temp->devices_id,temp->devices_name);
		if(temp->current_state==0)
		{
			i = i + 1;
		}
		temp = temp->next;


}

	return i;

}

 Node* deletes_list( Node *head, int p_device_no, int p_vehicle_number){

	 diag_printf("The %s !\n",__FUNCTION__);

	 Node *temp=NULL;

	 Node *temp1=NULL;

	 Node *temp2=NULL;

	 if(head->next  ==  NULL)
	 {

		 diag_printf("This is empty !\n");

         return head;

	 }

	 temp = query_list(head, p_vehicle_number,p_device_no, &temp1);


	 if(temp==NULL)
	 {

		diag_printf("Find faile !\n");

		return head;

	 }

	 else{

		temp1->next = temp->next;

	     free( temp);

	     diag_printf("Delete !\n");

	     return head;

	 }


}
