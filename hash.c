#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int get_key(char* name)
{
	unsigned long sum = 5381;
	int c;
	while ((c = *name++))sum = ((sum << 5) + sum) + (unsigned char)c;
	return sum % SET_ABILITY;
}

static node* init_node(char* name, void* value, int len_value)
{
	node* p = (node*)calloc(1, sizeof(node));
	if (p == NULL) 
	{
		printf("init fail.\n");
		return NULL;
	}
	
	if (name)
	{
		int len = strlen(name);
		if (len > 0) 
		{
			len = len <= MAX_NAME ? len : MAX_NAME;
			p->m_name = malloc(len + 1);
			if (p->m_name) 
			{
				char* temp_name = (char*)p->m_name;
				for (int i = 0; i < len; i++) temp_name[i] = name[i];
				temp_name[len] = '\0';
			}
		}
	}
	
	if (len_value > 0 && value)
	{
		p->m_value = malloc(len_value + 1);
		if (p->m_value && len_value > 0)
		{
			unsigned char* temp_value = (unsigned char*)p->m_value;
			unsigned char* V = (unsigned char*)value;
			for (int i = 0; i < len_value; i++)temp_value[i] = V[i];
		}
	}

	p->n_value = len_value > 0 ? len_value : 0;

	return p;
}

static void free_node(node *p) 
{
	if (p) 
	{
		free(p->m_name);
		free(p->m_value);
		free(p);
	}
}

static void free_pot(pot *P) 
{
	free(P->m_name);
	P->m_name = NULL;
	node* curr = P->m_front;
	while (curr) 
	{
		node* temp = curr;
		curr = curr->m_next;
		free_node(temp);
	}
	P->n_node = 0;
	P->m_front = NULL;
	P->m_back = NULL;
}

hash init_hash()
{
	hash arr;
	arr.n_node = 0;
	arr.n_level = 1;
	arr.m_p_set = (pot*)calloc(SET_ABILITY, sizeof(pot));
	return arr;
}

void free_hash(hash *H)
{
	if (H == NULL || H->m_p_set == NULL)return;

	int n = SET_ABILITY * H->n_level;
	for (int i = 0; i < n; i++)free_pot(H->m_p_set + i);
	free(H->m_p_set);
	H->m_p_set = NULL;
	H->n_node = 0;
}

pot* find(hash* Hash, char* name)
{
	int key = get_key(name);
	for (int i = 0; i < Hash->n_level; i++) 
	{
		pot* p = Hash->m_p_set + i * SET_ABILITY + key;
		if (p->m_name && strcmp(p->m_name, name) == 0) return p;
	}
	return NULL;
}

void insert(hash *Hash, char* name, void* value, int n_value)
{
	if (name == NULL || value == NULL || n_value < 1) 
	{
		printf("Invalid parameters.\n");
		return;
	}

	node *curr = init_node(name, value, n_value);
	int key = get_key(name), lac = 1;

	for (int i = 0; i < Hash->n_level; i++) 
	{
		int loc = i * SET_ABILITY + key;
		pot* p = Hash->m_p_set + loc;

		if (p->m_name == NULL) 
		{
			int n = strlen(name);
			p->m_name = malloc(n + 1);
			if (!p->m_name) 
			{
				free(curr);
				return;
			}

			for (int u = 0; u < n; u++)p->m_name[u] = name[u];
			p->m_name[n] = '\0';
			p->m_front = curr;
			p->m_back = curr;
			p->n_node++;
			lac = 0;

			break;
		}

		else if (!strcmp(p->m_name, name)) 
		{
			curr->m_next = p->m_front;
			p->m_front = curr;
			p->n_node++;

			if (p->n_node > POT_ABILITY) 
			{
				node* q = p->m_front;
				while (q->m_next != p->m_back) q = q->m_next;

				p->m_back = q;
				node* temp = q->m_next;
				p->m_back->m_next = NULL;
				free_node(temp);
				p->n_node--;
			}
			
			return;
		}
	}

	if (lac) 
	{
		int ability = (Hash->n_level + 1) * SET_ABILITY;
		pot* temp = realloc(Hash->m_p_set, ability * sizeof(pot));
		if (!temp) 
		{
			printf("realloc fail.\n");
			free_node(curr);
			return;
		}
		Hash->m_p_set = temp;

		for (int i = Hash->n_level * SET_ABILITY; i < ability; i++) 
		{
			pot* p = Hash->m_p_set + i;
			p->m_name = NULL;
			p->n_node = 0;
			p->m_front = NULL;
			p->m_back = NULL;
		}

		int loc = Hash->n_level * SET_ABILITY + key;
		pot* p = Hash->m_p_set + loc;

		int len = strlen(name);
		p->m_name = malloc(len + 1);
		for (int i = 0; i < len; i++)p->m_name[i] = name[i];
		p->m_name[len] = '\0';

		p->m_front = curr;
		p->m_back = curr;
		p->n_node++;

		lac = 0;
		Hash->n_level++;
	}

	if(!lac)Hash->n_node++;
}

void remove(hash* Hash, char* name)
{
	pot* p = find(Hash, name);

	if (!p)return;

	if (p->n_node == 0) 
	{
		free(p->m_name);
		p->m_name = NULL;
		return;
	}

	node* temp = p->m_front;
	p->m_front = p->m_front->m_next;
	temp->m_next = NULL;
	free_node(temp);
	p->n_node--;
	Hash->n_node--;

	if (p->n_node == 0)p->m_back = NULL;
}

