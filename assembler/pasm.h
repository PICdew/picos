#include <limits.h>

typedef enum { typeCon, typeId, typeOpr/*, typeSys*/ } nodeEnum;

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

/* system calls 
typedef struct{
  char name[FILENAME_MAX];  
} sysNodeType;*/

typedef struct nodeTypeTag {
    nodeEnum type;              /* type of node */

    /* union must be last entry in nodeType */
    /* because operNodeType may dynamically increase */
    union {
        conNodeType con;        /* constants */
        idNodeType id;          /* identifiers */
        oprNodeType opr;        /* operators */
    };
} nodeType;

extern int sym[26];

struct compiled_code
{
  unsigned char label;
  unsigned char val;
  struct compiled_code *next;
};

struct compiled_code *the_code;
struct compiled_code *the_code_end;
extern void insert_code(unsigned char val);
