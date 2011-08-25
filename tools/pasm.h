#include <limits.h>

typedef enum { typeCon, typeId, typeOpr, typeStr } nodeEnum;

/* constants */
typedef struct {
    int value;                  /* value of constant */
} conNodeType;

/* identifiers */
typedef struct {
    int i;                      /* subscript to sym array */
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
  unsigned char label;
  unsigned char val;
  struct compiled_code *next;
};

void insert_compiled_code(struct compiled_code** ptrlist, struct compiled_code** ptrlist_end, unsigned char val);
void _attach_label(struct compiled_code *ptrlist_end, unsigned char label);
extern int write_assembly(FILE *stream, const char *format, ...);

enum PRINT_TYPE{PRINT_HEX, PRINT_EEPROM_DATA};
size_t CountCode(struct compiled_code *the_code);
void FreeCode(struct compiled_code* code);
struct compiled_code* MakePCB(struct compiled_code *the_code, struct compiled_code *the_strings, int total_memory, unsigned char piclang_bitmap);
void FirstPass(struct compiled_code* code,int skip_assignment_check, unsigned char *piclang_bitmap,  int num_variables);
void FPrintCode(FILE *hex_file,struct compiled_code* code, int col, char *buffer,int start_address, int checksum, int print_type);
#define COMPILE_MAX_WIDTH 8//max width
void pasm_compile(FILE *eeprom_file,FILE *hex_file,struct compiled_code **the_code, struct compiled_code *the_strings, unsigned char *piclang_bitmap, int num_variables);

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

