/*
 * File: graphical_analyzer.cpp
 * -------------------------------------------------------------------------------------------------
 * Author/Developer: Evan Sheehan
 *
 */

#include <iostream>
#include <string>
#include "console.h"
#include "simpio.h"
#include "vector.h"
#include "generics.h"
#include "database.h"
#include "date_engine.h"
#include "math_engine.h"
#include "graphics_engine.h"
#include "gevents.h"
#include "goptionpane.h"

/*
 */
void choose_temporal_structure() {

    std::string timetable = toLowerCase(value_chooser->getSelectedItem());

    if (timetable == "weekly" || timetable == "week") {

        values.unit_type = "week";
        values.unit_type_plural = "weeks";
        values.unit_type_caps = "Week";
        values.unit_type_caps_plural = "Weeks";
        values.unit_type_all_caps = "WEEK";
        values.unit_type_all_caps_plural = "WEEKS";
        values.unit_adverb = "Weekly";
        values.units_to_complete_house = WEEKS_TO_COMPLETE_HOUSE;

    } else if (timetable == "monthly" || timetable == "month") {

        values.unit_type = "month";
        values.unit_type_plural = "months";
        values.unit_type_caps = "Month";
        values.unit_type_caps_plural = "Months";
        values.unit_type_all_caps = "MONTH";
        values.unit_type_all_caps_plural = "MONTHS";
        values.unit_adverb = "Monthly";
        values.units_to_complete_house = MONTHS_TO_COMPLETE_HOUSE;

    }
}

/*
 */
void init_general_cash_flow_data() {

    general_cash_flow_data.cash_out_initially = ZERO;
    general_cash_flow_data.cash_initially_in_acounts = ZERO;
    general_cash_flow_data.total_cash_available_for_building = ZERO;
    general_cash_flow_data.schedule.clear();
    general_cash_flow_data.individual_home_data.clear();
}

/*
 */
void init_data() {

    money_market_amount = ZERO;
    checking_account_amount = ZERO;
    other_sources_amount = ZERO;

    choose_temporal_structure();
    initial_date.month_index = set_initial_month("January");
    initial_date.specification_index = -SENTINEL;
    initial_date.week_index = -SENTINEL;

    database.clear();
    init_general_cash_flow_data();
}

/*
 */
void init_program() {

    init_interactors();
    init_east_interactors();
    init_interactor_data();

    init_data();

    add_interactors();
    add_east_interactors();

    init_home_window_interactors();

    std::cout << std::fixed;
    std::cout.precision(STREAM_PRECISION);

    // Inits bool preventing internals from tripping action listeners
    was_internals = true;
}

/*
 */
bool evaluate_main_textfields() {

    if ((days_in_month(initial_date.month_index) >= getIntegerAugment(day_field->getText())) &&
            (getIntegerAugment(day_field->getText()) != ZERO)) {
        initial_date.specification_index = getIntegerAugment(day_field->getText());
    } else {
        GOptionPane::showMessageDialog("Enter a valid day.");
        day_field->setText(integerToString(initial_date.specification_index));
        return false;
    }

    if (getRealAugment(money_market_field->getText()) >= ZERO) {
        money_market_amount = getRealAugment(money_market_field->getText());
    } else {
        money_market_field->setText(integerToString(money_market_amount));
        return false;
    }

    if (getRealAugment(checking_account_field->getText()) >= ZERO) {
        checking_account_amount = getRealAugment(checking_account_field->getText());
    } else {
        checking_account_field->setText(integerToString(checking_account_amount));
        return false;
    }

    if (getRealAugment(other_sources_field->getText()) >= ZERO) {
        other_sources_amount = getRealAugment(other_sources_field->getText());
    } else {
        other_sources_field->setText(integerToString(other_sources_amount));
        return false;
    }

    return true;
}

/*
 */
void run_analyzer(Vector<Home> homes_list) {

    if (!evaluate_main_textfields() || homes_list.isEmpty()) return;

    general_cash_flow_data = evaluate_cash_flow(homes_list, values);

    graphically_represent_data(general_cash_flow_data);
}

/*
 */
