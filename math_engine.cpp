/*
 * File: math_engine.cpp
 * -------------------------------------------------------------------------------------------------
 *
 */

#include "math_engine.h"
#include "generics.h"
#include "date_engine.h"
#include <cmath>

/*
 */
bool check_delay(Home& home, Temporal_Unit& current_unit, Local_Temporal_Unit& local_unit,
        Temporal_Values values) {

    if (is_delayed(home)) {

        home.temporal_units_delayed--;
        current_unit.houses_delayed += home;

        local_unit.delayed = true;
        return true;

    } else if (is_under_construction(home, values)) {

        current_unit.cash_out_this_temporal_unit += (home.cost_of_construction /
                                              values.units_to_complete_house);
        current_unit.houses_under_construction += home;

        local_unit.under_construction = true;
        local_unit.cash_out_this_temporal_unit += (home.cost_of_construction /
                                                   values.units_to_complete_house);
    }

    return false;
}

/*
 */
void evaluate_land_data(Home& home, Temporal_Unit& current_unit, Local_Temporal_Unit&
        local_temporal_unit) {

    current_unit.cash_out_this_temporal_unit += home.cost_of_land;
    current_unit.houses_land_is_bought_for += home;

    local_temporal_unit.cash_out_this_temporal_unit += home.cost_of_land;
    local_temporal_unit.land_bought_this_unit = true;
}

/*
 */
void evaluate_loan_draw(Home& home, Temporal_Values values, bool delayed,
        Temporal_Unit& current_unit, Local_Temporal_Unit& local_unit) {

    for (int temporal_unit: home.loan_draw_schedule) {

        if (temporal_unit == temporal_units_under_construction(home, values) && !delayed) {
            current_unit.cash_in += (home.cost_of_construction / home.number_of_loan_draws);
            current_unit.houses_loan_dispersements_made_for += home;

            local_unit.cash_in += (home.cost_of_construction / home.number_of_loan_draws);
            local_unit.loan_dispersement_made = true;
        }

    }
}

/*
 */
void evaluate_sale_data(Home& home, Temporal_Unit& current_unit, Local_Temporal_Unit&
        local_temporal_unit) {

    if (!is_using_loan_draws(home)) {
        current_unit.cash_in += home.cost_of_construction;
        local_temporal_unit.cash_in += home.cost_of_construction;
    }

    current_unit.cash_in += home.cost_of_land;
    current_unit.houses_sold += home;

    local_temporal_unit.cash_in += home.cost_of_land;
    local_temporal_unit.sold_this_unit = true;
}

/*
 */
void sum_local_cash_flow_data(Home& home, Local_Temporal_Unit& local_temporal_unit) {

    home.cash_flow_data.cumulative_cash_out_total +=
            local_temporal_unit.cash_out_this_temporal_unit;

    home.cash_flow_data.cumulative_cash_out_total -= local_temporal_unit.cash_in;
    local_temporal_unit.cash_out_total = (home.cash_flow_data.cumulative_cash_out_total > ZERO ?
            home.cash_flow_data.cumulative_cash_out_total : ZERO);
}

/*
 */
void evaluate_homes(General_Cash_Flow_Data& cash_flow_data, Temporal_Unit& current_unit,
        Temporal_Values values) {

    for (Home& home: cash_flow_data.individual_home_data) {

        if (!is_already_sold(home)) {

            Local_Temporal_Unit local_temporal_unit = {current_unit.name, "", 0, 0, 0,
                    false, false, false, false, false, false};

            bool delayed = check_delay(home, current_unit, local_temporal_unit, values);

            home.temporal_units_to_end--;
            home.temporal_units_to_sale--;
            home.temporal_units_to_land_payment--;

            if (land_is_to_be_bought_this_temporal_unit(home)) {
                evaluate_land_data(home, current_unit, local_temporal_unit);
            }

            if (is_using_loan_draws(home)) {
                evaluate_loan_draw(home, values, delayed, current_unit, local_temporal_unit);
            }

            if (is_finished(home)) {
                current_unit.houses_finished += home;
                local_temporal_unit.finished_this_unit = true;
            }

            if (is_sold_this_temporal_unit(home)) {
                evaluate_sale_data(home, current_unit, local_temporal_unit);
            }

            sum_local_cash_flow_data(home, local_temporal_unit);

            int units_under_construction = temporal_units_under_construction(home, values);
            if (units_under_construction < -SENTINEL) {
                local_temporal_unit.temporal_unit_name_relative_to_home = "Pre-construction";
            } else if (units_under_construction >= values.units_to_complete_house) {
                local_temporal_unit.temporal_unit_name_relative_to_home = "Finished";
            } else {
                local_temporal_unit.temporal_unit_name_relative_to_home = values.unit_type_caps +
                        " " + integerToString(units_under_construction);
            }

            home.cash_flow_data.schedule += local_temporal_unit;

        }
    }
}

