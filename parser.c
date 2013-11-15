/*
   Copyright (C) 2013 Vincenzo Lo Cicero

   Author: Vincenzo Lo Cicero.
   e-mail: vincenzo.locicero@live.it

   This file is part of ParseExprLib.

   ParseExprLib is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   ParseExprLib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with ParseExprLib.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#include "parser.h"

/*
expr  : expr1 {('+' | '-') expr1};
expr1 : expr2 {('*' | '/') expr2};
expr2 : ['-'] expr3;
expr3 : expr4 {'^' expr4};
expr4 : T_NUMBER
		| '(' expr ')';
*/

int shift(ParserData *pd)
{
	if ( pd->m_topOpr >= MAXSTACK )
	{
		printf("Error 4: stack operator overflow.\n");
		return 0;
	}
		
	pd->m_stackOpr[++(pd->m_topOpr)] = pd->m_Token.Type;
	
	GetNextToken(pd->m_strExpr, &(pd->m_Token));
	
	return 1;
}

int reduce(ParserData *pd, char *szRet)
{	
	if ( pd->m_topOpr < 1 )
	{
		printf("Error 5: missing operator or parenthesis.\n");
		return 0;
	}		
	
	if ( pd->m_top < 1 && pd->m_stackOpr[pd->m_topOpr] != T_OPAREN && pd->m_stackOpr[pd->m_topOpr] != T_CPAREN )
	{
		if ( pd->m_stackOpr[pd->m_topOpr] != T_UMINUS )
		{
			printf("Error 6: missing operand.\n");
			return 0;
		}
		else
		{
			if ( pd->m_top < 0 )
			{
				printf("Error 7: missing operand.\n");
				return 0;						
			}
		}
	}
							
	switch ( pd->m_stackOpr[pd->m_topOpr] )
	{
		case T_PLUS:			
			(pd->m_top)--;			
			strncat(szRet, "+ ", TOKEN_SIZE);							
			break;
		case T_MINUS:
			(pd->m_top)--;						
			strncat(szRet, "- ", TOKEN_SIZE);
			break;
		case T_MULT:
			(pd->m_top)--;						
			strncat(szRet, "* ", TOKEN_SIZE);
			break;
		case T_DIV:
			(pd->m_top)--;						
			strncat(szRet, "/ ", TOKEN_SIZE);
			break;
		case T_UMINUS:
			strncat(szRet, "u ", TOKEN_SIZE);
			break;
		case T_EXP:
			(pd->m_top)--;						
			strncat(szRet, "^ ", TOKEN_SIZE);
			break;
		case T_OPAREN:
			if ( pd->m_Token.Type == T_CPAREN )
				GetNextToken(pd->m_strExpr, &(pd->m_Token));
			break;			
		case T_CPAREN:
			break;						
		default:
			printf("Error 9: %s %d\n", pd->m_Token.str, pd->m_stackOpr[pd->m_topOpr]);
			return 0;
	}
	
	(pd->m_topOpr)--;
		
	return 1;
}

int InfixToPostfix(const char *strExpr, char *szRet)
{
	int ret = 1;	
	ParserData md;
					
	int8_t parseTable[9][9] =
	{
		/*            -------------- input ------------- */
		/*            +   -   *   /   UM  ^   (   )   $  */
		/*            --  --  --  --  --  --  --  --  -- */
		/* stack */
		/* ----- */		
		/*   +   */ { R,  R,  S,  S,  S,  S,  S,  R,  R },
		/*   -   */ { R,  R,  S,  S,  S,  S,  S,  R,  R },
		/*   *   */ { R,  R,  R,  R,  S,  S,  S,  R,  R },
		/*   /   */ { R,  R,  R,  R,  S,  S,  S,  R,  R },
		/*   UM  */ { R,  R,  R,  R,  S,  S,  S,  R,  R },
		/*   ^   */ { R,  R,  R,  R,  R,  S,  S,  R,  R },
		/*   (   */ { S,  S,  S,  S,  S,  S,  S,  R,  E1},
		/*   )   */ { R,  R,  R,  R,  R,  R,  E2, R,  R },
		/*   $   */ { S,  S,  S,  S,  S,  S,  S,  E3, A }
	};		
	
	szRet[0] = '\0';

	strncpy(md.m_strExpr, strExpr, STR_LEN);
	
	md.m_top = -1;
	md.m_topOpr = 0;
	md.m_stackOpr[0] = T_EOL;

	initToken(&(md.m_Token));
	GetNextToken(md.m_strExpr, &(md.m_Token));
	if ( md.m_Token.Type == T_EOL )
		return 1;

	while ( 1 )
	{		
		switch ( md.m_Token.Type )
		{
			case T_UNKNOWN:
				printf("Error 0: invalid token: %s\n", md.m_Token.str);
				return 0;			
			case T_NUMBER:
				++md.m_top;
				strncat(szRet, md.m_Token.str, TOKEN_SIZE);
				strncat(szRet, " ", TOKEN_SIZE);
				GetNextToken(md.m_strExpr, &(md.m_Token));
				break;
			case T_UPLUS:
				GetNextToken(md.m_strExpr, &(md.m_Token));
				break;
			default:
				switch ( parseTable[md.m_stackOpr[md.m_topOpr]][md.m_Token.Type] )
				{
					case S:
						if ( !shift(&md) )
							return 0;
						break;
					case R:
						if ( !reduce(&md, szRet) )
							return 0;
						break;
					case A:
						if ( md.m_top != 0 )
						{
							printf("Error 10: missing operator.\n");
							return 0;
						}
						if ( md.m_topOpr != 0 )
						{
							printf("Error 11: missing operand.\n");
							return 0;
						}						
						md.m_top--;
						return 1;
					case E1:
						printf("Error 1: missing right parenthesis\n");
						return 0;
					case E2:
						printf("Error 2: missing operator\n");
						return 0;					
					case E3:
						printf("Error 3: unbalanced parenthesis\n");
						return 0;						
				}
				break;
		}		
	}

	return ret;
}

