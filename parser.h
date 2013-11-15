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

#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

#include "lexer.h"

#define MAXSTACK 1024
#define STR_LEN 1024

/* actions */
typedef enum {
    S,  /* shift */
    R,  /* reduce */
    A,  /* accept */
    E1, /* error: missing right parenthesis */
    E2, /* error: missing operator */
    E3  /* error: unbalanced parenthesis */    
} actionsEnum;

typedef struct tagParserData
{
	char    m_strExpr[STR_LEN + 1];
	Token   m_Token;
	int     m_top;              	
	int     m_topOpr;
	int8_t  m_stackOpr[MAXSTACK];	
} ParserData;

int shift(ParserData *pd);
int reduce(ParserData *pd, char *szRet);

int InfixToPostfix(const char *strExpr, char *szRet);

int calculate(const char *strPostfixExpr, double *dblRet);

#endif /* PARSER_H */
