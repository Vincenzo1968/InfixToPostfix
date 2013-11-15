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

#ifndef LEXER_H
#define LEXER_H

#define TOKEN_SIZE 256

typedef enum tagTokenType
{	
	T_PLUS = 0,
	T_MINUS,
	T_MULT,
	T_DIV,	
	T_UMINUS,	
	T_EXP,		
	T_OPAREN,
	T_CPAREN,
	T_EOL,		
	T_NUMBER,	
	T_UPLUS,
	T_UNKNOWN	
	
}TokenTypeEnum;

typedef struct tagToken
{
	TokenTypeEnum Type;
	char str[TOKEN_SIZE + 1];
	int nNextPos;
	TokenTypeEnum PreviousTokenType;
	
}Token;

void initToken(Token *tok);
TokenTypeEnum GetNextToken(const char *str, Token *token);
TokenTypeEnum GetNextTokenPostfix(const char *str, Token *token);

#endif /* LEXER_H */
