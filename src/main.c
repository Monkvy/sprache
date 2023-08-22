#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "error.h"
#include "utils.h"
#include "parser.h"


void
print_help()
{
	printf(
		"sprache <command> [<args>]\n\n"
		"command            summary\n"
		"-------            -------\n"
		"help				Shows this message\n"
		"file <path>        Compiles a file\n"
		"buffer <source>    Compiles given source code\n"
	);
}

int32_t
main(int argc, char **argv)
{
	if (argc == 1)
	{
		fprintf(stderr, "command expected");
		return -1;
	}
	if (!strcmp(argv[1], "help"))
	{
		print_help();
		return 0;
	}
	else if (!strcmp(argv[1], "file"))
	{
		const char *source = read_file(argv[2]);

		struct Node root;
		struct Error err = parse(source, &root);
		if (err.code != ERROR_NONE)
		{
			free((void*)source);
			return fail(err);
		}

		/* print_ast(root); */
		/* free_ast(root); */
		free((void*)source);
		return 0;
	}
	else if (!strcmp(argv[1], "buffer"))
	{
		/* const char* source = argv[2]; */
		/* TODO: pass source into lexer (or tokenizer) */
		return 0;
	}
	fprintf(stderr, "invalid command");
	return -1;
}
