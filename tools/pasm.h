#ifndef PASM_H
#define PASM_H 1

#include "piclang.h"
#include "arg.h"
#include "utils.h"
#include "fs.h"

#include <stdio.h>
#include <limits.h>

enum LANGUAGE_TOKENS{
  PASM_WHILE = PICLANG_NUM_COMMANDS,
  PASM_BREAK,
  PASM_CONTINUE,
  PASM_IF,
  PASM_CALL,
  PASM_SUBROUTINE,
  PASM_STRING,
  PASM_DEFINE,
  PASM_EXIT,
  PASM_PASM_CR,
  PASM_PASM_POP,
  PASM_ARGV,
  PASM_ARGC,
  PASM_FIN,
  PASM_FEOF,
  PASM_STATEMENT_DELIM,
  PASM_LABEL/* Tags an address as being referenced by a label*/,
  PASM_ADDR/*Address reference*/
 };
  


typedef enum { typeCon, typeId, typeOpr, typeStr, typeLabel, typeCode, typeSubroutine, typePCB, typePreproc} nodeEnum;
// typeLabel is for label for jumps
// typeCode is for compiled code

/* constants */
typedef struct {
    int value;                  /* value of constant */
} conNodeType;

/* identifiers */
typedef struct idNodeType_NODE {
  int i;// index in page memory (zero indexed), when viewed as flat memory
  char name[FILENAME_MAX];// variable name used in program
  struct idNodeType_NODE *next;
} idNodeType;

/* operators */
typedef struct {
    int oper;                   /* operator */
    int nops;                   /* number of operands */
    struct nodeTypeTag *op[1];  /* operands (expandable) */
} oprNodeType;

/* system calls */
typedef struct{
  char string[FILENAME_MAX];  
} strNodeType;

typedef struct nodeTypeTag {
    nodeEnum type;              /* type of node */

    /* union must be last entry in nodeType */
    /* because operNodeType may dynamically increase */
    union {
        conNodeType con;        /* constants */
        idNodeType id;          /* identifiers */
        oprNodeType opr;        /* operators */
      strNodeType str;

    };
} nodeType;

extern int sym[26];

struct compiled_code
{
  picos_size_t label;
  picos_size_t val;
  nodeEnum type;
  struct compiled_code *next;
};

struct subroutine_map
{
  char name[FILENAME_MAX];// Human-readable name.
  size_t index;// Unique for each subroutine
  size_t label;// Links subroutines to code address.
  struct subroutine_map *next;
};

struct assembly_map
{
  const char keyword[256];
  int opcode, has_arg;
};

nodeType *opr(int oper, int nops, ...);
nodeType *id(idNodeType var);
nodeType *con(int value);

struct assembly_map* keyword2assembly(const char *keyword);
struct assembly_map* opcode2assembly(int opcode);

void insert_compiled_code(nodeEnum type, struct compiled_code** ptrlist, struct compiled_code** ptrlist_end, picos_size_t val, picos_size_t label);
#define insert_string(X) insert_compiled_code(typeStr, &the_strings,&the_strings_end,X,0)
#define insert_code(X) insert_compiled_code(typeCode, &the_code,&the_code_end,X,0)
#define insert_label(X,Y) insert_compiled_code(typeLabel, &the_code,&the_code_end,X,Y)

void insert_subroutine(const char *name, size_t label);
const struct subroutine_map* get_subroutine(const char *name);


void _attach_label(struct compiled_code *ptrlist_end, picos_size_t label);
extern int write_assembly(FILE *stream, const char *format, ...);

int lookup_label(const struct compiled_code* code, picos_size_t label);

enum PRINT_TYPE{PRINT_HEX, PRINT_EEPROM_DATA};
size_t CountCode(struct compiled_code *the_code);
void FreeCode(struct compiled_code* code);
struct compiled_code* MakePCB(struct compiled_code *the_code, struct compiled_code *the_strings, int total_memory, picos_size_t piclang_bitmap);
void FirstPass(struct compiled_code* code,int skip_assignment_check, unsigned char *piclang_bitmap,  int num_variables);
void FPrintCode(FILE *hex_file,struct compiled_code* code, int col, char *buffer,int start_address, int checksum, int print_type);
#define COMPILE_MAX_WIDTH 8//max width
void pasm_compile(FILE *eeprom_file,FILE *hex_file,struct compiled_code **the_code, struct compiled_code *the_strings, picos_size_t *piclang_bitmap, int num_variables);

void create_lst_file(FILE *lst_file, const struct compiled_code *code_to_lst, const struct compiled_code *strings_to_list);
void create_lnk_file(FILE *lnk_file, const struct compiled_code *code_to_lst);

#endif

