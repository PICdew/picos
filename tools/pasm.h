#ifndef PASM_H
#define PASM_H 1

#include "piclang.h"
#include "arg.h"
#include "utils.h"
#include "fs.h"

#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

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
  PASM_ADDR/*Address reference*/,
  PASM_INITIALIZATION/* Initial definition. This allows the compiler to differentiate the first definition of a variable and it's modification later.*/,
  PASM_SWITCH,
  PASM_CASE,
  PASM_DEFAULT
 };
  


// Node types
typedef enum { typeCon, typeId, typeOpr, typeStr, typeLabel, typeCode, typeSubroutine, typePCB, typePad, typePreproc} nodeEnum;
// typeLabel is for label for jumps
// typeCode is for compiled code

// Data types
enum { data_int};

// relocation types
enum {REL_STRING = 0,REL_VARIABLE,REL_LABEL};
typedef  struct {
    int addr;// address to be offset
    int val;// relocation amount. This may be an offset or new value, depending on the relocation type.
    int type;// type of offset. See enum above
  }relocation_t;

/* constants */
typedef struct {
    int value;                  /* value of constant */
    int relocation_type; 	/* If constant is relocatable, this is the enum value. Otherwise, -1 */
} conNodeType;

/* identifiers */
typedef struct idNodeType_NODE {
  int i;// index in page memory (zero indexed), when viewed as flat memory
  char name[FILENAME_MAX];// variable name used in program
  int type;// Type of variable. See data types enum above
  bool constant;
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

struct subroutine_map
{
  char name[FILENAME_MAX];// Human-readable name.
  int address, size;
  struct compiled_code *code, *code_end, *strings, *strings_end;
  idNodeType *variables;
  struct subroutine_map *next;
};

struct compiled_code
{
  picos_size_t label;
  picos_size_t val;
  nodeEnum type;
  int relocation_type;
  void *target;
  struct compiled_code *next;
};

struct relocation_map
{
  relocation_t relocation;
  struct relocation_map *next;
};

struct piclib_object
{
  struct compiled_code *code;
  struct compiled_code *strings;
  struct subroutine_map *subroutines;
  struct relocation_map *relmap;
  int offset;
  char filename[FILENAME_MAX];
  struct piclib_object *next;
};



struct assembly_map
{
  const char keyword[256];
  int opcode, has_arg;
};

extern const char PICLANG_LIB_MAGIC_NUMBERS[];

nodeType *opr(int oper, int nops, ...);
nodeType *id(idNodeType var);// creates a non-constant variable
nodeType *const_id(idNodeType var, bool is_const);// creates a variable which may be made either constant or non-constant
nodeType *full_id(idNodeType var, bool is_const, int data_type);// data_type: see enum above.
nodeType *con(int value);// Creates a constant that is not relocatable
nodeType *full_con(int value, int relocation_type);

/**
 * Exits the program with status 1 and prints the supplied message to stderr
 */
void reason_exit(const char *format, ...);

struct assembly_map* keyword2assembly(const char *keyword);
struct assembly_map* opcode2assembly(int opcode);

struct compiled_code* insert_compiled_code(nodeEnum type, struct subroutine_map *subroutine, picos_size_t val, picos_size_t label);
#define insert_string(X) insert_compiled_code(typeStr, g_curr_subroutine,X,0)
#define insert_code(X) insert_compiled_code(typeCode, g_curr_subroutine,X,0)
#define insert_label(X,Y) insert_compiled_code(typeLabel, g_curr_subroutine,X,Y)

/**
  * Frees code's memory. Does not do so recursively through the list.
  * See, free_all_code
  */
void free_code(struct compiled_code *code);

/**
  * Frees code memory, recursively through the linked list.
  *
  */
void free_all_code(struct compiled_code *code_list);

/**
  * Creates and inserts a subroutine into the global subroutine linked list
  * Returns a pointer to the subroutine. If the subroutine already exists,
  * and error is thrown
  */
struct subroutine_map* insert_subroutine(const char *name);

/**
  * Frees a subroutine's memory. Will recursively free all components of the 
  * struct, but will not free it's "next" component. To free recursively,
  * iterate through linked list.
  */
void free_subroutine(struct subroutine_map *subroutine);

/**
  * Recursively frees a subroutine linked list. Calls free_subroutine.
  */
void all_free_subroutines(struct subroutine_map *subroutine);

 void free_all_variables(idNodeType *variable);
 
struct subroutine_map* get_subroutine(const char *name);


void _attach_label(struct compiled_code *ptrlist_end, picos_size_t label);
extern int write_assembly(FILE *stream, const char *format, ...);

int lookup_label(const struct compiled_code* code, picos_size_t label);

enum PRINT_TYPE{PRINT_HEX, PRINT_EEPROM_DATA};
size_t CountCode(const struct compiled_code *the_code);
void FreeCode(struct compiled_code* code);
struct compiled_code* MakePCB(struct subroutine_map *subroutines, int total_memory, picos_size_t piclang_bitmap);
void FirstPass(struct compiled_code* code,int skip_assignment_check, unsigned char *piclang_bitmap,  int num_variables);
void FPrintCode(FILE *hex_file,struct compiled_code* code, int col, char *buffer,int start_address, int checksum, int print_type);
#define COMPILE_MAX_WIDTH 8//max width

/**
 * Compiles code, but does not link.
 *
 */
void pasm_compile(FILE *eeprom_file,FILE *hex_file,struct subroutine_map *the_subroutines, picos_size_t *piclang_bitmap);

/**
 * Compiles and links the code.
 */
void pasm_build(FILE *eeprom_file,FILE *hex_file,struct subroutine_map *the_subroutines, picos_size_t *piclang_bitmap);

/**
 * Loads a library file and creates a library struct
 */
struct piclib_object* piclib_load(FILE *libfile);

/**
 * Links a library object to the current code base
 */
int piclib_link(struct piclib_object *library, struct subroutine_map *subroutines);

/**
 * writes a piclang library object
 */
void write_piclib_obj(FILE *binary_file,const struct subroutine_map *subroutines);

void create_lst_file(FILE *lst_file, const struct subroutine_map *subroutines);
void create_lnk_file(FILE *lnk_file, const struct subroutine_map *subroutines);

#endif

