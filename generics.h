/*
 * File: generics.h
 * -------------------------------------------------------------------------------------------------
 */

#ifndef Generics_Included
#define Generics_Included

#include <string>
#include "set.h"
#include "vector.h"

extern const int MONTHS_TO_COMPLETE_HOUSE;
extern const int WEEKS_TO_COMPLETE_HOUSE;

extern const int MONTHS_PER_YEAR;
extern const int DAYS_PER_WEEK;

extern const int SENTINEL;
extern const int STREAM_PRECISION;
extern const double ROUNDING_BENCHMRK;
extern const int ZERO;

extern double money_market_amount;
extern double checking_account_amount;
extern double other_sources_amount;

/*
 */
struct Local_Temporal_Unit {

    std::string temporal_unit_name;
    std::string temporal_unit_name_relative_to_home;

    double cash_in;
    double cash_out_total;
    double cash_out_this_temporal_unit;

    bool land_bought_this_unit;
    bool loan_dispersement_made;
    bool under_construction;
    bool delayed;
    bool finished_this_unit;
    bool sold_this_unit;

};

/*
 */
struct Individual_Cash_Flow_Data {

    double cash_out_initially;
    double cumulative_cash_out_total;
    Vector<Local_Temporal_Unit> schedule;

};

/*
 */
struct Home {

    std::string address;

    double total_cost;
    double cost_of_land;
    double cost_of_construction;

    Set<int> loan_draw_schedule;
    int number_of_loan_draws;

    int temporal_units_to_land_payment;
    int temporal_units_to_end;
    int temporal_units_delayed;
    int temporal_units_to_sale;

    // Only used in math engine to store data about home's cash flow
    Individual_Cash_Flow_Data cash_flow_data;

};

/*
 */
struct Temporal_Unit {

    std::string name;
    double cash_on_hand;
    double cash_out_total;
    double cash_out_this_temporal_unit;
    double cash_in;

    Vector<Home> houses_land_is_bought_for;
    Vector<Home> houses_loan_dispersements_made_for;
    Vector<Home> houses_under_construction;
    Vector<Home> houses_delayed;
    Vector<Home> houses_finished;
    Vector<Home> houses_sold;

};

/*
 */
struct General_Cash_Flow_Data {

    double cash_out_initially;
    double cash_initially_in_acounts;
    double total_cash_available_for_building;

    Vector<Temporal_Unit> schedule;
    Vector<Home> individual_home_data;

};

/*
 */
struct Temporal_Values {

    std::string unit_type;
    std::string unit_type_plural;
    std::string unit_type_caps;
    std::string unit_type_caps_plural;
    std::string unit_type_all_caps;
    std::string unit_type_all_caps_plural;
    std::string unit_adverb;
    int units_to_complete_house;

};

/*
 */
struct Date {

    int month_index;
    int week_index;
    int specification_index;

};

extern Temporal_Values values;
extern Date initial_date;

extern General_Cash_Flow_Data general_cash_flow_data;

/*
 */
int getIntegerAugment(std::string prompt);

/*
 */
double getRealAugment(std::string prompt);

/*
 */
int temporal_units_under_construction(Home house, Temporal_Values values);

/*
 */
bool loan_draw_schedule_contains(Home home, int num);

/*
 */
bool is_using_loan_draws(Home& home);

/*
 */
bool land_is_to_be_bought(Home home);

/*
 */
bool land_is_to_be_bought_this_temporal_unit(Home home);

/*
 */
bool is_to_be_built(Home home, Temporal_Values values);

/*
 */
bool is_delayed(Home home);

/*
 */
bool is_under_construction(Home home, Temporal_Values values);

/*
 */
bool is_finished(Home home);

/*
 */
bool is_sold_this_temporal_unit(Home home);

/*
 */
bool is_already_sold(Home home);

#endif
