
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "pasm_yacc.y"

#include "pasm.h"
#include "../piclang.h"
#include "utils.h"
#include "../page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <getopt.h>
#include <errno.h>

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

struct compiled_code *the_code;
struct compiled_code *the_code_end;
struct compiled_code *the_strings;
struct compiled_code *the_strings_end;
struct subroutine_map *subroutines;

char **string_list;
size_t num_strings;
int *variable_list;
size_t num_variables;
static int lbl;

/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *id(int i);
nodeType *con(int value);
void freeNode(nodeType *p);
int ex(nodeType *p);
int yylex(void);
 FILE *assembly_file;
void yyerror(char *s);
 int resolve_string(const char *str, int *is_new);
 int resolve_variable(const int id);
 extern char *yytext;
 extern char *last_string;
 nodeType* store_string(const char *);
  
 int sym[26];                    /* symbol table */


/* Line 189 of yacc.c  */
#line 125 "pasm_yacc.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INTEGER = 258,
     VARIABLE = 259,
     WHILE = 260,
     IF = 261,
     PUTCH = 262,
     PUTD = 263,
     EXIT = 264,
     SYSTEM = 265,
     SPRINT = 266,
     STRING = 267,
     CR = 268,
     MORSE = 269,
     TIME = 270,
     ARGD = 271,
     ARGCH = 272,
     SET_TIME = 273,
     SET_DATE = 274,
     GETD = 275,
     GETCH = 276,
     CLEAR = 277,
     FPUTCH = 278,
     FPUTD = 279,
     FFLUSH = 280,
     CALL = 281,
     EXIT_RETURN = 282,
     SUBROUTINE = 283,
     RETURN = 284,
     DEFINE = 285,
     ENDDEF = 286,
     IFX = 287,
     ELSE = 288,
     NE = 289,
     EQ = 290,
     LE = 291,
     GE = 292,
     UMINUS = 293
   };
#endif
/* Tokens.  */
#define INTEGER 258
#define VARIABLE 259
#define WHILE 260
#define IF 261
#define PUTCH 262
#define PUTD 263
#define EXIT 264
#define SYSTEM 265
#define SPRINT 266
#define STRING 267
#define CR 268
#define MORSE 269
#define TIME 270
#define ARGD 271
#define ARGCH 272
#define SET_TIME 273
#define SET_DATE 274
#define GETD 275
#define GETCH 276
#define CLEAR 277
#define FPUTCH 278
#define FPUTD 279
#define FFLUSH 280
#define CALL 281
#define EXIT_RETURN 282
#define SUBROUTINE 283
#define RETURN 284
#define DEFINE 285
#define ENDDEF 286
#define IFX 287
#define ELSE 288
#define NE 289
#define EQ 290
#define LE 291
#define GE 292
#define UMINUS 293




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 52 "pasm_yacc.y"

    int iValue;                 /* integer value */
    char sIndex;                /* symbol table index */
    nodeType *nPtr;             /* node pointer */



