#define CG_INIT  1
#define CG_OUT   2
#define CG_STUPD 3
#define CG_END   4

typedef struct {
  int nin;             /* Number of inputs */
  int nout;            /* Number of outputs */
  int * dimIn;       /* Port signal dimension */
  int * dimOut;      /* Port signal dimension */
  int *nx;             /* Cont. and Discr states */
  void **u;            /* inputs */
  void **y;            /* outputs */
  double *realPar;     /* Real parameters */
  int *intPar;         /* Int parameters */
  char * str;          /* String */
  void * ptrPar;       /* Generic pointer */
}python_block;
