#include "iec_std_lib.h"

//Booleans
IEC_BOOL *bool_input[BUFFER_SIZE][8];
IEC_BOOL *bool_output[BUFFER_SIZE][8];

//Bytes
IEC_BYTE *byte_input[BUFFER_SIZE];
IEC_BYTE *byte_output[BUFFER_SIZE];

//Analog I/O
IEC_UINT *int_input[BUFFER_SIZE];
IEC_UINT *int_output[BUFFER_SIZE];

//Memory
IEC_UINT *int_memory[BUFFER_SIZE];
IEC_DINT *dint_memory[BUFFER_SIZE];
IEC_LINT *lint_memory[BUFFER_SIZE];

//Special Functions
IEC_LINT *special_functions[BUFFER_SIZE];

IEC_TIME __CURRENT_TIME;

#define __LOCATED_VAR(type, name, ...) type __##name;
#include "LOCATED_VARIABLES.h"
#undef __LOCATED_VAR
#define __LOCATED_VAR(type, name, ...) type *name = &__##name;
#include "LOCATED_VARIABLES.h"
#undef __LOCATED_VAR

void glue_vars()
{
#define POOL_BOOL_I bool_input
#define POOL_BOOL_Q bool_output
#define POOL_UINT_I int_input
#define INDEX_BOOL(a, b) [a][b]
#define INDEX_UINT(a, b) [a]

#define __LOCATED_VAR(type, name, inout, type_sym, a, b) POOL_##type##_##inout INDEX_##type(a, b) = name;
#include "LOCATED_VARIABLES.h"
#undef __LOCATED_VAR
}