/* Line 214 of yacc.c  */
#line 245 "pasm_yacc.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 257 "pasm_yacc.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   488

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  52
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  6
/* YYNRULES -- Number of rules.  */
#define YYNRULES  52
/* YYNRULES -- Number of states.  */
#define YYNSTATES  160

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   293

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      46,    47,    42,    40,    48,    41,     2,    43,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    45,
      35,    49,    34,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    50,     2,    51,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    36,
      37,    38,    39,    44
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     8,     9,    11,    16,    22,    25,
      29,    33,    41,    51,    57,    63,    69,    77,    87,    90,
      96,   102,   108,   114,   119,   124,   130,   136,   144,   148,
     153,   158,   160,   162,   165,   167,   169,   173,   177,   181,
     185,   190,   193,   197,   201,   205,   209,   213,   217,   221,
     225,   229,   233
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      53,     0,    -1,    54,    -1,    54,    55,    -1,    -1,    45,
      -1,    27,    46,    47,    45,    -1,    27,    46,    57,    47,
      45,    -1,    29,    55,    -1,    26,    28,    45,    -1,    30,
      28,    55,    -1,    10,    46,    57,    48,    57,    47,    45,
      -1,    10,    46,    57,    48,    57,    48,    57,    47,    45,
      -1,    11,    46,    12,    47,    45,    -1,    14,    46,    12,
      47,    45,    -1,    14,    46,    57,    47,    45,    -1,    18,
      46,    57,    48,    57,    47,    45,    -1,    19,    46,    57,
      48,    57,    48,    57,    47,    45,    -1,    57,    45,    -1,
       7,    46,    57,    47,    45,    -1,     8,    46,    57,    47,
      45,    -1,    23,    46,    57,    47,    45,    -1,    24,    46,
      57,    47,    45,    -1,    25,    46,    47,    45,    -1,     4,
      49,    57,    45,    -1,     5,    46,    57,    47,    55,    -1,
       6,    46,    57,    47,    55,    -1,     6,    46,    57,    47,
      55,    33,    55,    -1,    50,    56,    51,    -1,    13,    46,
      47,    45,    -1,    22,    46,    47,    45,    -1,     9,    -1,
      55,    -1,    56,    55,    -1,     3,    -1,     4,    -1,    16,
      46,    47,    -1,    17,    46,    47,    -1,    20,    46,    47,
      -1,    21,    46,    47,    -1,    15,    46,    57,    47,    -1,
      41,    57,    -1,    57,    40,    57,    -1,    57,    41,    57,
      -1,    57,    42,    57,    -1,    57,    43,    57,    -1,    57,
      35,    57,    -1,    57,    34,    57,    -1,    57,    39,    57,
      -1,    57,    38,    57,    -1,    57,    36,    57,    -1,    57,
      37,    57,    -1,    46,    57,    47,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    76,    76,    80,    81,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   115,   116,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INTEGER", "VARIABLE", "WHILE", "IF",
  "PUTCH", "PUTD", "EXIT", "SYSTEM", "SPRINT", "STRING", "CR", "MORSE",
  "TIME", "ARGD", "ARGCH", "SET_TIME", "SET_DATE", "GETD", "GETCH",
  "CLEAR", "FPUTCH", "FPUTD", "FFLUSH", "CALL", "EXIT_RETURN",
  "SUBROUTINE", "RETURN", "DEFINE", "ENDDEF", "IFX", "ELSE", "'>'", "'<'",
  "NE", "EQ", "LE", "GE", "'+'", "'-'", "'*'", "'/'", "UMINUS", "';'",
  "'('", "')'", "','", "'='", "'{'", "'}'", "$accept", "program",
  "function", "stmt", "stmt_list", "expr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,    62,    60,   289,   290,   291,   292,
      43,    45,    42,    47,   293,    59,    40,    41,    44,    61,
     123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    52,    53,    54,    54,    55,    55,    55,    55,    55,
      55,    55,    55,    55,    55,    55,    55,    55,    55,    55,
      55,    55,    55,    55,    55,    55,    55,    55,    55,    55,
      55,    55,    56,    56,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
      57,    57,    57
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     0,     1,     4,     5,     2,     3,
       3,     7,     9,     5,     5,     5,     7,     9,     2,     5,
       5,     5,     5,     4,     4,     5,     5,     7,     3,     4,
       4,     1,     1,     2,     1,     1,     3,     3,     3,     3,
       4,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,     2,     1,    34,    35,     0,     0,     0,     0,
      31,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     5,     0,     0,     3,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     8,     0,
      35,    41,     0,    32,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    18,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    36,    37,     0,
       0,    38,    39,     0,     0,     0,     0,     9,     0,     0,
      10,    52,    28,    33,    47,    46,    50,    51,    49,    48,
      42,    43,    44,    45,    24,     0,     0,     0,     0,     0,
       0,    29,     0,     0,    40,     0,     0,    30,     0,     0,
      23,     6,     0,    25,    26,    19,    20,     0,    13,    14,
      15,     0,     0,    21,    22,     7,     0,     0,     0,     0,
       0,    27,    11,     0,    16,     0,     0,     0,    12,    17
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     2,    34,    64,    35
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -45
static const yytype_int16 yypact[] =
{
     -45,     1,   122,   -45,   -45,   -44,   -34,   -33,   -29,   -26,
     -45,   -24,   -21,   -20,   -18,   -13,    -1,     0,     2,     3,
       4,     5,     7,     8,     9,    10,     6,    11,   122,    30,
     176,   -45,   176,   122,   -45,   431,   176,   176,   176,   176,
     176,   176,    72,    21,   161,   176,    40,    41,   176,   176,
      43,    44,    45,   176,   176,    46,    49,   154,   -45,   122,
     -45,   -45,   249,   -45,    56,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   -45,   443,   263,   277,   291,
     305,   189,    51,    54,    53,   319,   333,   -45,   -45,   204,
     219,   -45,   -45,    60,   347,   361,    78,   -45,    79,   375,
     -45,   -45,   -45,   -45,   -11,   -11,   -11,   -11,   -11,   -11,
     -39,   -39,   -45,   -45,   -45,   122,   122,   105,   108,   176,
     109,   -45,   110,   111,   -45,   176,   176,   -45,   114,   115,
     -45,   -45,   116,   -45,    86,   -45,   -45,    74,   -45,   -45,
     -45,   389,   234,   -45,   -45,   -45,   122,   117,   176,   121,
     176,   -45,   -45,   403,   -45,   417,   138,   139,   -45,   -45
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -45,   -45,   -45,   -12,   -45,   -30
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      61,     3,    62,    73,    74,    36,    76,    77,    78,    79,
      80,    81,    37,    38,    85,    86,    58,    39,    89,    90,
      40,    63,    41,    94,    95,    42,    43,    99,    44,    71,
      72,    73,    74,    45,    56,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,    46,    47,   100,    48,    49,
      50,    51,   103,    52,    53,    54,    55,    57,    59,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    83,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    82,    28,    29,    87,    88,   137,
      91,    92,    93,    96,    97,   141,   142,    30,   120,   121,
     122,    31,    32,   133,   134,   127,    33,   102,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,   153,   146,
     155,   147,   148,   130,   131,     4,     5,     6,     7,     8,
       9,    10,    11,    12,   151,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
     135,    28,    29,   136,   138,   139,   140,     4,    60,   143,
     144,   145,   152,    30,     4,    60,   154,    31,    32,    15,
      16,    17,    33,    84,    20,    21,    15,    16,    17,     4,
      60,    20,    21,   158,   159,     0,     0,     0,     0,     0,
       0,    15,    16,    17,     0,    30,    20,    21,     0,     0,
      32,    98,    30,     0,     0,     0,     0,    32,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    30,     0,     0,
       0,     0,    32,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,     0,     0,     0,     0,   119,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,     0,     0,
       0,     0,   125,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,     0,     0,     0,     0,   126,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,     0,     0,
       0,     0,   150,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,     0,     0,     0,   101,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,     0,     0,     0,
     115,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,     0,     0,     0,   116,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,     0,     0,     0,   117,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,     0,
       0,     0,   118,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,     0,     0,     0,   123,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,     0,     0,     0,
     124,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,     0,     0,     0,   128,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,     0,     0,     0,   129,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,     0,
       0,     0,   132,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,     0,     0,     0,   149,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,     0,     0,     0,
     156,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,     0,     0,     0,   157,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,     0,    75,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,     0,   114
};

