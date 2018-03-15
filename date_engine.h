/*
 * File: date_engine.cpp
 * -------------------------------------------------------------------------------------------------
 *
 */

#ifndef Date_Engine_Included
#define Date_Engine_Included

#include "generics.h"

/*
 */
int set_initial_month(std::string month_name);

/*
 */
int days_in_month(int month);

/*
 */
std::string get_updated_date(Date& date, Temporal_Values values);

#endif