void evaluate_action_event(GActionEvent e) {

    if (e.getSource() == value_chooser) {

//        // If they didn't change the format, do nothing
//        if (value_chooser->getSelectedItem() == values.unit_adverb) return;

        main_window.clear();

        if (value_chooser->getSelectedItem() != values.unit_adverb) {
            choose_temporal_structure();
            transition_database();
        } else {
            choose_temporal_structure();
        }

        update_home_chooser();
        add_interactors();

        run_analyzer(database);

    } else if (e.getSource() == month_chooser) {

        initial_date.month_index = set_initial_month(month_chooser->getSelectedItem());
        run_analyzer(database);

    } else if (e.getSource() == day_field) {

        if ((days_in_month(initial_date.month_index) >= getIntegerAugment(day_field->getText())) &&
                (getIntegerAugment(day_field->getText()) != ZERO)) {

            initial_date.specification_index = getIntegerAugment(day_field->getText());
            run_analyzer(database);

        } else {
            GOptionPane::showMessageDialog("Enter a valid day.");
            day_field->setText(integerToString(initial_date.specification_index));
        }

    } else if (e.getSource() == add_home_button) {

        if (add_home()) {
            update_home_chooser();
            run_analyzer(database);
        }

    } else if (e.getSource() == reset_button) {

        main_window.clear();

        init_data();
        init_interactor_data();
        init_east_interactors();
//        add_east_interactors();
//        add_interactors();

    } else if (e.getSource() == money_market_field) {

        if (getRealAugment(money_market_field->getText()) >= ZERO) {
            money_market_amount = getRealAugment(money_market_field->getText());
            run_analyzer(database);
        } else {
            money_market_field->setText(integerToString(money_market_amount));
        }

    } else if (e.getSource() == checking_account_field) {

        if (getRealAugment(checking_account_field->getText()) >= ZERO) {
            checking_account_amount = getRealAugment(checking_account_field->getText());
            run_analyzer(database);
        } else {
            checking_account_field->setText(integerToString(checking_account_amount));
        }

    } else if (e.getSource() == other_sources_field) {

        if (getRealAugment(other_sources_field->getText()) >= ZERO) {
            other_sources_amount = getRealAugment(other_sources_field->getText());
            run_analyzer(database);
        } else {
            other_sources_field->setText(integerToString(other_sources_amount));
        }

    } else if (e.getSource() == home_chooser) {

        if (!was_internals) {

            std::string response = GOptionPane::showOptionDialog("What would you like to do to " +
                    home_chooser->getSelectedItem() + "?", {"Cancel", "Remove", "Edit"},
                    "Cancel");

            if (response != "Cancel" && response != "") {

                bool check = true;

                if (response == "Remove") {
                    remove_home(home_chooser->getSelectedItem());
                } else if (response == "Edit") {
                    check = edit_home(home_chooser->getSelectedItem());
                }

                if (check) {
                    main_window.clearCanvas();
                    update_home_chooser();
                    run_analyzer(database);
                }
            }

        } else {
            was_internals = false;
        }

    } else if (e.getSource() == run_analysis_button) {
        run_analyzer(database);
    } else if (e.getSource() == clear_database_button) {

        database.clear();
        init_general_cash_flow_data();
        update_home_chooser();
        main_window.clearCanvas();

    } else if (e.getSource() == cash_on_hand_checkbox) {
        graphically_represent_data(general_cash_flow_data);
    } else if (e.getSource() == cash_out_total_checkbox) {
        graphically_represent_data(general_cash_flow_data);
    } else if (e.getSource() == cash_out_this_temporal_unit_checkbox) {
        graphically_represent_data(general_cash_flow_data);
    } else if (e.getSource() == cash_in_checkbox) {
        graphically_represent_data(general_cash_flow_data);
    }
}

/*
 */
void evaluate_table_event(GTableEvent e) {
    create_data_table(e.getColumn(), e.getRow());
}

/*
 */
int main() {

    init_window();
    init_program();

    while (true) {

        GEvent e = waitForEvent(ACTION_EVENT | TABLE_EVENT);
        if (e.getEventClass() == ACTION_EVENT) {
            evaluate_action_event(e);
        } else if (e.getEventClass() == TABLE_EVENT) {
            evaluate_table_event(e);
        }

    }

    return 0;
}

