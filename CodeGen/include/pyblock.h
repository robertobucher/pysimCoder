#define INIT  1
#define OUT   2
#define STUPD 3
#define END   4

typedef struct {
  int nin;             /* Number of inputs */
  int nout;            /* Number of outputs */
  int *nx;             /* Cont. and Discr states */
  void **u;            /* inputs */
  void **y;            /* outputs */
  double *realPar;     /* Real parameters */
  int *intPar;         /* Int parameters */
  char * str;          /* String */
  void * ptrPar;       /* Generic pointer */
}python_block;
