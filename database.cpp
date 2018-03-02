/*
 * File: database.cpp
 * -------------------------------------------------------------------------------------------------
 */

#include "database.h"
#include "generics.h"
#include <string>
#include "simpio.h"
#include "strlib.h"
#include "goptionpane.h"
#include "graphics_engine.h"

const int WEEKS_PER_MONTH = 4;

Vector<Home> database;

/*
 */
void convert_months_to_weeks() {

    for (Home& home: database) {

        Set<int> new_loan_draw_schedule;

        for (int draw: home.loan_draw_schedule) {
            new_loan_draw_schedule += draw * WEEKS_PER_MONTH;
        }

        home.loan_draw_schedule = new_loan_draw_schedule;
        home.number_of_loan_draws = home.loan_draw_schedule.size();

        home.temporal_units_delayed = (home.temporal_units_delayed * WEEKS_PER_MONTH);
        home.temporal_units_to_end = (home.temporal_units_to_end * WEEKS_PER_MONTH);
        home.temporal_units_to_land_payment = (home.temporal_units_to_land_payment *
                WEEKS_PER_MONTH);
        home.temporal_units_to_sale = (home.temporal_units_to_sale * WEEKS_PER_MONTH);
    }
}

/*
 */
void convert_weeks_to_months() {

    for (Home& home: database) {

        Set<int> new_loan_draw_schedule;
        for (int draw: home.loan_draw_schedule) {
            new_loan_draw_schedule += ((draw / WEEKS_PER_MONTH) == ZERO ? - SENTINEL : (draw /
                    WEEKS_PER_MONTH));
        }

        home.loan_draw_schedule = new_loan_draw_schedule;
        home.number_of_loan_draws = home.loan_draw_schedule.size();

        home.temporal_units_delayed = (is_delayed(home) ?
                ((home.temporal_units_delayed / WEEKS_PER_MONTH) == ZERO ? -SENTINEL :
                (home.temporal_units_delayed / WEEKS_PER_MONTH)) : ZERO);

        home.temporal_units_to_end = home.temporal_units_to_end / WEEKS_PER_MONTH;
        home.temporal_units_to_land_payment = (land_is_to_be_bought(home) ?
                ((home.temporal_units_to_land_payment / WEEKS_PER_MONTH) == ZERO ? -SENTINEL :
                (home.temporal_units_to_land_payment / WEEKS_PER_MONTH)) : ZERO);

        home.temporal_units_to_sale = home.temporal_units_to_sale / WEEKS_PER_MONTH;
    }
}

/*
 */
void transition_database() {

    if (values.unit_type == "week") {
        convert_months_to_weeks();
    } else {
        convert_weeks_to_months();
    }
}

/*
 */
void sort_database() {

    for (int i = 0; i < database.size() - 1; i++) {

        for (int j = i; (j >= 0) && (database[j + 1].temporal_units_to_sale <
                database[j].temporal_units_to_sale); j--) {

            Home placeholder = database[j];
            database[j] = database[j + 1];
            database[j + 1] = placeholder;

        }

    }
}

/*
 */
bool database_contains_address(std::string address) {

    for (Home home: database) {
        if (home.address == address) return true;
    }

    return false;
}

/*
 */
Home get_home(std::string address) {

    for (Home home: database) {
        if (home.address == address) return home;
    }

    Home home;
    return home;
}

/*
 */
bool remove_home(std::string address) {

    for (int i = 0; i < database.size(); i++) {
        if (database[i].address == address) {
            database.remove(i);
            return true;
        }
    }

    return false;
}

/*
 */
bool validate_under_construction_data(Home home, bool is_delayed) {

    if (!(home.temporal_units_to_end >= home.temporal_units_delayed)) {
        GOptionPane::showMessageDialog("Invalid Entry: \"" + values.unit_type_caps_plural +
                " Until Home Is Finished\" cannot be less than \"" +
                values.unit_type_caps_plural + " Delayed\".");
        return false;
    }

    if (!(home.temporal_units_to_end <= (values.units_to_complete_house +
            home.temporal_units_delayed))) {
        GOptionPane::showMessageDialog("Invalid Entry: \"" + values.unit_type_caps_plural +
                " Until Home Is Finished\" or \"" + values.unit_type_caps_plural + " Delayed\".");
        return false;
    }

    if (!(home.temporal_units_to_end > ZERO)) {
        GOptionPane::showMessageDialog("Invalid Entry: \"" + values.unit_type_caps_plural +
                " Until Home Is Finished\" cannot be zero.");
        return false;
    }

    if (is_delayed && !(home.temporal_units_delayed > ZERO)) {
        GOptionPane::showMessageDialog("Invalid Entry: \"" + values.unit_type_caps_plural +
                " Delayed\" cannot be zero.");
        return false;
    }

    return true;
}

