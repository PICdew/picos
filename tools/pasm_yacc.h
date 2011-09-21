
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
     BREAK = 262,
     IF = 263,
     CALL = 264,
     SUBROUTINE = 265,
     STRING = 266,
     RETURN = 267,
     DEFINE = 268,
     EXIT = 269,
     PASM_CR = 270,
     PASM_POP = 271,
     ARGV = 272,
     ARGC = 273,
     IFX = 274,
     ELSE = 275,
     NE = 276,
     EQ = 277,
     LE = 278,
     GE = 279,
     BSR = 280,
     BSL = 281,
     UMINUS = 282
   };
#endif
/* Tokens.  */
#define INTEGER 258
#define FUNCT 259
#define VARIABLE 260
#define WHILE 261
#define BREAK 262
#define IF 263
#define CALL 264
#define SUBROUTINE 265
#define STRING 266
#define RETURN 267
#define DEFINE 268
#define EXIT 269
#define PASM_CR 270
#define PASM_POP 271
#define ARGV 272
#define ARGC 273
#define IFX 274
#define ELSE 275
#define NE 276
#define EQ 277
#define LE 278
#define GE 279
#define BSR 280
#define BSL 281
#define UMINUS 282




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 56 "pasm_yacc.y"

    int iValue;                 /* integer value */
    idNodeType variable;          /* symbol table index */
    nodeType *nPtr;             /* node pointer */



/* Line 1676 of yacc.c  */
#line 114 "pasm_yacc.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


