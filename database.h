/*
 * File: database.h
 * -------------------------------------------------------------------------------------------------
 */

#ifndef Database_Included
#define Database_Included

#include "generics.h"

extern Vector<Home> database;

/*
 */
void transition_database();

/*
 */
bool database_contains_address(std::string address);

/*
 */
Home get_home(std::string address);

/*
 */
bool remove_home(std::string address);

/*
 */
bool validate_entry(Home home, bool is_under_construction, bool is_delayed, bool is_yet_to_buy_land,
        bool is_using_loan_draws);

/*
 */
bool edit_home(std::string address);

/*
 */
bool add_home();

#endif
