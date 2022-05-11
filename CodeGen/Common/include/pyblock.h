#ifndef PYBLOCK_H
#define PYBLOCK_H

#define CG_INIT  1
#define CG_OUT   2
#define CG_STUPD 3
#define CG_END   4

typedef struct {
  int nin;             /* Number of inputs */
  int nout;            /* Number of outputs */
  int * dimIn;         /* Port signal dimension */
  int * dimOut;        /* Port signal dimension */
  int *nx;             /* Cont. and Discr states */
  void **u;            /* inputs */
  void **y;            /* outputs */
  double *realPar;     /* Real parameters */
  int realParNum;      /* Number of real parameters */
  int *intPar;         /* Int parameters */
  int intParNum;       /* Number of int parameters */
  char * str;          /* String */
  void * ptrPar;       /* Generic pointer */
  char **realParNames; /* Names of real parameters */
  char **intParNames;  /* Names of integer parameter */
} python_block;

/* SHV related structures */

typedef struct {
  const char * block_name;  /* Name of the block */
  int block_idx;            /* Index in python_block structure */
  int system_inputs;        /* Block has system inputs */
  int system_outputs;       /* Block has system outputs */
} python_block_name_entry;

typedef struct {
  const python_block_name_entry * blocks;   /* Pointer to python_block_name_entry structure */
  const python_block * block_structure;     /* Pointer to python_block structure */
  int blocks_count;                         /* Number of blocks */
} python_block_name_map;

#endif /* PYBLOCK_H */