/*
 */
void sum_cash_flow_data(Temporal_Unit& period, double& total_cash, double& cash_out_total) {

    cash_out_total += period.cash_out_this_temporal_unit;
    cash_out_total -= period.cash_in;

    period.cash_out_total = (cash_out_total <= ZERO ? ZERO : cash_out_total);
    period.cash_on_hand = (((total_cash - cash_out_total) > -ROUNDING_BENCHMRK) &&
            ((total_cash - cash_out_total) < ZERO) ? ZERO : (total_cash - cash_out_total));

}

/*
 */
double get_initial_cash_out(General_Cash_Flow_Data& data, Temporal_Values values) {

    double initial_cash_out = ZERO;
    for (Home& home: data.individual_home_data) {

        if (is_under_construction(home, values)) {
            initial_cash_out += (home.cost_of_construction / values.units_to_complete_house)
                    * temporal_units_under_construction(home, values);
            home.cash_flow_data.cash_out_initially += (home.cost_of_construction /
                    values.units_to_complete_house) * temporal_units_under_construction(home,
                    values);
        }

        if (is_using_loan_draws(home)) {
            for (int temporal_unit: home.loan_draw_schedule) {

                if (temporal_unit <= temporal_units_under_construction(home, values)) {
                    initial_cash_out -= (home.cost_of_construction / home.number_of_loan_draws);
                    home.cash_flow_data.cash_out_initially -= (home.cost_of_construction /
                            home.number_of_loan_draws);
                }

            }
        }

        // if land has already been purchased
        if (!land_is_to_be_bought(home)) {
            initial_cash_out += home.cost_of_land;
            home.cash_flow_data.cash_out_initially += home.cost_of_land;
        }

        home.cash_flow_data.cumulative_cash_out_total += home.cash_flow_data.cash_out_initially;
    }

    return initial_cash_out;
}

/*
 */
General_Cash_Flow_Data evaluate_cash_flow(Vector<Home> schedule, Temporal_Values values) {

    General_Cash_Flow_Data cash_flow_data;

    cash_flow_data.individual_home_data = schedule;
    double total_cash = money_market_amount + checking_account_amount +
            other_sources_amount;
    double cash_out_total = get_initial_cash_out(cash_flow_data, values);
    //std::cout << std::endl <<"Cash Out Initially: " << cash_out_total << std::endl;
    cash_flow_data.cash_out_initially = cash_out_total;
    //std::cout << "Total Cash In Accounts Right Now: " << total_cash << std::endl;
    cash_flow_data.cash_initially_in_acounts = total_cash;

    total_cash += (cash_out_total <= ZERO ? ZERO : cash_out_total);
    //std::cout << "Total Cash Available For Building: " << total_cash << std::endl << std::endl;
    cash_flow_data.total_cash_available_for_building = total_cash;

    Date current_date = initial_date;

    for (int temporal_unit = 0; temporal_unit <
         schedule[schedule.size() - 1].temporal_units_to_sale; temporal_unit++) {

        Temporal_Unit current_period = {get_updated_date(current_date, values), 0, 0, 0, 0, {}, {},
                {}, {}};
        evaluate_homes(cash_flow_data, current_period, values);
        sum_cash_flow_data(current_period, total_cash, cash_out_total);

        cash_flow_data.schedule += current_period;

    }

    return cash_flow_data;
}
