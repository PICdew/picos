
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
#define YYLSP_NEEDED 1



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

extern struct assembly_map opcodes[];

char **string_list;
size_t num_strings;
picos_size_t FS_BUFFER_SIZE;

idNodeType *variable_list = NULL;// Variable table
extern picos_size_t label_counter;
 int break_to_label, continue_to_label;

/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *id(idNodeType var);
nodeType *con(int value);
void freeNode(nodeType *p);
int ex(nodeType *p);
int yylex(void);
 FILE *assembly_file;
 FILE *lst_file;
void yyerror(char *s);
 int resolve_string(const char *str, int *is_new);
 int resolve_variable(const char *name);// Looks up a variable and retrieves its page memory index. If the variable does not yet exist, it will be added to the list.
 extern char *yytext;
 extern char *last_string;
 nodeType* store_string(const char *);
  


/* Line 189 of yacc.c  */
#line 129 "pasm_yacc.c"

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
     FUNCT = 259,
     VARIABLE = 260,
     WHILE = 261,
     BREAK = 262,
     CONTINUE = 263,
     IF = 264,
     CALL = 265,
     SUBROUTINE = 266,
     STRING = 267,
     RETURN = 268,
     DEFINE = 269,
     EXIT = 270,
     PASM_CR = 271,
     PASM_POP = 272,
     ARGV = 273,
     ARGC = 274,
     FIN = 275,
     FEOF = 276,
     STATEMENT_DELIM = 277,
     IFX = 278,
     ELSE = 279,
     NE = 280,
     EQ = 281,
     LE = 282,
     GE = 283,
     BSR = 284,
     BSL = 285,
     UMINUS = 286
   };
#endif
/* Tokens.  */
#define INTEGER 258
#define FUNCT 259
#define VARIABLE 260
#define WHILE 261
#define BREAK 262
#define CONTINUE 263
#define IF 264
#define CALL 265
#define SUBROUTINE 266
#define STRING 267
#define RETURN 268
#define DEFINE 269
#define EXIT 270
#define PASM_CR 271
#define PASM_POP 272
#define ARGV 273
#define ARGC 274
#define FIN 275
#define FEOF 276
#define STATEMENT_DELIM 277
#define IFX 278
#define ELSE 279
#define NE 280
#define EQ 281
#define LE 282
#define GE 283
#define BSR 284
#define BSL 285
#define UMINUS 286




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 56 "pasm_yacc.y"

    int iValue;                 /* integer value */
    idNodeType variable;          /* symbol table index */
    nodeType *nPtr;             /* node pointer */



