/*
 * File: generics.cpp
 * -------------------------------------------------------------------------------------------------
 */

#include "generics.h"
#include <iostream>
#include <string>
#include "simpio.h"
#include "goptionpane.h"

const int MONTHS_TO_COMPLETE_HOUSE = 5;
const int WEEKS_TO_COMPLETE_HOUSE = 20;

const int MONTHS_PER_YEAR = 12;
const int DAYS_PER_WEEK = 7;

const int SENTINEL = -1;
const int STREAM_PRECISION = 2;
const double ROUNDING_BENCHMRK = .01;
const int ZERO = 0;

double money_market_amount;
double checking_account_amount;
double other_sources_amount;

Temporal_Values values;
Date initial_date;

General_Cash_Flow_Data general_cash_flow_data;

/*
 */
int getIntegerAugment(std::string entry) {

    try {
        int i = stoi(entry);
        if (i >= 0) return i;
    } catch (...) {}

    GOptionPane::showMessageDialog("Illegal format. Try again.");

    return SENTINEL;
}

/*
 */
double getRealAugment(std::string entry) {

    try {
        int i = stod(entry);
        if (i >= 0) return i;
    } catch (...) {}

    GOptionPane::showMessageDialog("Illegal format. Try again.");

    return SENTINEL;
}

/*
 */
int temporal_units_under_construction(Home house, Temporal_Values values) {
    return values.units_to_complete_house - (house.temporal_units_to_end -
            house.temporal_units_delayed);
}

/*
 */
bool loan_draw_schedule_contains(Home home, int num) {
    return home.loan_draw_schedule.contains(num);
}

/*
 */
bool is_using_loan_draws(Home& home) {
    return !home.loan_draw_schedule.isEmpty();
}

/*
 */
bool land_is_to_be_bought(Home home) {
    return home.temporal_units_to_land_payment > ZERO;
}

/*
 */
bool land_is_to_be_bought_this_temporal_unit(Home home) {
    return home.temporal_units_to_land_payment == ZERO;
}

/*
 */
bool is_to_be_built(Home home, Temporal_Values values) {
    return home.temporal_units_to_end > values.units_to_complete_house;
}

/*
 */
bool is_delayed(Home home) {
    return home.temporal_units_delayed != ZERO;
}

/*
 */
bool is_under_construction(Home home, Temporal_Values values) {
    return ((home.temporal_units_to_end <= (values.units_to_complete_house +
            home.temporal_units_delayed)) && (home.temporal_units_to_end > ZERO) &&
            (home.temporal_units_to_end >= home.temporal_units_delayed));
}

/*
 */
bool is_finished(Home home) {
    return home.temporal_units_to_end == ZERO;
}

/*
 */
bool is_sold_this_temporal_unit(Home home) {
    return home.temporal_units_to_sale == ZERO;
}

/*
 */
bool is_already_sold(Home home) {
    return home.temporal_units_to_sale <= ZERO;
}
