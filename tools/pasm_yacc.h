
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INTEGER = 258,
     FUNCT = 259,
     VARIABLE = 260,
     WHILE = 261,
     IF = 262,
     CALL = 263,
     SUBROUTINE = 264,
     RETURN = 265,
     DEFINE = 266,
     STRING = 267,
     EXIT = 268,
     PASM_CR = 269,
     PASM_POP = 270,
     IFX = 271,
     ELSE = 272,
     NE = 273,
     EQ = 274,
     LE = 275,
     GE = 276,
     UMINUS = 277
   };
#endif
/* Tokens.  */
#define INTEGER 258
#define FUNCT 259
#define VARIABLE 260
#define WHILE 261
#define IF 262
#define CALL 263
#define SUBROUTINE 264
#define RETURN 265
#define DEFINE 266
#define STRING 267
#define EXIT 268
#define PASM_CR 269
#define PASM_POP 270
#define IFX 271
#define ELSE 272
#define NE 273
#define EQ 274
#define LE 275
#define GE 276
#define UMINUS 277




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 55 "pasm_yacc.y"

    int iValue;                 /* integer value */
    char sIndex;                /* symbol table index */
    nodeType *nPtr;             /* node pointer */



/* Line 1676 of yacc.c  */
#line 104 "pasm_yacc.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