static const yytype_int16 yycheck[] =
{
      30,     0,    32,    42,    43,    49,    36,    37,    38,    39,
      40,    41,    46,    46,    44,    45,    28,    46,    48,    49,
      46,    33,    46,    53,    54,    46,    46,    57,    46,    40,
      41,    42,    43,    46,    28,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    46,    46,    59,    46,    46,
      46,    46,    64,    46,    46,    46,    46,    46,    28,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    47,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    12,    29,    30,    47,    47,   119,
      47,    47,    47,    47,    45,   125,   126,    41,    47,    45,
      47,    45,    46,   115,   116,    45,    50,    51,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,   148,    33,
     150,    47,    48,    45,    45,     3,     4,     5,     6,     7,
       8,     9,    10,    11,   146,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      45,    29,    30,    45,    45,    45,    45,     3,     4,    45,
      45,    45,    45,    41,     3,     4,    45,    45,    46,    15,
      16,    17,    50,    12,    20,    21,    15,    16,    17,     3,
       4,    20,    21,    45,    45,    -1,    -1,    -1,    -1,    -1,
      -1,    15,    16,    17,    -1,    41,    20,    21,    -1,    -1,
      46,    47,    41,    -1,    -1,    -1,    -1,    46,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,    -1,    -1,
      -1,    -1,    46,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    -1,    -1,    -1,    -1,    48,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    -1,    -1,
      -1,    -1,    48,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    -1,    -1,    -1,    -1,    48,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    -1,    -1,
      -1,    -1,    48,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    -1,    -1,    -1,    47,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    -1,    -1,    -1,
      47,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    -1,    -1,    -1,    47,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    -1,    -1,    -1,    47,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    -1,
      -1,    -1,    47,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    -1,    -1,    -1,    47,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    -1,    -1,    -1,
      47,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    -1,    -1,    -1,    47,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    -1,    -1,    -1,    47,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    -1,
      -1,    -1,    47,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    -1,    -1,    -1,    47,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    -1,    -1,    -1,
      47,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    -1,    -1,    -1,    47,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    -1,    45,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    -1,    45
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    53,    54,     0,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    29,    30,
      41,    45,    46,    50,    55,    57,    49,    46,    46,    46,
      46,    46,    46,    46,    46,    46,    46,    46,    46,    46,
      46,    46,    46,    46,    46,    46,    28,    46,    55,    28,
       4,    57,    57,    55,    56,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    45,    57,    57,    57,    57,
      57,    57,    12,    47,    12,    57,    57,    47,    47,    57,
      57,    47,    47,    47,    57,    57,    47,    45,    47,    57,
      55,    47,    51,    55,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    45,    47,    47,    47,    47,    48,
      47,    45,    47,    47,    47,    48,    48,    45,    47,    47,
      45,    45,    47,    55,    55,    45,    45,    57,    45,    45,
      45,    57,    57,    45,    45,    45,    33,    47,    48,    47,
      48,    55,    45,    57,    45,    57,    47,    47,    45,    45
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 76 "pasm_yacc.y"
    { YYACCEPT; }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 80 "pasm_yacc.y"
    { ex((yyvsp[(2) - (2)].nPtr)); freeNode((yyvsp[(2) - (2)].nPtr)); }
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 85 "pasm_yacc.y"
    { (yyval.nPtr) = opr(';', 2, NULL, NULL); }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 86 "pasm_yacc.y"
    { (yyval.nPtr) = opr(EXIT_RETURN,0); }
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 87 "pasm_yacc.y"
    { (yyval.nPtr) = opr(EXIT_RETURN, 1, (yyvsp[(3) - (5)].nPtr)); }
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 88 "pasm_yacc.y"
    { (yyval.nPtr) = opr(RETURN,1,(yyvsp[(2) - (2)].nPtr));}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 89 "pasm_yacc.y"
    { (yyval.nPtr) = opr(CALL,1,(yyvsp[(2) - (3)].nPtr)); }
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 90 "pasm_yacc.y"
    {  (yyval.nPtr) = opr(DEFINE,2,(yyvsp[(2) - (3)].nPtr),(yyvsp[(3) - (3)].nPtr));}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 91 "pasm_yacc.y"
    { (yyval.nPtr) = opr(SYSTEM,2,(yyvsp[(3) - (7)].nPtr),(yyvsp[(5) - (7)].nPtr));}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 92 "pasm_yacc.y"
    {(yyval.nPtr) = opr(SYSTEM,3,(yyvsp[(3) - (9)].nPtr),(yyvsp[(5) - (9)].nPtr),(yyvsp[(7) - (9)].nPtr));}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 93 "pasm_yacc.y"
    {(yyval.nPtr) = opr(SPRINT,1,(yyvsp[(3) - (5)].nPtr));}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 94 "pasm_yacc.y"
    {(yyval.nPtr) = opr(MORSE,2,(yyvsp[(3) - (5)].nPtr),con(PICLANG_MORSE_STRING));}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 95 "pasm_yacc.y"
    {(yyval.nPtr) = opr(MORSE,2,(yyvsp[(3) - (5)].nPtr),con(PICLANG_MORSE_CHAR));}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 96 "pasm_yacc.y"
    { (yyval.nPtr) = opr(SET_TIME,2,(yyvsp[(3) - (7)].nPtr),(yyvsp[(5) - (7)].nPtr));}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 97 "pasm_yacc.y"
    { (yyval.nPtr) = opr(SET_DATE,3,(yyvsp[(3) - (9)].nPtr),(yyvsp[(5) - (9)].nPtr),(yyvsp[(7) - (9)].nPtr));}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 98 "pasm_yacc.y"
    { (yyval.nPtr) = (yyvsp[(1) - (2)].nPtr); }
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 99 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PUTCH, 1, (yyvsp[(3) - (5)].nPtr)); }
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 100 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PUTD,1,(yyvsp[(3) - (5)].nPtr)); }
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 101 "pasm_yacc.y"
    { (yyval.nPtr) = opr(FPUTCH,1,(yyvsp[(3) - (5)].nPtr)); }
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 102 "pasm_yacc.y"
    { (yyval.nPtr) = opr(FPUTD,1,(yyvsp[(3) - (5)].nPtr)); }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 103 "pasm_yacc.y"
    {(yyval.nPtr) = opr(FFLUSH,0);}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 104 "pasm_yacc.y"
    { (yyval.nPtr) = opr('=', 2, id((yyvsp[(1) - (4)].sIndex)), (yyvsp[(3) - (4)].nPtr)); }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 105 "pasm_yacc.y"
    { (yyval.nPtr) = opr(WHILE, 2, (yyvsp[(3) - (5)].nPtr), (yyvsp[(5) - (5)].nPtr)); }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 106 "pasm_yacc.y"
    { (yyval.nPtr) = opr(IF, 2, (yyvsp[(3) - (5)].nPtr), (yyvsp[(5) - (5)].nPtr)); }
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 107 "pasm_yacc.y"
    { (yyval.nPtr) = opr(IF, 3, (yyvsp[(3) - (7)].nPtr), (yyvsp[(5) - (7)].nPtr), (yyvsp[(7) - (7)].nPtr)); }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 108 "pasm_yacc.y"
    { (yyval.nPtr) = (yyvsp[(2) - (3)].nPtr); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 109 "pasm_yacc.y"
    {(yyval.nPtr) = opr(PUTCH,1,con(0xa));}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 110 "pasm_yacc.y"
    { (yyval.nPtr) = opr(CLEAR,0);}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 111 "pasm_yacc.y"
    {YYACCEPT;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 115 "pasm_yacc.y"
    { (yyval.nPtr) = (yyvsp[(1) - (1)].nPtr); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 116 "pasm_yacc.y"
    { (yyval.nPtr) = opr(';', 2, (yyvsp[(1) - (2)].nPtr), (yyvsp[(2) - (2)].nPtr)); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 120 "pasm_yacc.y"
    { (yyval.nPtr) = con((yyvsp[(1) - (1)].iValue)); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 121 "pasm_yacc.y"
    { (yyval.nPtr) = id((yyvsp[(1) - (1)].sIndex)); }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 122 "pasm_yacc.y"
    { (yyval.nPtr) = opr(ARGD,0);}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 123 "pasm_yacc.y"
    { (yyval.nPtr) = opr(ARGCH,0);}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 124 "pasm_yacc.y"
    { (yyval.nPtr) = opr(GETD,0);}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 125 "pasm_yacc.y"
    { (yyval.nPtr) = opr(GETCH,0);}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 126 "pasm_yacc.y"
    { (yyval.nPtr) = opr(TIME,1,(yyvsp[(3) - (4)].nPtr));}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 127 "pasm_yacc.y"
    { (yyval.nPtr) = opr(UMINUS, 1, (yyvsp[(2) - (2)].nPtr)); }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 128 "pasm_yacc.y"
    { (yyval.nPtr) = opr('+', 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 129 "pasm_yacc.y"
    { (yyval.nPtr) = opr('-', 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 130 "pasm_yacc.y"
    { (yyval.nPtr) = opr('*', 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 131 "pasm_yacc.y"
    { (yyval.nPtr) = opr('/', 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 132 "pasm_yacc.y"
    { (yyval.nPtr) = opr('<', 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 133 "pasm_yacc.y"
    { (yyval.nPtr) = opr('>', 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 134 "pasm_yacc.y"
    { (yyval.nPtr) = opr(GE, 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 135 "pasm_yacc.y"
    { (yyval.nPtr) = opr(LE, 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 136 "pasm_yacc.y"
    { (yyval.nPtr) = opr(NE, 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 137 "pasm_yacc.y"
    { (yyval.nPtr) = opr(EQ, 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 138 "pasm_yacc.y"
    { (yyval.nPtr) = (yyvsp[(2) - (3)].nPtr); }
    break;



/* Line 1455 of yacc.c  */
#line 1983 "pasm_yacc.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 141 "pasm_yacc.y"


void insert_subroutine(const char *name, size_t label)
{
  
  if(subroutines == NULL)
    subroutines = (struct subroutine_map*)malloc(sizeof(struct subroutine_map));
  else
    {
      struct subroutine_map* tmp = (struct subroutine_map*)malloc(sizeof(struct subroutine_map));
      tmp->next = subroutines;
      subroutines = tmp;
    }
  strcpy(subroutines->name,name);
  subroutines->label = label;
}

#define SIZEOF_NODETYPE ((char *)&p->con - (char *)p)

nodeType *con(int value) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(conNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeCon;
    p->con.value = value;

    return p;
}

nodeType *id(int i) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeId;
    p->id.i = i;

    return p;
}

nodeType *opr(int oper, int nops, ...) {
    va_list ap;
    nodeType *p;
    size_t nodeSize;
    int i;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(oprNodeType) +
        (nops - 1) * sizeof(nodeType*);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;
    va_start(ap, nops);
    for (i = 0; i < nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);
    return p;
}

void freeNode(nodeType *p) {
    int i;

    if (!p) return;
    if (p->type == typeOpr) {
        for (i = 0; i < p->opr.nops; i++)
            freeNode(p->opr.op[i]);
    }
    free (p);
}

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
    exit(-1);
}





static const char short_options[] = "a:e:ho:";
enum OPTION_INDICES{OUTPUT_HEX};
static struct option long_options[] =
             {
	       {"help",0,NULL,'h'},
               {"hex", 1,NULL, OUTPUT_HEX},
	       {"asm", 1,NULL, 'a'},
	       {"eeprom",1,NULL, 'e'},
	       {"binary",1,NULL,'o'},
               {0, 0, 0, 0}
             };

void print_help()
{
  printf("\n");
  printf("pasm -- Piclang compiler.\n");
  printf("Copyright 2011 David Coss, PhD\n");
  printf("-------------------------------\n");
  printf("Compiles piclang programs for use with the Pic Operating System.\n");
  printf("Note: If no source file is provided, the compiler will act as \n\tan interpreter.\n");
  printf("\n");
  printf("Usage: pasm [options] [source code]\n\n");
  printf("Options:\n");
  printf("--help, -h :\t\t Displays this dialog.\n");
  printf("--asm,-a <file> :\t Outputs the assembly to the specified file.\n");
  printf("--hex <file>    :\t Outputs Intel Hex to the specified file.\n");
  printf("--eeprom, -e <file> :\t Outputs \"__EEPROM_DATA(...)\" code for use\n");
  printf("                     \t with the Hi Tech C Compiler.\n");
  printf("--binary, -o <file> :\t Outputs a binary file containing the compiled program.\n");
}

int main(int argc, char **argv) 
{
  char hex_buffer[45];
  FILE *hex_file = NULL, *eeprom_file = NULL, *binary_file = NULL;
  char opt;
  int opt_index;
  unsigned char piclang_bitmap = 0;
  struct compiled_code *curr_code = NULL;

  assembly_file = NULL;
  the_code_end = the_code = NULL;
  the_strings = the_strings = NULL;
  string_list = NULL;num_strings = 0;
  variable_list = NULL;num_variables = 0;
  subroutines = NULL;

  while(TRUE)
    {    
      opt = getopt_long(argc,argv,short_options,long_options,&opt_index);
      if(opt == -1)
	break;
      
      switch(opt)
	{
	case OUTPUT_HEX:
	  hex_file = fopen(optarg,"w");
	  if(hex_file == NULL)
	    hex_file = stdout;
	  break;
	case 'a':
	  assembly_file = fopen(optarg,"w");
	  if(assembly_file == NULL)
	    assembly_file = stdout;
	  break;
	case 'o':
	  binary_file = fopen(optarg,"w");
	  if(binary_file == NULL)
	    {
	      fprintf(stderr,"Could not open %s for writing.\n",optarg);
	      exit(ENOENT);
	    }
	  break;
	case 'e':
	  eeprom_file = fopen(optarg,"w");
	  if(eeprom_file == NULL)
	    eeprom_file = stdout;
	  break;
	case 'h':
	  print_help();
	  return 0;
	default:
	  fprintf(stderr,"ERROR - Unknown flag %c\n",opt);
	  print_help();
	  return -1;
	}
    }
  printf("Welcome to the piclang compiler.\n");

  if(optind < argc)
    {
      FILE *input = fopen(argv[optind++],"r");
      extern FILE *yyin;
      if(input != NULL)
	yyin = input;
      if(assembly_file == NULL)
	{
	  printf("Assembly:\n");
	  assembly_file = stdout;
	}
    }

  
  yyparse();
  insert_code(EOP);

  if(hex_file == stdout)
    printf("Here comes your code.\nThank you come again.\nCODE:\n");
  pasm_compile(eeprom_file,hex_file,&the_code,the_strings,&piclang_bitmap,num_variables);

  if(binary_file != NULL)
    {
      curr_code = the_code;
      while(curr_code != NULL)
	{
	  fputc(curr_code->val,binary_file);
	  curr_code = curr_code->next;
	}
    }
  
  FreeCode(the_code);
  return 0;
}

int ex(nodeType *p) {
int lbl1, lbl2;

    if (!p) return 0;
    switch(p->type) {
    case typeCon:
      write_assembly(assembly_file,"\tpushl\t0x%x\n", p->con.value); 
      insert_code(PICLANG_PUSHL);
      insert_code(p->con.value);
      break;
    case typeId:        
      write_assembly(assembly_file,"\tpush\t%c\n", p->id.i + 'a');
      insert_code(PICLANG_PUSH);
      insert_code(resolve_variable(p->id.i));
      break;
    case typeStr:
      {
	char *pStr = p->str.string;
	if(pStr != NULL)
	  {
	    nodeType str_pointer;
	    int is_new = TRUE;
	    str_pointer.con.value = resolve_string(pStr,&is_new);
	    str_pointer.type = typeCon;
	    if(is_new)
	      {
		write_assembly(assembly_file,"\tstore\t\"%s\"\n", pStr);
		while(pStr != NULL)
		  {
		    insert_string(*pStr);
		    if(*pStr == 0)
		      break;
		    pStr++;
		  }
	      }
	    ex(&str_pointer);
	  }
	break;
      }
    case typeOpr:
        switch(p->opr.oper) {
	case EXIT_RETURN:
	  if(p->opr.nops == 0)
	    {
	      write_assembly(assembly_file,"\tpushl\t0\n",0); 
	      insert_code(PICLANG_PUSHL);
	      insert_code(0);
	    }
	  else
	    ex(p->opr.op[0]);
	  write_assembly(assembly_file,"\texit\n",0); 
	  insert_code(PICLANG_EXIT);
	  insert_code(EOP);
	  break;
	case CALL:
	  {
	    struct subroutine_map *subroutine = get_subroutine(p->opr.op[0]->str.string);
	    write_assembly(assembly_file,"\tcall L%03d\n",subroutine->label);
	    insert_code(PICLANG_CALL);
	    insert_code(subroutine->label);
	    break;
	  }
	case DEFINE:// KEEP RETURN AFTER DEFINE
	  {
	    const char *subroutine = p->opr.op[0]->str.string;
	  write_assembly(assembly_file,"L%03d:\n", lbl1 = lbl++);
	  insert_label(PICLANG_LABEL);
	  insert_subroutine(subroutine,lbl1);
	  ex(p->opr.op[1]);
	  if(strcmp(subroutine,"main") == 0)
	    {
	      write_assembly(assembly_file,"\texit\n");
	      break;
	    }
	  }
	case RETURN:// KEEP RETURN AFTER DEFINE
	  ex(p->opr.op[0]);
	  write_assembly(assembly_file,"\treturn\n");
	  insert_code(PICLANG_RETURN);
	  break;
        case WHILE:
            write_assembly(assembly_file,"L%03d:\n", lbl1 = lbl++);
	    insert_label(PICLANG_LABEL);
            ex(p->opr.op[0]);
            write_assembly(assembly_file,"\tjz\tL%03d\n", lbl2 = lbl++);
	    insert_code(PICLANG_JZ);
	    insert_code(lbl2);
            ex(p->opr.op[1]);
            write_assembly(assembly_file,"\tjmp\tL%03d\n", lbl1);
	    insert_code(PICLANG_JMP);
	    insert_code(lbl1);
            write_assembly(assembly_file,"L%03d:\n", lbl2);
	    insert_label(PICLANG_LABEL);
            break;
        case IF:
            ex(p->opr.op[0]);
            if (p->opr.nops > 2) {
                /* if else */
                write_assembly(assembly_file,"\tjz\tL%03d\n", lbl1 = lbl++);
		insert_code(PICLANG_JZ);
		insert_code(lbl1);
                ex(p->opr.op[1]);
                write_assembly(assembly_file,"\tjmp\tL%03d\n", lbl2 = lbl++);
		insert_code(PICLANG_JMP);
		insert_code(lbl2);
                write_assembly(assembly_file,"L%03d:\n", lbl1);
		insert_label(PICLANG_LABEL);
                ex(p->opr.op[2]);
                write_assembly(assembly_file,"L%03d:\n", lbl2);
		insert_label(PICLANG_LABEL);
            } else {
                /* if */
                write_assembly(assembly_file,"\tjz\tL%03d\n", lbl1 = lbl++);
		insert_code(PICLANG_JZ);
		insert_code(lbl1);
                ex(p->opr.op[1]);
                write_assembly(assembly_file,"L%03d:\n", lbl1);
		insert_label(PICLANG_LABEL);
            }
            break;
        case PUTD:     
	  ex(p->opr.op[0]);
	  write_assembly(assembly_file,"\tputd\n");insert_code(PICLANG_PRINTL);
	  break;
        case PUTCH:
	  ex(p->opr.op[0]);
	  write_assembly(assembly_file,"\tputch\n");insert_code(PICLANG_PRINT);
	  break;
        case FPUTCH:
	  ex(p->opr.op[0]);
	  write_assembly(assembly_file,"\tfputch\n");insert_code(PICLANG_FPUTCH);
	  break;
        case FPUTD:
	  ex(p->opr.op[0]);
	  write_assembly(assembly_file,"\tfputd\n");insert_code(PICLANG_FPUTD);
	  break;
        case '=':       
            ex(p->opr.op[1]);
            write_assembly(assembly_file,"\tpop\t%c\n", p->opr.op[0]->id.i + 'a');insert_code( PICLANG_POP);insert_code(resolve_variable(p->opr.op[0]->id.i));
            break;
        case UMINUS:    
            ex(p->opr.op[0]);
            write_assembly(assembly_file,"\tneg\n");
            break;
        case SYSTEM:
	  {
	    int op_counter = p->opr.nops - 1;
	    for(;op_counter >= 0 ;op_counter--)
		ex(p->opr.op[op_counter]);
	    write_assembly(assembly_file,"\tsystem\n");insert_code(PICLANG_SYSTEM);
	    break;
	  }
	case SPRINT:
	  ex(p->opr.op[0]);
	  write_assembly(assembly_file,"\tsprint\n");
	  insert_code(PICLANG_SPRINT);
	  break;
	case MORSE:
	  ex(p->opr.op[1]);
	  ex(p->opr.op[0]);
	  write_assembly(assembly_file,"\tmorse\n");
	  insert_code(PICLANG_MORSE);
	  break;
	case TIME:
	  ex(p->opr.op[0]);
	  write_assembly(assembly_file,"\ttime\n");
	  insert_code(PICLANG_TIME);
	  break;
	case FFLUSH:
	  write_assembly(assembly_file,"\tfflush\n");
	  insert_code(PICLANG_FFLUSH);
	  break;
	case CLEAR:
	  write_assembly(assembly_file,"\tclear\n");
	  insert_code(PICLANG_CLEAR);
	  break;
	case SET_TIME:
	  ex(p->opr.op[0]);
	  ex(p->opr.op[1]);
	  write_assembly(assembly_file,"\tsettime\n");
	  insert_code(PICLANG_SET_TIME);
	  break;
	case SET_DATE:
	  ex(p->opr.op[0]);
	  ex(p->opr.op[1]);
	  ex(p->opr.op[2]);
	  write_assembly(assembly_file,"\tsetdate\n");
	  insert_code(PICLANG_SET_DATE);
	  break;
	case ARGCH:
	  write_assembly(assembly_file,"\targch\n");
	  insert_code(PICLANG_ARGCH);
	  break;
	case ARGD:
	  write_assembly(assembly_file,"\targd\n");
	  insert_code(PICLANG_ARGD);
	  break;
	case GETD:
	  write_assembly(assembly_file,"\tgetd\n");insert_code(PICLANG_GETD);
	  break;
	case GETCH:
	  write_assembly(assembly_file,"\tgetch\n");insert_code(PICLANG_GETCH);
	  break;
        default:
            ex(p->opr.op[0]);
            ex(p->opr.op[1]);
            switch(p->opr.oper) {
            case '+':   
	      write_assembly(assembly_file,"\tadd \n"); 
	      insert_code(PICLANG_ADD);
	      break;
            case '-':   
	      write_assembly(assembly_file,"\tsub\n");
	      insert_code(PICLANG_SUB); 
	      break; 
            case '*':   
	      write_assembly(assembly_file,"\tmul\n");
	      insert_code(PICLANG_MULT); 
	      break;
            case '/':   
	      write_assembly(assembly_file,"\tdiv\n"); 
	      break;
            case '<':   
	      write_assembly(assembly_file,"\tcompLT\n"); 
	      insert_code(PICLANG_COMPLT);
	      break;
            case '>':   
	      write_assembly(assembly_file,"\tcompGT\n"); 
	      insert_code(PICLANG_COMPGT);
	      break;
            case GE:    
	      write_assembly(assembly_file,"\tcompGE\n"); 
	      break;
            case LE:   
	      write_assembly(assembly_file,"\tcompLE\n"); 
	      break;
            case NE:    
	      write_assembly(assembly_file,"\tcompNE\n"); 
	      insert_code(PICLANG_COMPNE);
	      break;
            case EQ:    
	      write_assembly(assembly_file,"\tcompEQ\n"); 
	      insert_code(PICLANG_COMPEQ);
	      break;
            }
        }
    }
    return 0;
}

int resolve_string(const char *str, int *is_new)
{
  size_t i,retval;
  if(string_list == NULL)
    {
      string_list = (char**)malloc(sizeof(char*));
      string_list[0] = strdup(str);
      num_strings = 1;
      if(is_new != NULL)
	*is_new = TRUE;
      return 0;
    }
  
  if(is_new != NULL)
    *is_new = FALSE;
  i = 0;
  retval = 0;
  for(;i<num_strings;i++)
    if(strcmp(string_list[i],str) == 0)
      return retval;
    else
      retval += strlen(string_list[i]) + 1;
  
  if(is_new != NULL)
    *is_new = TRUE;
  string_list = (char**)realloc(string_list,(num_strings+1)*sizeof(char*));
  string_list[num_strings++] = strdup(str);
  return retval;
}

int resolve_variable(const int id)
{
  int i;
  if(variable_list == NULL)
    {
      variable_list = (int*)malloc(sizeof(int));
      variable_list[0] = id;
      num_variables = 1;
      return 0;
    }
  
  i = 0;
  for(;i<num_variables;i++)
    if(variable_list[i] == id)
      return i;
  
  variable_list = (int*)realloc(variable_list,(num_variables+1)*sizeof(int));
  variable_list[num_variables++] = id;
  return num_variables - 1;
  
}

const struct subroutine_map* get_subroutine(const char *name)
{
  const struct subroutine_map *retval = NULL;
  if(name == NULL)
    {
      fprintf(stderr,"NULL pointer for the subroutine name.\n");
      exit(-1);
    }
  if(subroutines == NULL)
    {
      fprintf(stderr,"No subroutines yet defined.\n");
      exit(-1);
    }
  
  retval = subroutines;
  while(retval != NULL)
    {
      if(strcmp(retval->name,name) == 0)
	return retval;
      retval = retval->next;
    }
  fprintf(stderr,"No such subroutine: %s\n",name);
  exit(-1);
}

struct compiled_code* MakePCB(struct compiled_code *the_code, struct compiled_code *the_strings, int total_memory, unsigned char piclang_bitmap)
{
  int i;
  struct compiled_code *magic_number = NULL;
  struct compiled_code *size = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  struct compiled_code *bitmap = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  bitmap->val = piclang_bitmap;
  struct compiled_code *num_pages = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  num_pages->val = (unsigned char)ceil(1.0*total_memory/PAGE_SIZE);
  struct compiled_code *pc = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  struct compiled_code *status = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  status->val = PICLANG_SUCCESS;
  struct compiled_code *start_address = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  start_address->val = PCB_SIZE;
  struct compiled_code *string_address = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  struct compiled_code *stack, *end_of_stack;
  struct compiled_code *call_stack, *end_of_call_stack;
  create_stack(&stack,&end_of_stack,PICLANG_STACK_SIZE);
  end_of_stack->next->val = 0;
  end_of_stack = end_of_stack->next;// stack head

  create_stack(&call_stack,&end_of_call_stack,PICLANG_CALL_STACK_SIZE);
  end_of_call_stack->next->val = 0;
  end_of_call_stack = end_of_call_stack->next;// stack head
  
  // Piece the linked list together
  size->next = bitmap;
  bitmap->next = num_pages;
  num_pages->next = pc;
  pc->next = status;
  status->next = start_address;
  start_address->next = string_address;
  string_address->next = stack;
  end_of_stack->next = call_stack;
  end_of_call_stack->next = NULL;// temporary to count PCB's size
  start_address->val = CountCode(size);

  end_of_call_stack->next = the_code;
  string_address->val = CountCode(size);

  if(the_code == NULL)
    {
      fprintf(stderr,"No code to compile!\n");
      exit -1;
    }
  
  // Find the location of the main function
  pc->val = lookup_label(the_code,((struct subroutine_map*) get_subroutine("main"))->label);

  while(the_code->next != NULL)
    the_code = the_code->next;
  if(the_strings != NULL)
    the_code->next = the_strings;
  else
    {
      the_code->next = (struct compiled_code*)malloc(sizeof(struct compiled_code));
      the_code->next->val = 0;
    }
  
  size->val =  CountCode(size);
  
  // Magic number to identify the executable
  i = PCB_MAGIC_NUMBER_OFFSET - 1;
  for(;i >= 0;i--)
    {
      magic_number = (struct compiled_code*)malloc(sizeof(struct compiled_code));
      magic_number->next = size;
      magic_number->val = PICLANG_magic_numbers[i];
      size = magic_number;
    }


  return size;
}

void pasm_compile(FILE *eeprom_file,FILE *hex_file,struct compiled_code **the_code, struct compiled_code *the_strings, unsigned char *piclang_bitmap, int num_variables)
{
  char hex_buffer[45];
  void resolve_labels(struct compiled_code* code);

  resolve_labels(*the_code);
  *the_code = MakePCB(*the_code,the_strings,num_variables,piclang_bitmap);
  memset(hex_buffer,0,(9 + COMPILE_MAX_WIDTH + 2)*sizeof(char));// header + data + checksum
  if(hex_file != NULL)
    {
      fprintf(hex_file,":020000040000FA\n");
      FPrintCode(hex_file,*the_code,0,hex_buffer,0x4200,0,PRINT_HEX);
      fprintf(hex_file,":00000001FF\n");
    }
  if(eeprom_file != NULL)
    FPrintCode(eeprom_file,*the_code,0,hex_buffer,0x4200,0,PRINT_EEPROM_DATA);

}






