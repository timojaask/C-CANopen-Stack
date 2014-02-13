All the variables and functions in this project are explicitly scoped. Meaning that in every declaration there is either "extern" or "static" keyword, which defines the scope of the variable or function.
The coding style is also very consistent.
 - All the externally accessible functions or variables must be declared in a .h file as extern.
 - All the externally accessible functions or variables must be prefixed with a name of a file that they belong to. (exception: od_initialize.h)
 - All the privat functions must be declared near the beginning of a .c file before any function definition.
 - In .h files, the order of items is always:
   + Global definitions (#define, type definitions)
   + Global variables
   + Global prototypes
 - In .c files, the order of items is always:
   + Local definitions
   + Local variables
   + Local prototypes
   + Global functions
   + Local functions
 - Variables should be defined with the smallest possible scope.
 - Functions should only be declared external if they are used outside of the file that they belong to, otherwise declare as static.
 - There can only be one return statement per function.
 - All function and variable names are all lower case and underscore ('_') is used as word separator.
 - Defined constants are all upper case.