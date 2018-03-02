/*
 * File: math_engine.h
 * -------------------------------------------------------------------------------------------------
 */

#ifndef Math_Engine_Included
#define Math_Engine_Included

#include "generics.h"
#include "vector.h"

/*
 */
General_Cash_Flow_Data evaluate_cash_flow(Vector<Home> schedule, Temporal_Values values);

#endif
