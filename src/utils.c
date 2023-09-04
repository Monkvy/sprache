#include "utils.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char*
read_file(const char *path)
{
	FILE* file = fopen(path, "rb");
	if (!file) {
		perror(NULL);
		exit(-1);
	}
	
	fseek(file, 0, SEEK_END);
    int64_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	char* source = malloc(size + 1);
	fread(source, sizeof(char), size, file);
	source[size] = '\0';

	fclose(file);
	return source;
}

bool
is_digit(char c)
{
	return c >= '0' && c <= '9';
}

bool
is_ident_start(char c)
{
	return ((c >= 'A' && c <= 'Z') || 
			(c >= 'a' && c <= 'z') ||
			c == '_');
}

bool
is_ident_char(char c)
{
	return is_ident_start(c) || is_digit(c);
}

void
print_node(struct Node node, uint8_t identation)
{
	const char *text;
	switch (node.kind)
	{
		case NODE_ROOT:
			text = "ROOT";
			break;
		case NODE_FUNCTION:
			text = "fn";
			break;
		case NODE_RETURN:
			text = "return";
			break;
		case NODE_LITERAL:
			text = "LITERAL";
			break;
		default:
			text = "UNKNOWN";
	}

    char ident[identation * 2 + 1];
    for (uint8_t i = 0; i < identation * 2; ++i)
        ident[i] = ' ';
    ident[identation * 2] = '\0';


    printf("%s%s\n", ident, text);

	if (node.kind == NODE_ROOT)
		print_node_list(node.n_root.body, identation + 1);
	else if (node.kind == NODE_FUNCTION)
		print_node_list(node.n_function.body, identation + 1);
}

void
print_node_list(struct NodeListElement *root, uint8_t identation)
{
	struct NodeListElement *current = root;
	while (current != NULL)
	{
		if (current->self == NULL)
			printf("NULL\n");
		else
			print_node(*current->self, identation);
		current = current->next;
	}
}

void
append_node_list(struct NodeListElement *root, struct Node value)
{
	struct Node *node = malloc(sizeof(struct Node));
	if (node == NULL)
		fail(ERROR_MEMORY_ALLOCATION, "malloc failed");
	memcpy(node, &value, sizeof(struct Node));
	
	struct NodeListElement *new_element = malloc(sizeof(struct NodeListElement));
	if (new_element == NULL)
		fail(ERROR_MEMORY_ALLOCATION, "malloc failed");

	new_element->self = node;
	new_element->next = NULL;

	struct NodeListElement *last = root;

	while (last->next != NULL)
		last = last->next;

	last->next = new_element;
}

void
insert_node_list(struct NodeListElement *root, struct Node value, uint32_t nth)
{
    struct NodeListElement *current = root;

    uint32_t n = 0;
    while (current != NULL)
    {
        if (n == nth)
        {
            if (current->self == NULL)
            {
                current->self = malloc(sizeof(struct Node));
                if (current->self == NULL)
                    fail(ERROR_MEMORY_ALLOCATION, "malloc failed");
            }
            memcpy(current->self, &value, sizeof(struct Node));
        }
        ++n;
        current = current->next;
    }
}

void
free_node_list(struct NodeListElement *root)
{
	struct NodeListElement *current = root;
	while (current != NULL)
	{
		if (current->self != NULL)
			free(current->self);

		struct NodeListElement *next = current->next;
		free(current);
		current = next;
	}
}