/* Line 214 of yacc.c  */
#line 235 "pasm_yacc.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 260 "pasm_yacc.c"

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
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

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
#define YYLAST   460

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  51
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  6
/* YYNRULES -- Number of rules.  */
#define YYNRULES  50
/* YYNRULES -- Number of states.  */
#define YYNSTATES  110

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   286

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    37,    48,     2,
      40,    41,    35,    33,    47,    34,     2,    36,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    39,
      26,    42,    25,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    45,     2,    46,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    43,    49,    44,    50,     2,     2,     2,
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
      27,    28,    29,    30,    31,    32,    38
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     8,     9,    11,    14,    18,    22,
      27,    30,    35,    41,    44,    47,    53,    61,    65,    67,
      69,    72,    74,    76,    78,    80,    82,    84,    89,    93,
      98,   105,   112,   121,   125,   130,   133,   137,   141,   145,
     149,   153,   157,   161,   165,   169,   172,   176,   180,   184,
     188
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      52,     0,    -1,    53,    -1,    53,    54,    -1,    -1,    39,
      -1,    13,    54,    -1,    10,    11,    39,    -1,    14,    11,
      54,    -1,    16,    40,    41,    39,    -1,    56,    39,    -1,
       5,    42,    56,    39,    -1,     6,    40,    56,    41,    54,
      -1,     7,    39,    -1,     8,    39,    -1,     9,    40,    56,
      41,    54,    -1,     9,    40,    56,    41,    54,    24,    54,
      -1,    43,    55,    44,    -1,    15,    -1,    54,    -1,    55,
      54,    -1,     3,    -1,     5,    -1,    12,    -1,    19,    -1,
      20,    -1,    21,    -1,    18,    45,    56,    46,    -1,    17,
      40,    41,    -1,     4,    40,    56,    41,    -1,     4,    40,
       3,    47,    11,    41,    -1,     4,    40,    56,    47,    56,
      41,    -1,     4,    40,    56,    47,    56,    47,    56,    41,
      -1,     4,    40,    41,    -1,    56,    45,    56,    46,    -1,
      34,    56,    -1,    56,    33,    56,    -1,    56,    34,    56,
      -1,    56,    35,    56,    -1,    56,    36,    56,    -1,    56,
      26,    56,    -1,    56,    25,    56,    -1,    56,    37,    56,
      -1,    56,    48,    56,    -1,    56,    49,    56,    -1,    50,
      56,    -1,    56,    32,    56,    -1,    56,    31,    56,    -1,
      56,    27,    56,    -1,    56,    28,    56,    -1,    40,    56,
      41,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    84,    84,    88,    89,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   110,
     111,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INTEGER", "FUNCT", "VARIABLE", "WHILE",
  "BREAK", "CONTINUE", "IF", "CALL", "SUBROUTINE", "STRING", "RETURN",
  "DEFINE", "EXIT", "PASM_CR", "PASM_POP", "ARGV", "ARGC", "FIN", "FEOF",
  "STATEMENT_DELIM", "IFX", "ELSE", "'>'", "'<'", "NE", "EQ", "LE", "GE",
  "BSR", "BSL", "'+'", "'-'", "'*'", "'/'", "'%'", "UMINUS", "';'", "'('",
  "')'", "'='", "'{'", "'}'", "'['", "']'", "','", "'&'", "'|'", "'~'",
  "$accept", "program", "function", "stmt", "stmt_list", "expr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,    62,    60,   280,   281,   282,
     283,   284,   285,    43,    45,    42,    47,    37,   286,    59,
      40,    41,    61,   123,   125,    91,    93,    44,    38,   124,
     126
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    51,    52,    53,    53,    54,    54,    54,    54,    54,
      54,    54,    54,    54,    54,    54,    54,    54,    54,    55,
      55,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     0,     1,     2,     3,     3,     4,
       2,     4,     5,     2,     2,     5,     7,     3,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     4,     3,     4,
       6,     6,     8,     3,     4,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     3,     3,     3,     3,
       3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,     2,     1,    21,     0,    22,     0,     0,     0,
       0,     0,    23,     0,     0,    18,     0,     0,     0,    24,
      25,    26,     0,     5,     0,     0,     0,     3,     0,     0,
       0,     0,    13,    14,     0,     0,     6,     0,     0,     0,
       0,    22,    35,     0,    19,     0,    45,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    10,     0,
       0,     0,    21,    33,     0,     0,     0,     0,     7,     8,
       0,    28,     0,    50,    17,    20,    41,    40,    48,    49,
      47,    46,    36,    37,    38,    39,    42,     0,    43,    44,
       0,    29,     0,    11,     0,     0,     9,    27,    34,     0,
       0,    12,    15,    30,    31,     0,     0,     0,    16,    32
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     2,    27,    45,    28
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -45
static const yytype_int16 yypact[] =
{
     -45,     2,    99,   -45,   -45,   -37,   -36,   -33,   -31,   -30,
     -29,    -1,   -45,    99,     3,   -45,   -24,   -21,   -22,   -45,
     -45,   -45,   151,   -45,   151,    99,   151,   -45,   211,   140,
     151,   151,   -45,   -45,   151,   -12,   -45,    99,    -8,    -7,
     151,   -45,   -44,   236,   -45,    51,   411,   151,   151,   151,
     151,   151,   151,   151,   151,   151,   151,   151,   -45,   151,
     151,   151,   -11,   -45,   161,   261,   286,   311,   -45,   -45,
      -4,   -45,   336,   -45,   -45,   -45,    92,    92,    92,    92,
      92,    92,    39,    39,   -19,   -19,   -44,   361,   411,   411,
      17,   -45,   151,   -45,    99,    99,   -45,   -45,   -45,     8,
     186,   -45,    13,   -45,   -45,   151,    99,   386,   -45,   -45
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -45,   -45,   -45,   -13,   -45,    -9
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      36,    59,     3,    29,    60,    61,    30,    31,    32,    33,
      35,    34,    44,    42,    37,    43,    38,    46,    57,    39,
      64,    65,    66,    40,    69,    67,    59,    68,    99,    60,
      61,    72,    75,    70,    71,    96,    90,   106,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,   103,
      87,    88,    89,     0,     4,     5,     6,     7,     8,     9,
      10,    11,     0,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,     0,    55,    56,    57,     0,     0,     0,
       0,   101,   102,   100,    59,    22,     0,    60,    61,     0,
      23,    24,     0,   108,    25,    74,   107,     0,     0,     0,
       0,    26,     4,     5,     6,     7,     8,     9,    10,    11,
       0,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,     0,     0,     0,     0,    53,    54,    55,    56,    57,
       0,     0,     0,    22,     0,     0,     0,    59,    23,    24,
      60,    61,    25,    62,     5,    41,     0,     0,     0,    26,
       0,     0,    12,     0,     4,     5,    41,    17,    18,    19,
      20,    21,     0,    12,     0,     0,     0,     0,    17,    18,
      19,    20,    21,     0,    22,     0,     0,     0,     0,     0,
      24,    63,     0,     0,     0,    22,    47,    48,    49,    50,
      26,    24,    51,    52,    53,    54,    55,    56,    57,     0,
       0,    26,    91,     0,     0,     0,    59,     0,    92,    60,
      61,    47,    48,    49,    50,     0,     0,    51,    52,    53,
      54,    55,    56,    57,     0,     0,     0,   104,     0,     0,
       0,    59,     0,   105,    60,    61,    47,    48,    49,    50,
       0,     0,    51,    52,    53,    54,    55,    56,    57,     0,
      58,     0,     0,     0,     0,     0,    59,     0,     0,    60,
      61,    47,    48,    49,    50,     0,     0,    51,    52,    53,
      54,    55,    56,    57,     0,     0,     0,    73,     0,     0,
       0,    59,     0,     0,    60,    61,    47,    48,    49,    50,
       0,     0,    51,    52,    53,    54,    55,    56,    57,     0,
      93,     0,     0,     0,     0,     0,    59,     0,     0,    60,
      61,    47,    48,    49,    50,     0,     0,    51,    52,    53,
      54,    55,    56,    57,     0,     0,     0,    94,     0,     0,
       0,    59,     0,     0,    60,    61,    47,    48,    49,    50,
       0,     0,    51,    52,    53,    54,    55,    56,    57,     0,
       0,     0,    95,     0,     0,     0,    59,     0,     0,    60,
      61,    47,    48,    49,    50,     0,     0,    51,    52,    53,
      54,    55,    56,    57,     0,     0,     0,     0,     0,     0,
       0,    59,    97,     0,    60,    61,    47,    48,    49,    50,
       0,     0,    51,    52,    53,    54,    55,    56,    57,     0,
       0,     0,     0,     0,     0,     0,    59,    98,     0,    60,
      61,    47,    48,    49,    50,     0,     0,    51,    52,    53,
      54,    55,    56,    57,     0,     0,     0,   109,     0,     0,
       0,    59,     0,     0,    60,    61,    47,    48,    49,    50,
       0,     0,    51,    52,    53,    54,    55,    56,    57,     0,
       0,     0,     0,     0,     0,     0,    59,     0,     0,    60,
      61
};