int calculate(const char *strPostfixExpr, double *dblRet)
{
	ParserData md;
	double myStack[MAXSTACK];
	int top = -1;
	double right;
	int i;
	
	strncpy(md.m_strExpr, strPostfixExpr, STR_LEN);
	
	md.m_top = -1;
	md.m_topOpr = 0;
	md.m_stackOpr[0] = T_EOL;
	
	*dblRet = 0.0;
	
	initToken(&(md.m_Token));
	GetNextTokenPostfix(md.m_strExpr, &(md.m_Token));
		
	while ( md.m_Token.Type != T_EOL )
	{
		switch ( md.m_Token.Type )
		{
			case T_UNKNOWN:
				printf("Error: invalid token: %s\n", md.m_Token.str);
				return 0;			
			case T_NUMBER:
				++top;
				if ( top >= MAXSTACK )
				{
					printf("Error: stack operator overflow.\n");
					return 0;
				}
				/*printf("SHIFT %s\n", md.m_Token.str);*/
				myStack[top] = atof(md.m_Token.str);
				break;				
			case T_PLUS:
				if ( top < 1 )
				{
					printf("Error: missing operand.\n");
					return 0;					
				}
				right = myStack[top--];
				/*printf("REDUCE %g + %g = %g\n", myStack[top], right, myStack[top] + right);*/
				myStack[top] += right;
				break;
			case T_MINUS:
				if ( top < 1 )
				{
					printf("Error: missing operand.\n");
					return 0;					
				}		
				right = myStack[top--];
				/*printf("REDUCE %g - %g = %g\n", myStack[top], right, myStack[top] - right);*/
				myStack[top] -= right;
				break;
			case T_MULT:
				if ( top < 1 )
				{
					printf("Error: missing operand.\n");
					return 0;					
				}		
				right = myStack[top--];
				/*printf("REDUCE %g * %g = %g\n", myStack[top], right, myStack[top] * right);*/
				myStack[top] *= right;
				break;
			case T_DIV:
				if ( top < 1 )
				{
					printf("Error: missing operand.\n");
					return 0;					
				}		
				right = myStack[top--];
				if ( right == 0 )
				{
					printf("Error: division by 0.\n");
					return 0;					
				}
				/*printf("REDUCE %g / %g = %g\n", myStack[top], right, myStack[top] / right);*/
				myStack[top] /= right;
				break;
			case T_UMINUS:
				if ( top < 0 )
				{
					printf("Error: missing operand.\n");
					return 0;					
				}	
				/*printf("REDUCE - %g = %g\n", myStack[top], -myStack[top]);*/	
				myStack[top] = -myStack[top];
				break;
			case T_EXP:
				if ( top < 1 )
				{
					printf("Error: missing operand.\n");
					return 0;					
				}		
				right = myStack[top--];
				/*printf("REDUCE %g ^ %g = %g\n", myStack[top], right, pow(myStack[top], right));*/
				myStack[top] = pow(myStack[top], right);
				break;	
			default:
				printf("\nError: %s\n", md.m_Token.str);
				return 0;
		}
		
		GetNextTokenPostfix(md.m_strExpr, &(md.m_Token));
	}
		
	if ( top > 0 )
	{
		printf("Error: missing operand.\n");
		for ( i = top; i >= 0; i-- )
		{
			printf("Stack[%d] = %g\n", i, myStack[i]);
		}
		return 0;	
	}
	
	*dblRet = myStack[top--];
			
	return 1;
}
