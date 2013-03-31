/*Global include file for kdv project, keep include lines outside of indivual
 *files also some shameless gcc only macros and such
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define lambda(return_type, function_body) \
({ \
      return_type __fn__ function_body \
          __fn__; \
})
#define for_each_array(fe_arrType, fe_arr, fe_fn_body)  \
  {                                                     \
    int i=0;                                            \
    for(;i<sizeof(fe_arr)/sizeof(fe_arrType);i++){      \
      fe_arr[i] = fe_fn_body(&fe_arr[i]); }             \
    }
#define unless(conditional,body)                    \
  if (!conditional)                                 \
    { body }                                        \

#define HERE fprintf(stderr, "HERE at %s:%d (%s)\n", __FILE__, __LINE__, __FUNCTION__)

#define malloc_check (ptr) {\
  if(ptr == NULL){          \
  fprintf(stderr,"Memory allocation failed\n")\
    exit(EXIT_FAILURE)}
