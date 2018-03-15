/*
 * File: date_engine.cpp
 * -------------------------------------------------------------------------------------------------
 */

#include "date_engine.h"
#include <string>
#include "simpio.h"
#include "strlib.h"

/*
 */
int set_initial_month(std::string month_name) {

    month_name = toLowerCase(month_name);
    while (true) {
        if (month_name == "january") {
            return 1;
        } else if (month_name == "february") {
            return 2;
        } else if (month_name == "march") {
            return 3;
        } else if (month_name == "april") {
            return 4;
        } else if (month_name == "may") {
            return 5;
        } else if (month_name == "june") {
            return 6;
        } else if (month_name == "july") {
            return 7;
        } else if (month_name == "august") {
            return 8;
        } else if (month_name == "september") {
            return 9;
        } else if (month_name == "october") {
            return 10;
        } else if (month_name == "november") {
            return 11;
        } else if (month_name == "december") {
            return 12;
        }
    }

}

/*
 */
std::string get_current_month(int index) {

    switch (index) {
        case 1 :
            return "January";
        case 2 :
            return "February";
        case 3 :
            return "March";
        case 4 :
            return "April";
        case 5 :
            return "May";
        case 6 :
            return "June";
        case 7 :
            return "July";
        case 8 :
            return "August";
        case 9 :
            return "September";
        case 10 :
            return "October";
        case 11 :
            return "November";
        case 0 :
            return "December";
        default:
            return "Error";
    }

}

int days_in_month(int month) {
    switch(month) {
        case 1 :
            return 31;
        case 2 :
            return 28;
        case 3 :
            return 31;
        case 4 :
            return 30;
        case 5 :
            return 31;
        case 6 :
            return 30;
        case 7 :
            return 31;
        case 8 :
            return 31;
        case 9 :
            return 30;
        case 10 :
            return 31;
        case 11 :
            return 30;
        case 0 :
            return 31;
        default:
            return SENTINEL;
    }
}

/*
 */
void date_by_week(Date& date, std::string& current_date) {

    current_date += integerToString(date.week_index) + " -- " + get_current_month(date.month_index)
            + " " + integerToString(date.specification_index) + " - ";

    date.specification_index += DAYS_PER_WEEK;

    if (date.specification_index > days_in_month(date.month_index)) {
        date.specification_index = date.specification_index % days_in_month(date.month_index);
        date.month_index = (date.month_index + 1) % MONTHS_PER_YEAR;
    }

    current_date += get_current_month(date.month_index) + " " +
            integerToString(date.specification_index);

    date.week_index++;
}

/*
 */
void date_by_month(Date& date, std::string& current_date) {
    current_date += get_current_month(date.month_index);
    date.month_index = (date.month_index + 1) % MONTHS_PER_YEAR;
}

/*
 */
std::string get_updated_date(Date& date, Temporal_Values values) {

    std::string updated_date = "";

    if (values.unit_type == "week") {
        date_by_week(date, updated_date);
    } else if (values.unit_type == "month") {
        date_by_month(date, updated_date);
    }

    return updated_date;

}

