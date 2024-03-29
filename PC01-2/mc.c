/*
 * PC01-2.c
 *
 *  Created on: Jan 6, 2015
 *      Author: Matthew Stephenson
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	double op1, op2;

	if (argc != 4) {
		printf("Usage: ./mc (operator) (operand 1) (operand 2)\n");
		exit(1);
	}

	op1 = atof(argv[2]);
	op2 = atof(argv[3]);

	switch (*argv[1]) {
	case '+':
		printf("%f\n", op1 + op2);
		break;
	case '-':
		printf("%f\n", op1 - op2);
		break;
	case '*':
	case 'x':
		printf("%f\n", op1 * op2);
		break;
	case '/':
		printf("%f\n", op1 / op2);
		break;
	case '%':
		printf("%i\n", (int)op1 % (int)op2);
		break;
	default:
		printf("Invalid operator\n");
	}

	return 0;
}
