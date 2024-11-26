/* Description:Macros useful for development.
 * Language:C++
 * Author:***
 *
 */

#ifndef LIB_MACROS
#define LIB_MACROS

#include "lGeneral.hpp"

#define SIZEOF(type_name) sizeof(type_name)
#define STRUCTOFFSET(struct_name, struct_member) (UBINT)(&(((struct_name *)NULL)->struct_member))

#endif