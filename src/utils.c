#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"


char*
read_file(const char *path)
{
	FILE* file = fopen(path, "rb");
	if (file == NULL) {
		perror(NULL);
		exit(-1);
	}
	
	fseek(file, 0, SEEK_END);
    int64_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	char* source = malloc(size + 1);
	if (source == NULL)
		fail(ERROR_MEMORY_ALLOCATION, "malloc failed");

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
skip_whitespace(struct Loc *loc)
{
    while (
        loc->c == ' ' ||
        loc->c == '\t' ||
        loc->c == '\n' ||
        loc->c == '\r'
    ) {
        step(loc);
    }
}

void
print_node(struct Node node, uint8_t identation)
{
	const char *text;
	char *value = malloc(1);
	if (value == NULL)
		fail(ERROR_MEMORY_ALLOCATION, "malloc failed");
	value[0] = '\0';

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

			/* val_len considers null termination */
			uint32_t val_len = node.n_literal.value.end.i - node.n_literal.value.start.i + 1;
			value = realloc(value, val_len);
			value[val_len] = '\0';

			strncpy(value, node.n_literal.value.end.source + node.n_literal.value.start.i, val_len - 1);
			break;
		default:
			text = "UNKNOWN";
	}

    char ident[identation * 2 + 1];
    for (uint8_t i = 0; i < identation * 2; ++i)
        ident[i] = ' ';
    ident[identation * 2] = '\0';

    printf("%s%s %s\n", ident, text, value);
	free(value);

	if (node.kind == NODE_ROOT)
		print_node_list(node.n_root.body, identation + 1);
	else if (node.kind == NODE_FUNCTION)
		print_node_list(node.n_function.body, identation + 1);
	else if (node.kind == NODE_RETURN)
		print_node(*node.n_return.value, identation + 1);
}

void
free_token(struct Token token)
{
	if (token.value != NULL)
		free((void*)token.value);
}

void
free_node(struct Node node)
{
    switch (node.kind)
    {
        case NODE_ROOT:
            free_node_list(node.n_root.body);
            break;
        case NODE_FUNCTION:
			free_token(node.n_function.name);
            free_node_list(node.n_function.args);
            free_node_list(node.n_function.body);
            break;
        case NODE_RETURN:
            free(node.n_return.value);
            break;
        case NODE_LITERAL:
			free_token(node.n_literal.value);
            break;
        default:
            fail(ERROR_NODE_INVALID, "not imlpemented for free_node");
    }
}

void
print_node_list(struct NodeListElement *root, uint8_t identation)
{
	struct NodeListElement *current = root;
	while (current != NULL)
	{
		if (current->self == NULL)
        {
            char ident[identation * 2 + 1];
            for (uint8_t i = 0; i < identation * 2; ++i)
                ident[i] = ' ';
            ident[identation * 2] = '\0';
            printf("%sNULL\n", ident);
        }
		else
			print_node(*current->self, identation);
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
        {
            free_node(*current->self);
			free(current->self);
        }

		struct NodeListElement *next = current->next;
		free(current);
		current = next;
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