static const yytype_int8 yycheck[] =
{
      13,    45,     0,    40,    48,    49,    42,    40,    39,    39,
      11,    40,    25,    22,    11,    24,    40,    26,    37,    40,
      29,    30,    31,    45,    37,    34,    45,    39,    11,    48,
      49,    40,    45,    41,    41,    39,    47,    24,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    41,
      59,    60,    61,    -1,     3,     4,     5,     6,     7,     8,
       9,    10,    -1,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    -1,    35,    36,    37,    -1,    -1,    -1,
      -1,    94,    95,    92,    45,    34,    -1,    48,    49,    -1,
      39,    40,    -1,   106,    43,    44,   105,    -1,    -1,    -1,
      -1,    50,     3,     4,     5,     6,     7,     8,     9,    10,
      -1,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    -1,    -1,    -1,    -1,    33,    34,    35,    36,    37,
      -1,    -1,    -1,    34,    -1,    -1,    -1,    45,    39,    40,
      48,    49,    43,     3,     4,     5,    -1,    -1,    -1,    50,
      -1,    -1,    12,    -1,     3,     4,     5,    17,    18,    19,
      20,    21,    -1,    12,    -1,    -1,    -1,    -1,    17,    18,
      19,    20,    21,    -1,    34,    -1,    -1,    -1,    -1,    -1,
      40,    41,    -1,    -1,    -1,    34,    25,    26,    27,    28,
      50,    40,    31,    32,    33,    34,    35,    36,    37,    -1,
      -1,    50,    41,    -1,    -1,    -1,    45,    -1,    47,    48,
      49,    25,    26,    27,    28,    -1,    -1,    31,    32,    33,
      34,    35,    36,    37,    -1,    -1,    -1,    41,    -1,    -1,
      -1,    45,    -1,    47,    48,    49,    25,    26,    27,    28,
      -1,    -1,    31,    32,    33,    34,    35,    36,    37,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    25,    26,    27,    28,    -1,    -1,    31,    32,    33,
      34,    35,    36,    37,    -1,    -1,    -1,    41,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    25,    26,    27,    28,
      -1,    -1,    31,    32,    33,    34,    35,    36,    37,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    25,    26,    27,    28,    -1,    -1,    31,    32,    33,
      34,    35,    36,    37,    -1,    -1,    -1,    41,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    25,    26,    27,    28,
      -1,    -1,    31,    32,    33,    34,    35,    36,    37,    -1,
      -1,    -1,    41,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    25,    26,    27,    28,    -1,    -1,    31,    32,    33,
      34,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    46,    -1,    48,    49,    25,    26,    27,    28,
      -1,    -1,    31,    32,    33,    34,    35,    36,    37,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    46,    -1,    48,
      49,    25,    26,    27,    28,    -1,    -1,    31,    32,    33,
      34,    35,    36,    37,    -1,    -1,    -1,    41,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    25,    26,    27,    28,
      -1,    -1,    31,    32,    33,    34,    35,    36,    37,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    52,    53,     0,     3,     4,     5,     6,     7,     8,
       9,    10,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    34,    39,    40,    43,    50,    54,    56,    40,
      42,    40,    39,    39,    40,    11,    54,    11,    40,    40,
      45,     5,    56,    56,    54,    55,    56,    25,    26,    27,
      28,    31,    32,    33,    34,    35,    36,    37,    39,    45,
      48,    49,     3,    41,    56,    56,    56,    56,    39,    54,
      41,    41,    56,    41,    44,    54,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      47,    41,    47,    39,    41,    41,    39,    46,    46,    11,
      56,    54,    54,    41,    41,    47,    24,    56,    54,    41
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
		  Type, Value, Location); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
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
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
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
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

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

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

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
       `yyls': related to locations.

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

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
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
  yylsp = yyls;

#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

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
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
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
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

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
  *++yylsp = yylloc;
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

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 84 "pasm_yacc.y"
    { YYACCEPT; }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 88 "pasm_yacc.y"
    { ex((yyvsp[(2) - (2)].nPtr)); freeNode((yyvsp[(2) - (2)].nPtr)); }
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 93 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PASM_STATEMENT_DELIM, 2, NULL, NULL); }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 94 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_RETURN,1,(yyvsp[(2) - (2)].nPtr));}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 95 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_CALL,1,(yyvsp[(2) - (3)].nPtr)); }
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 96 "pasm_yacc.y"
    {  (yyval.nPtr) = opr(PASM_DEFINE,2,(yyvsp[(2) - (3)].nPtr),(yyvsp[(3) - (3)].nPtr));}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 97 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_PRINTL,1,con(0xa));}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 98 "pasm_yacc.y"
    { (yyval.nPtr) = (yyvsp[(1) - (2)].nPtr); }
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 99 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_POP, 2, id((yyvsp[(1) - (4)].variable)), (yyvsp[(3) - (4)].nPtr)); }
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 100 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PASM_WHILE, 2, (yyvsp[(3) - (5)].nPtr), (yyvsp[(5) - (5)].nPtr)); }
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 101 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PASM_BREAK, 0); }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 102 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PASM_CONTINUE, 0); }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 103 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PASM_IF, 2, (yyvsp[(3) - (5)].nPtr), (yyvsp[(5) - (5)].nPtr)); }
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 104 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PASM_IF, 3, (yyvsp[(3) - (7)].nPtr), (yyvsp[(5) - (7)].nPtr), (yyvsp[(7) - (7)].nPtr)); }
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 105 "pasm_yacc.y"
    { (yyval.nPtr) = (yyvsp[(2) - (3)].nPtr); }
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 106 "pasm_yacc.y"
    {YYACCEPT;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 110 "pasm_yacc.y"
    { (yyval.nPtr) = (yyvsp[(1) - (1)].nPtr); }
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 111 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PASM_STATEMENT_DELIM, 2, (yyvsp[(1) - (2)].nPtr), (yyvsp[(2) - (2)].nPtr)); }
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 115 "pasm_yacc.y"
    { (yyval.nPtr) = con((yyvsp[(1) - (1)].iValue)); }
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 116 "pasm_yacc.y"
    { (yyval.nPtr) = id((yyvsp[(1) - (1)].variable)); }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 117 "pasm_yacc.y"
    { (yyval.nPtr) = con(handle_string((yyvsp[(1) - (1)].nPtr)->str.string)); }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 118 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_ARGC,0); }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 119 "pasm_yacc.y"
    { (yyval.nPtr) = con(ARG_SIZE); }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 120 "pasm_yacc.y"
    { (yyval.nPtr) = con(((picos_size_t)(-1))); }
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 121 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_ARGV,1,(yyvsp[(3) - (4)].nPtr)); }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 122 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_POP,0); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 123 "pasm_yacc.y"
    { (yyval.nPtr) = opr((yyvsp[(1) - (4)].iValue),1,(yyvsp[(3) - (4)].nPtr)); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 124 "pasm_yacc.y"
    { (yyval.nPtr) = opr((yyvsp[(1) - (6)].iValue),2,con((yyvsp[(3) - (6)].iValue)),(yyvsp[(5) - (6)].nPtr)); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 125 "pasm_yacc.y"
    { (yyval.nPtr) = opr((yyvsp[(1) - (6)].iValue),2,(yyvsp[(3) - (6)].nPtr),(yyvsp[(5) - (6)].nPtr)); }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 126 "pasm_yacc.y"
    { (yyval.nPtr) = opr((yyvsp[(1) - (8)].iValue),3,(yyvsp[(3) - (8)].nPtr),(yyvsp[(5) - (8)].nPtr),(yyvsp[(7) - (8)].nPtr)); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 127 "pasm_yacc.y"
    { (yyval.nPtr) = opr((yyvsp[(1) - (3)].iValue),0); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 128 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_DEREF,2, (yyvsp[(1) - (4)].nPtr),(yyvsp[(3) - (4)].nPtr)); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 129 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_UMINUS, 1, (yyvsp[(2) - (2)].nPtr)); }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 130 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_ADD, 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 131 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_SUB, 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 132 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_MULT, 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 133 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_DIV, 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 134 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_COMPLT, 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 135 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_COMPGT, 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 136 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_MOD, 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 137 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_AND, 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 138 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_OR, 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 139 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_NOT, 1, (yyvsp[(2) - (2)].nPtr)); }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 140 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_BSL, 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 141 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_BSR, 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 142 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_COMPNE, 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 143 "pasm_yacc.y"
    { (yyval.nPtr) = opr(PICLANG_COMPEQ, 2, (yyvsp[(1) - (3)].nPtr), (yyvsp[(3) - (3)].nPtr)); }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 144 "pasm_yacc.y"
    { (yyval.nPtr) = (yyvsp[(2) - (3)].nPtr); }
    break;



/* Line 1455 of yacc.c  */
#line 1984 "pasm_yacc.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

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

  yyerror_range[0] = yylloc;

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
		      yytoken, &yylval, &yylloc);
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

  yyerror_range[0] = yylsp[1-yylen];
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

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

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
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
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
#line 147 "pasm_yacc.y"


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

int handle_string(const char *pStr)
{
  int retval = -1;
  if(pStr != NULL)
    {
      int is_new = TRUE;
      retval = resolve_string(pStr,&is_new) + PICLANG_STRING_OFFSET;// when referencing strings, arguments will go first.
      if(is_new)
	{
	  if(assembly_file != NULL)
	    write_assembly(assembly_file,"\t; \"%s\" = %d\n", pStr,retval);
	  while(pStr != NULL)
	    {
	      insert_string(*pStr);
	      if(*pStr == 0)
		break;
	      pStr++;
	    }
	}
    }
  else
    {
      yyerror("Invalid string");
    }
  return retval;
}

nodeType *id(idNodeType variable_node) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeId;
    p->id.i = variable_node.i;
    strcpy(p->id.name, variable_node.name);
    p->id.next = NULL;

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
  fprintf(stdout, "(%d - %d) %s: %s\n",yylloc.first_line,yylloc.last_line, s,yytext);
    exit(-1);
}


void write_val_for_pic(FILE *binary_file,picos_size_t val)
{
  picos_size_t endiantest = 1;
  int size;
  char is_littleendian, *end; 
  if(binary_file == NULL)
    return;

  // Ensure little endian is used.
  size = sizeof(val);// in bytes
  end = (char*)&endiantest;
  is_littleendian = *end;
  if(is_littleendian)
    {
      //little endian
      end = (char*)&val;
      while(size > 0)
	{
	  fprintf(binary_file,"%c",*end);
	  end++;
	  size--;
	}
    }
  else
    {
      while(size > 0)
	{
	  fprintf(binary_file,"%c",(val & 0xff));
	  val >> 8;
	  size--;
	}
    }
  
}


static const char short_options[] = "a:b:e:hl:o:";
enum OPTION_INDICES{OUTPUT_HEX};
static struct option long_options[] =
             {
	       {"help",0,NULL,'h'},
               {"hex", 1,NULL, OUTPUT_HEX},
	       {"asm", 1,NULL, 'a'},
	       {"eeprom",1,NULL, 'e'},
	       {"binary",1,NULL,'o'},
	       {"list",1,NULL,'l'},
	       {"buffer_size",1,NULL,'b'},
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
  printf("--list, -l <file> :\t Outputs a list of program addresses (PC values) for each assembly entry.\n");
  printf("--block_size, -b <INT> :\t Sets the size of block of the target PICFS (Default: 128)");
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
  variable_list = NULL;
  subroutines = NULL;
  break_to_label = -1;
  continue_to_label = -1;
  FS_BUFFER_SIZE = 128;
  
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
	case 'b':
	  if(sscanf(optarg,"%hu",&FS_BUFFER_SIZE) != 1)
	    {
	      fprintf(stderr,"Could not read buffers size: %s\n",optarg);
	      exit(-1);
	    }
	  break;
	case 'a':
	  assembly_file = fopen(optarg,"w");
	  if(assembly_file == NULL)
	    assembly_file = stdout;
	  break;
	case 'l':
	  lst_file = fopen(optarg,"w");
	  if(lst_file == NULL)
	    lst_file = stdout;
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

  if(optind < argc)
    {
      FILE *input = fopen(argv[optind++],"r");
      extern FILE *yyin;
      if(input != NULL)
	yyin = input;
    }
  else
    printf("Welcome to the piclang compiler.\n");
  
  yyparse();
  insert_code(PICLANG_PUSHL);
  insert_code(PICLANG_SUCCESS);
  insert_code(PICLANG_EXIT);

  if(hex_file == stdout)
    printf("Here comes your code.\nThank you come again.\nCODE:\n");
  pasm_compile(eeprom_file,hex_file,&the_code,the_strings,&piclang_bitmap,count_variables());

  if(binary_file != NULL)
    {
      curr_code = the_code;
      while(curr_code != NULL)
	{
	  if(curr_code->type != typeStr)
	    write_val_for_pic(binary_file,curr_code->val);
	  else
	    fprintf(binary_file,"%c",(char)curr_code->val);
	  curr_code = curr_code->next;
	}
    }

  if(lst_file != NULL)
    {
      struct assembly_map* curr;
      int code_counter = 0;
      curr_code = the_code;
      for(;curr_code != NULL;curr_code = curr_code->next)
	{
	  if(curr_code->type == typePCB)
	    continue;
	  if(curr_code->type == typeStr)
	    break;
	  curr = opcode2assembly(curr_code->val);
	  fprintf(lst_file,"(%d)\t",code_counter++);
	  switch(curr->opcode)
	    {
	    case PICLANG_NUM_COMMANDS:
	      if(curr_code->type == typeLabel)
		{
		  fprintf(lst_file,"L%03hu",curr_code->label);
		  break;
		}
	      else if(curr_code->val == PICLANG_RETURN)
		{
		  fprintf(lst_file,"return");
		  break;
		}
	      else if(curr_code->val == PICLANG_CALL)
		{
		  curr_code = curr_code->next;
		  fprintf(lst_file,"call %hu",curr_code->val);
		  code_counter++;
		  break;
		}
	    default:
	      fprintf(lst_file,"%s",curr->keyword);
	      break;
	    }
	  if(curr->has_arg)
	    {
	      curr_code = curr_code->next;
	      code_counter++;
	      fprintf(lst_file," %d",curr_code->val);
	      if(curr->has_arg > 1)
		{
		  int arg_counter = 1;
		  for(;arg_counter < curr->has_arg;arg_counter++)
		    {
		      code_counter++;
		      curr_code = curr_code->next;
		      fprintf(lst_file,", %d",curr_code->val);
		    }
		}
	    }
	  fprintf(lst_file,"\n");
	}
      // print strings
      if(curr_code != NULL)
	fprintf(lst_file,"Strings:\n\"");
      for(;curr_code != NULL;curr_code = curr_code->next)
	{
	  if(curr_code->val == 0)
	    {
	      fprintf(lst_file,"\"\n");
	      if(curr_code->next != NULL)
		fprintf(lst_file,"\"");
	    }
	  else if(curr_code->val == '"')
	    fprintf(lst_file,"\"%c",curr_code->val);
	  else
	    fprintf(lst_file,"%c",curr_code->val);
	}
    }
  
  FreeCode(the_code);
  return 0;
}

int ex(nodeType *p) {
  int lbl1, lbl2;
  int previous_break_to_label = break_to_label;
  int previous_continue_to_label = continue_to_label;
  if (!p) return 0;
  switch(p->type) {
  case typeCon:
    write_assembly(assembly_file,"\tpushl\t0x%x\n", p->con.value); 
    insert_code(PICLANG_PUSHL);
    insert_code(p->con.value);
    break;
  case typeId:        
    write_assembly(assembly_file,"\tpush\t%s\n", p->id.name);
    insert_code(PICLANG_PUSH);
    insert_code(resolve_variable(p->id.name));
    break;
  case typeOpr:
    switch(p->opr.oper) {
    case PICLANG_EXIT:
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
      break;
    case PICLANG_PUSH:
      ex(p->opr.op[0]);
      break;
    case PICLANG_CALL:
      {
	struct subroutine_map *subroutine = get_subroutine(p->opr.op[0]->str.string);
	write_assembly(assembly_file,"\tcall\tL%03d\n",subroutine->label);
	insert_code(PICLANG_CALL);
	insert_code(subroutine->label);
	break;
      }
    case PASM_DEFINE:// KEEP RETURN AFTER DEFINE
      {
	const char *subroutine = p->opr.op[0]->str.string;
	write_assembly(assembly_file,"L%03d:\t;<%s>\n", (lbl1 = label_counter),subroutine);
	label_counter++;
	insert_label(PICLANG_LABEL,lbl1);
	insert_subroutine(subroutine,lbl1);
	ex(p->opr.op[1]);
	if(strcmp(subroutine,"main") == 0)
	  {
	    write_assembly(assembly_file,"\texit\n");//eop will be written by the compile routine
	    break;
	  }
      }
    case PICLANG_RETURN:// KEEP RETURN AFTER DEFINE
      ex(p->opr.op[0]);
      write_assembly(assembly_file,"\treturn\n");
      insert_code(PICLANG_RETURN);
      break;
    case PASM_CONTINUE:
      if(continue_to_label < 0)
	{
	  yyerror("Not within a block in which to continue");
	  exit(-1);
	}
      write_assembly(assembly_file,"\tjmp\tL%03d\n", continue_to_label);
      insert_code(PICLANG_JMP);
      insert_code(continue_to_label);
      break;
    case PASM_BREAK:
      if(break_to_label < 0)
	{
	  yyerror("Not within a block from which to break");
	  exit(-1);
	}
      write_assembly(assembly_file,"\tjmp\tL%03d\n", break_to_label);
      insert_code(PICLANG_JMP);
      insert_code(break_to_label);
      break;
    case PASM_WHILE:
      write_assembly(assembly_file,"L%03d:\n", (lbl1 = label_counter));
      continue_to_label = lbl1;
      insert_label(PICLANG_LABEL,lbl1);
      label_counter++;
      ex(p->opr.op[0]);
      write_assembly(assembly_file,"\tjz\tL%03d\n", (lbl2 = label_counter));
      label_counter++;
      break_to_label = lbl2;
      insert_code(PICLANG_JZ);
      insert_code(lbl2);
      ex(p->opr.op[1]);
      write_assembly(assembly_file,"\tjmp\tL%03d\n", lbl1);
      insert_code(PICLANG_JMP);
      insert_code(lbl1);
      write_assembly(assembly_file,"L%03d:\n", lbl2);
      insert_label(PICLANG_LABEL,lbl2);
      break_to_label = previous_break_to_label;
      continue_to_label = previous_continue_to_label;
      break;
    case PASM_IF:
      ex(p->opr.op[0]);
      if (p->opr.nops > 2) {
	/* if else */
	write_assembly(assembly_file,"\tjz\tL%03d\n", (lbl1 = label_counter));
	label_counter++;
	insert_code(PICLANG_JZ);
	insert_code(lbl1);
	ex(p->opr.op[1]);
	write_assembly(assembly_file,"\tjmp\tL%03d\n", (lbl2 = label_counter));
	label_counter++;
	insert_code(PICLANG_JMP);
	insert_code(lbl2);
	write_assembly(assembly_file,"L%03d:\n", lbl1);
	insert_label(PICLANG_LABEL,lbl1);
	ex(p->opr.op[2]);
	write_assembly(assembly_file,"L%03d:\n", lbl2);
	insert_label(PICLANG_LABEL,lbl2);
      } else {
	/* if */
	write_assembly(assembly_file,"\tjz\tL%03d\n", (lbl1 = label_counter));
	label_counter++;
	insert_code(PICLANG_JZ);
	insert_code(lbl1);
	ex(p->opr.op[1]);
	write_assembly(assembly_file,"L%03d:\n", lbl1);
	insert_label(PICLANG_LABEL,lbl1);
      }
      break;
    case PICLANG_PRINT:     
      ex(p->opr.op[0]);
      write_assembly(assembly_file,"\tputd\n");insert_code(PICLANG_PRINTL);
      break;
    case PICLANG_ARGV:
      ex(p->opr.op[0]);
      write_assembly(assembly_file,"\targv\n");
      insert_code(PICLANG_ARGV);
      break;
    case PICLANG_ARGC:
      write_assembly(assembly_file,"\targc\n");
      insert_code(PICLANG_ARGC);
      break;
    case PICLANG_PRINTL:
      ex(p->opr.op[0]);
      write_assembly(assembly_file,"\tputch\n");insert_code(PICLANG_PRINT);
      break;
    case PICLANG_FPUTCH:
      ex(p->opr.op[0]);
      write_assembly(assembly_file,"\tfputch\n");insert_code(PICLANG_FPUTCH);
      break;
    case PICLANG_FPUTD:
      ex(p->opr.op[0]);
      write_assembly(assembly_file,"\tfputd\n");insert_code(PICLANG_FPUTD);
      break;
    case PICLANG_FOPEN:
      ex(p->opr.op[0]);
      write_assembly(assembly_file,"\tfopen\n");insert_code(PICLANG_FOPEN);
      break;
    case PICLANG_FCLOSE:
      ex(p->opr.op[0]);
      write_assembly(assembly_file,"\tfclose\n");insert_code(PICLANG_FCLOSE);
      break;
    case PICLANG_FREAD:
      ex(p->opr.op[0]);
      write_assembly(assembly_file,"\tfread\n");insert_code(PICLANG_FREAD);
      break;
    case PICLANG_DROP:
      write_assembly(assembly_file,"\tfdrop\n");
      insert_code(PICLANG_DROP);
      break;
    case PICLANG_SWAP:
      write_assembly(assembly_file,"\tfswap\n");
      insert_code(PICLANG_SWAP);
      break;
    case PICLANG_POP:
      if(p->opr.nops == 0)
	break;
      if(p->opr.nops > 1)
	ex(p->opr.op[1]);
      write_assembly(assembly_file,"\tpop\t%s\n", p->opr.op[0]->id.name);
      insert_code( PICLANG_POP);
      insert_code(resolve_variable(p->opr.op[0]->id.name));
      break;
    case PICLANG_UMINUS:
      ex(p->opr.op[0]);
      write_assembly(assembly_file,"\tneg\n");
      insert_code(PICLANG_UMINUS);
      break;
    case PICLANG_SYSTEM:
      {
	int op_counter = p->opr.nops - 1;
	for(;op_counter >= 0 ;op_counter--)
	  ex(p->opr.op[op_counter]);
	write_assembly(assembly_file,"\tsystem\n");insert_code(PICLANG_SYSTEM);
	break;
      }
    case PICLANG_SIGNAL:
      {
	struct subroutine_map *subroutine = NULL;
	if(p->opr.nops != 2)
	  {
	    fprintf(stderr,"Invalid number of arguments to signal()\nNeeded 2, got %d\n",p->opr.nops);
	    yyerror("Syntax error");
	  }
	subroutine = get_subroutine(p->opr.op[1]->str.string);
	if(subroutine == NULL)
	  {
	    fprintf(stderr,"Invalid subroutine: %s\n",p->opr.op[1]->str.string);
	    yyerror("Syntax error");
	  }
	write_assembly(assembly_file,"\tsignal %d, L%03d\n", p->opr.op[0]->con.value, subroutine->label);
	insert_code(PICLANG_SIGNAL);
	insert_code(p->opr.op[0]->con.value);
	insert_code(subroutine->label);
	break;
      }
    case PICLANG_SLEEP:
      ex(p->opr.op[0]);
      write_assembly(assembly_file,"\tsleep\n");
      insert_code(PICLANG_SLEEP);
      break;
    case PICLANG_SPRINT:
      ex(p->opr.op[0]);
      write_assembly(assembly_file,"\tsprint\n");
      insert_code(PICLANG_SPRINT);
      break;
    case PICLANG_MORSE:
      ex(p->opr.op[1]);
      ex(p->opr.op[0]);
      write_assembly(assembly_file,"\tmorse\n");
      insert_code(PICLANG_MORSE);
      break;
    case PICLANG_TIME:
      ex(p->opr.op[0]);
      write_assembly(assembly_file,"\ttime\n");
      insert_code(PICLANG_TIME);
      break;
    case PICLANG_FFLUSH:
      write_assembly(assembly_file,"\tfflush\n");
      insert_code(PICLANG_FFLUSH);
      break;
    case PICLANG_FCLEAR:
      write_assembly(assembly_file,"\tfclear\n");
      insert_code(PICLANG_FCLEAR);
      break;
    case PICLANG_CLEAR:
      write_assembly(assembly_file,"\tclear\n");
      insert_code(PICLANG_CLEAR);
      break;
    case PICLANG_SET_TIME:
      ex(p->opr.op[0]);
      ex(p->opr.op[1]);
      write_assembly(assembly_file,"\tsettime\n");
      insert_code(PICLANG_SET_TIME);
      break;
    case PICLANG_SET_DATE:
      ex(p->opr.op[0]);
      ex(p->opr.op[1]);
      ex(p->opr.op[2]);
      write_assembly(assembly_file,"\tsetdate\n");
      insert_code(PICLANG_SET_DATE);
      break;
    case PICLANG_GETD:
      write_assembly(assembly_file,"\tgetd\n");insert_code(PICLANG_GETD);
      break;
    case PICLANG_GETCH:
      write_assembly(assembly_file,"\tgetch\n");insert_code(PICLANG_GETCH);
      break;
    case PICLANG_NOT:
      ex(p->opr.op[0]);
      write_assembly(assembly_file,"\tnot\n");
      insert_code(PICLANG_NOT);
      break;
    default:// all piclang functions
      {
	ex(p->opr.op[0]);
	ex(p->opr.op[1]);
	switch(p->opr.oper) {
	case PICLANG_DEREF:// array access
	  write_assembly(assembly_file,"\tderef\n");
	  insert_code(PICLANG_DEREF);
	  break;
	case PICLANG_BSR:
	  write_assembly(assembly_file,"\tbsr \n");
	  insert_code(PICLANG_BSR);
	  break;
	case PICLANG_BSL:
	  write_assembly(assembly_file,"\tbsl \n");
	  insert_code(PICLANG_BSL);
	  break;
	case PICLANG_AND:
	  write_assembly(assembly_file,"\tand\n");
	  insert_code(PICLANG_AND);
	  break;
	case PICLANG_OR:
	  write_assembly(assembly_file,"\tor\n");
	  insert_code(PICLANG_OR);
	  break;
	case PICLANG_MOD:
	  write_assembly(assembly_file,"\tmod \n"); 
	  insert_code(PICLANG_MOD);
	  break;
	case PICLANG_ADD:   
	  write_assembly(assembly_file,"\tadd \n"); 
	  insert_code(PICLANG_ADD);
	  break;
	case PICLANG_SUB:   
	  write_assembly(assembly_file,"\tsub\n");
	  insert_code(PICLANG_SUB); 
	  break; 
	case PICLANG_MULT:   
	  write_assembly(assembly_file,"\tmul\n");
	  insert_code(PICLANG_MULT); 
	  break;
	case PICLANG_DIV:   
	  write_assembly(assembly_file,"\tdiv\n"); 
	  insert_code(PICLANG_DIV);
	  break;
	case PICLANG_COMPLT:   
	  write_assembly(assembly_file,"\tcompLT\n"); 
	  insert_code(PICLANG_COMPLT);
	  break;
	case PICLANG_COMPGT:   
	  write_assembly(assembly_file,"\tcompGT\n"); 
	  insert_code(PICLANG_COMPGT);
	  break;
	case PICLANG_COMPNE:    
	  write_assembly(assembly_file,"\tcompNE\n"); 
	  insert_code(PICLANG_COMPNE);
	  break;
	case PICLANG_COMPEQ:    
	  write_assembly(assembly_file,"\tcompEQ\n"); 
	  insert_code(PICLANG_COMPEQ);
	  break;
	case PASM_STATEMENT_DELIM:
	  break;
	default:
	  fprintf(stderr,"Unknown op code: %d\n",p->opr.oper);
	  exit(-1);
	  break;
	}//switch
      }//default
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

int count_variables()
{
  int retval = 0;
  const idNodeType *it = variable_list;
  while(it != NULL)
    {
      retval++;
      it = it->next;
    }
  return retval;
}

int resolve_variable(const char *name)
{
  int i;
  idNodeType *curr_variable = variable_list;
  if(name == NULL)
    {
      yyerror("Invalid variable name: NULL POINTER\n");
      return -1;
    }

  if(variable_list == NULL)
    {
      variable_list = (idNodeType*)malloc(sizeof(idNodeType));
      variable_list->i = 0;
      strcpy(variable_list->name,name);
      variable_list->next = NULL;
      return 0;
    }
  
  while(curr_variable != NULL)
    {
      if(strcmp(curr_variable->name,name) == 0)
	return curr_variable->i;
      curr_variable = curr_variable->next;
    }

  // At this point, the variable does not exist.
  curr_variable = (idNodeType*)malloc(sizeof(idNodeType));
  strcpy(curr_variable->name,name);
  curr_variable->i = count_variables();
  curr_variable->next = variable_list;
  variable_list = curr_variable;

  return curr_variable->i;
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

void set_pcb_type(struct compiled_code *the_pcb)
{
  if(the_pcb == NULL)
    return;
  the_pcb->type = typePCB;
  set_pcb_type(the_pcb->next);
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
  end_of_call_stack->next = NULL;// temporary to count PCB's size and set PCB code types
  set_pcb_type(size);
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
  /*  else
    {
      // In this case, there are no strings. So a single null-terminated character will indiate that.
      the_code->next = (struct compiled_code*)malloc(sizeof(struct compiled_code));
      the_code = the_code->next;
      the_code->next = NULL;
      the_code->val = 0;
      the_code->type = typeStr;

      }*/
  
  size->val =  CountCode(size);
  
  // Magic number to identify the executable
  i = PCB_MAGIC_NUMBER_OFFSET - 1;
  for(;i >= 0;i--)
    {
      magic_number = (struct compiled_code*)malloc(sizeof(struct compiled_code));
      magic_number->next = size;
      magic_number->val = PICLANG_magic_numbers[i];
      magic_number->type = typePCB;
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