/*
 */
bool validate_to_be_built_data(Home home, bool is_yet_to_buy_land) {

    if (!(home.temporal_units_to_end > values.units_to_complete_house)) {
        GOptionPane::showMessageDialog("Invalid Entry: \"" + values.unit_type_caps_plural +
                " Until Home Is Started\" cannot be zero.");
        return false;
    }

    if (is_yet_to_buy_land && !(home.temporal_units_to_land_payment > ZERO)) {
        GOptionPane::showMessageDialog("Invalid Entry: \"" + values.unit_type_caps_plural +
                " Until Lot Is Bought\" cannot be zero.");
        return false;
    }

    if (!(home.temporal_units_to_land_payment <= home.temporal_units_to_end -
            values.units_to_complete_house)) {
        GOptionPane::showMessageDialog("Invalid Entry: \"" + values.unit_type_caps_plural +
                " Until Lot Is Bought\" must be less than \"" + values.unit_type_caps_plural +
                " Until Home Is Started\".");
        return false;
    }

    return true;
}

/*
 */
bool validate_entry(Home home, bool is_under_construction, bool is_delayed, bool is_yet_to_buy_land,
        bool is_using_loan_draws) {

    if ((home.cost_of_construction < ZERO) || (home.cost_of_land < ZERO) ||
            (home.temporal_units_delayed < ZERO) || (home.temporal_units_to_end < ZERO) ||
            (home.temporal_units_to_land_payment < ZERO) || (home.temporal_units_to_sale < ZERO)) {
        GOptionPane::showMessageDialog("Illegal Numerical Format: Less than zero error.");
        return false;
    }

    if (home.address == "") {
        GOptionPane::showMessageDialog("Illegal String Format: Enter a valid address.");
        return false;
    }

    if (is_under_construction) {
        if (!validate_under_construction_data(home, is_delayed)) return false;
    } else {
        if (!validate_to_be_built_data(home, is_yet_to_buy_land)) return false;
    }

    if (!(home.temporal_units_to_sale >= home.temporal_units_to_end)) {
        GOptionPane::showMessageDialog("Invalid Entry: \"" + values.unit_type_caps_plural +
                " Until Home Is Sold\" must be greater than or equal to the number of " +
                values.unit_type_plural + " until construction is ended.");
        return false;
    }

    if (is_using_loan_draws && !(home.loan_draw_schedule.size() > ZERO)) {
        GOptionPane::showMessageDialog("Illegal Format: Must select at least one dispersement.");
        return false;
    }

    return true;
}

/*
 */
void print_home_data(Home home) {

    std::cout << "Address: " << home.address << std::endl;

    std::cout << "Cost of Land: " << home.cost_of_land << std::endl;
    std::cout << "Cost of Construction: " << home.cost_of_construction << std::endl;
    std::cout << "Total Cost: " << home.total_cost << std::endl;

    std::cout << "Units to End: " << home.temporal_units_to_end << std::endl;
    std::cout << "Units to Sale: " << home.temporal_units_to_sale << std::endl;
    std::cout << "Units Delayed: " << home.temporal_units_delayed << std::endl;
    std::cout << "Units to Land Payment: " << home.temporal_units_to_land_payment << std::endl;

    std::cout << "Number of Loan Draws: " << home.number_of_loan_draws << std::endl;
    std::cout << "Loan Draw Schedule: " << home.loan_draw_schedule.toString() << std::endl <<
            std::endl;
}

/*
 */
void print_database() {

    std::cout << "DATABASE: " << std::endl;
    for (Home home: database) {
        print_home_data(home);
    }

    std::cout << std::endl;
}

/*
 */
bool augment_database(Home home, std::string address, bool check) {

    if (!check) return check;

    // Only unique addresses can be stored in the database
    remove_home(address);
    database += home;
    sort_database();

    return true;
}

/*
 */
bool edit_home(std::string address) {

    bool check = true;
    Home home = get_home(address);
    alter_data(home, check);

    return augment_database(home, address, check);
}

/*
 */
bool add_home() {

    bool check = true;
    Home new_home = create_home(check);

    return augment_database(new_home, new_home.address, check);
}

