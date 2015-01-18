#include <stdio.h>

int main( int argc, char *argv[] ) {
	int i = 0;

	printf("\nargc: %i\nPos\tArg\n", argc);

	for(; argv[i]; i++)
		printf("%i\t%s\n", i, argv[i]);

	return 0;
}
