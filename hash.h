#ifndef HASH_H
#define HASH_H

#define SET_ABILITY 256
#define POT_ABILITY 16
#define MAX_NAME    16

typedef struct node 
{
    char* m_name;
    void* m_value;
    int n_value;
    node* m_next;
} node;

typedef struct pot 
{
    char* m_name;
    int n_node;
    node* m_front; 
    node* m_back;
} pot;

typedef struct hash 
{
    int n_node;
    int n_level;
    pot* m_p_set;  
} hash;

hash init_hash();
void free_hash(hash *H);

pot* find(hash* Hash, char* name);
void insert(hash* Hash, char* name, void* value, int n_value);
void remove(hash* Hash, char* name);

#endif
