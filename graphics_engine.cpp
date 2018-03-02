/*
 * File: graphics_engine.cpp
 * -------------------------------------------------------------------------------------------------
 *
 */

#include <string>
#include <cmath>
#include "graphics_engine.h"
#include "generics.h"
#include "database.h"
#include "strlib.h"
#include "gwindow.h"
#include "ginteractors.h"
#include "gevents.h"
#include "goptionpane.h"
#include "gobjects.h"
#include "gtable.h"
#include "map.h"

// For Main Window
const int WINDOW_OFFSET = 88;
const int SPACEHOLDER = 26;
const int SPACEHOLDER_SUBTRACTION = 3;

GWindow main_window;

GChooser* value_chooser;
GChooser* month_chooser;
GTextField* day_field;

GButton* add_home_button;
GButton* reset_button;

GTextField* money_market_field;
GTextField* checking_account_field;
GTextField* other_sources_field;

GLabel* home_chooser_label;
GChooser* home_chooser;
GButton* run_analysis_button;
GButton* clear_database_button;
GLabel* east_spaceholder;

GCheckBox* cash_on_hand_checkbox;
GCheckBox* cash_out_total_checkbox;
GCheckBox* cash_out_this_temporal_unit_checkbox;
GCheckBox* cash_in_checkbox;

// For Add/Edit Home Window
const double HOME_WINDOW_OFFSET_FACTOR = 3.5;
const int HOME_EXTENDED_WINDOW_OFFSET_FACTOR = 2;

const int HOME_WINDOW_DIVISOR = 8;
const int HOME_WINDOW_EXTENDED_DIVISOR = 4;
const int HOME_CENTERING_DIVIDEND = 7;
const int HOME_CENTERING_DIVISOR = 16;
const int HOME_EXTENDED_CENTERING_DIVIDEND = 3;
const int HOME_EXTENDED_CENTERING_DIVISOR = 8;

const int ADD_HOME_PRELIMINARY_EVENT_NUMBER = 4;
const int EDIT_HOME_PRELIMINARY_EVENT_NUMBER = 3;
bool was_internals;

GTextField* address_field;

GChooser* under_construction_chooser;
GChooser* delay_chooser;
GTextField* delay_field;

GTextField* temporal_units_to_finish_or_start_field;
GTextField* temporal_units_to_sale_field;

GTextField* lot_price_field;
GTextField* construction_price_field;

GChooser* lot_payment_chooser;
GTextField* temporal_units_to_land_payment_field;

GChooser* loan_draw_chooser;
GCheckBox* one;
GCheckBox* two;
GCheckBox* three;
GCheckBox* four;
GCheckBox* five;
GCheckBox* six;
GCheckBox* seven;
GCheckBox* eight;
GCheckBox* nine;
GCheckBox* ten;
GCheckBox* eleven;
GCheckBox* twelve;
GCheckBox* thirteen;
GCheckBox* fourteen;
GCheckBox* fifteen;
GCheckBox* sixteen;
GCheckBox* seventeen;
GCheckBox* eighteen;
GCheckBox* nineteen;
GCheckBox* twenty;

GButton* save_button;
GButton* cancel_button;

// For Generic Tables: Canvas
const int INDEX_TWO = 2;
const int INDEX_THREE = 3;
const int INDEX_FOUR = 4;
const int INDEX_FIVE = 5;

const double TABLE_HEIGHT_FACTOR = 16.05;
const int INITIAL_ROW_HEIGHT = 22;

// For Main Window: Canvas
const double BUFFER_WIDTH = 49;
const double PERIOD_TABLE_WIDTH = 231;
const double GENERIC_TABLE_WIDTH = 163;

const int MAX_WIDTH_INDEX = 4;
const int INITIAL_NUM_COLUMNS = 2;
const int MAIN_TABLE_NUM_COLUMNS = 6;

// For Main Window: Graph
const double UPPER_MARGIN = 10;
const double BOTTOM_MARGIN = 30;
const double SIDE_MARGIN = 80;
const double AXIS_LINE_WIDTH = 2.5;
const double LINE_WIDTH = 1.5;

const int NUM_VERTICAL_LABELS = 11;
const int MAX_NUM_HORIZONTAL_LABELS = 40;

// For Temporal Unit Data Window: Canvas
const int TEMPORAL_UNIT_TABLE_NUM_COLUMNS = 6;

// For Home Data Window: Canvas
const int HOME_DATA_TABLE_NUM_COLUMNS = 6;

// For Generic Data Tables: Canvas & Generic Data Windows
const int TABLE_WIDTH_COEFFICIENT = 5;
const int TABLE_WIDTH_DIVISOR = 6;
const double DATA_TABLE_PERIOD_WIDTH = 110;

const int DATA_WINDOW_HEIGHT = 500;
const int DATA_WINDOW_LOCATION_HEIGHT_DIVISOR = 8;

/*
 * Generic Window: Canvas GUI Functions
 *--------------------------------------------------------------------------------------------------
 */

/*
 */
void vertical_add_label(double x, double& y, std::string title, std::string font, GWindow& gw) {

    GLabel* label = new GLabel(title);
    label->setFont(font);
    y += label->getHeight();
    gw.add(label, x, y);
}

/*
 */
void horizontal_add_label(double& x, double& y, std::string title, std::string font, GWindow& gw) {

    GLabel* label = new GLabel(title);
    label->setFont(font);
    gw.add(label, x, y);
    x += label->getWidth();
}

/*
 */
double get_table_height(GTable* table) {
    return INITIAL_ROW_HEIGHT + (TABLE_HEIGHT_FACTOR * (table->numRows() + SENTINEL));
}

/*
 * Generic Data Window: Canvas GUI Functions
 *--------------------------------------------------------------------------------------------------
 */

/*
 */
GTable* init_data_table(int rows, int columns, GWindow gw) {

    GTable* table = new GTable(rows, columns);

    double height = get_table_height(table);
    double display_height = (height < gw.getCanvasHeight() / 2 ? height :
            gw.getCanvasHeight() / 2);

    table->setSize((TABLE_WIDTH_COEFFICIENT * gw.getWidth()) / TABLE_WIDTH_DIVISOR, display_height);
    table->setEditable(false);
    table->setRowColumnHeadersVisible(false);

    return table;
}

/*
 */
GWindow init_data_window(std::string title) {

    GWindow gw(getScreenWidth(), DATA_WINDOW_HEIGHT);
    gw.setLocation(ZERO, (getScreenWidth()) / DATA_WINDOW_LOCATION_HEIGHT_DIVISOR);

    gw.setTitle(title);

    return gw;
}

/*
 */
void center_data_table_headers(GTable* table) {
    for (int i = ZERO; i < table->numCols(); i++) {
        table->setCellAlignment(ZERO, i, GTable::Alignment::CENTER);
    }
}

/*
 * Home Data Window: Canvas GUI Functions
 *--------------------------------------------------------------------------------------------------
 */

/*
 */
void set_upper_home_data_table_headers(GTable* table) {

    table->set(ZERO, ZERO, "PERIOD DATES (" + values.unit_type_all_caps_plural + ")");
    table->set(ZERO, -SENTINEL, "STAGE OF HOME (" + values.unit_type_all_caps + ")");
    table->set(ZERO, INDEX_TWO, "CASH OUT TOTAL ($)");
    table->set(ZERO, INDEX_THREE, "CASH OUT THIS " + values.unit_type_all_caps + " ($)");
    table->set(ZERO, INDEX_FOUR, "CASH IN THIS " + values.unit_type_all_caps + " ($)");
    table->set(ZERO, INDEX_FIVE, "LOAN DISPERSEMENT MADE");

    center_data_table_headers(table);
}

/*
 */
void set_lower_home_data_table_headers(GTable* table, Local_Temporal_Unit unit) {

    //table->setHorizontalAlignment(GTable::Alignment::CENTER);
    table->set(ZERO, ZERO, "PERIOD DATES (" + values.unit_type_all_caps_plural + ")");
    table->set(ZERO, -SENTINEL, "STAGE OF HOME (" + values.unit_type_all_caps + ")");
    table->set(ZERO, INDEX_TWO, "UNDER CONSTRUCTION");

    if (unit.under_construction || unit.delayed) {
        table->set(ZERO, INDEX_THREE, "DELAYED");
    } else {
        table->set(ZERO, INDEX_THREE, "LAND BOUGHT");
    }

    table->set(ZERO, INDEX_FOUR, "FINISHED");
    table->set(ZERO, INDEX_FIVE, "SOLD");

    center_data_table_headers(table);
}

/*
 */
void set_upper_home_data_table_data(GTable* table, Vector<Local_Temporal_Unit> schedule) {

    set_upper_home_data_table_headers(table);

    for (int i = ZERO; i < schedule.size(); i++) {

        table->set(i - SENTINEL, ZERO, " " + schedule[i].temporal_unit_name);
        table->set(i - SENTINEL, -SENTINEL, " " + schedule[i].temporal_unit_name_relative_to_home);
        table->set(i - SENTINEL, INDEX_TWO, doubleToString(schedule[i].cash_out_total));
        table->set(i - SENTINEL, INDEX_THREE,
                doubleToString(schedule[i].cash_out_this_temporal_unit));
        table->set(i - SENTINEL, INDEX_FOUR, doubleToString(schedule[i].cash_in));
        table->set(i - SENTINEL, INDEX_FIVE, (schedule[i].loan_dispersement_made ? "YES" : ""));
    }

    table->setColumnWidth(ZERO, DATA_TABLE_PERIOD_WIDTH);
}

/*
 */
void set_lower_home_data_table_data(GTable* table, Vector<Local_Temporal_Unit> schedule) {

    set_lower_home_data_table_headers(table, schedule[ZERO]);

    for (int i = ZERO; i < schedule.size(); i++) {

        table->set(i - SENTINEL, ZERO, " " + schedule[i].temporal_unit_name);
        table->set(i - SENTINEL, -SENTINEL, " " + schedule[i].temporal_unit_name_relative_to_home);
        table->set(i - SENTINEL, INDEX_TWO, (schedule[i].under_construction ? "YES" : "NO"));

        if (schedule[ZERO].under_construction || schedule[ZERO].delayed) {
            table->set(i - SENTINEL, INDEX_THREE, (schedule[i].delayed ? "YES" : ""));
        } else {
            table->set(i - SENTINEL, INDEX_THREE, (schedule[i].land_bought_this_unit ? "YES" : ""));
        }

        table->set(i - SENTINEL, INDEX_FOUR, (schedule[i].finished_this_unit ? "YES" : ""));
        table->set(i - SENTINEL, INDEX_FIVE, (schedule[i].sold_this_unit ? "YES" : ""));
    }

    table->setColumnWidth(ZERO, DATA_TABLE_PERIOD_WIDTH);
}

/*
 */
void add_home_label_data(Home home, GWindow gw) {

    double x = (TABLE_WIDTH_COEFFICIENT * gw.getWidth()) / TABLE_WIDTH_DIVISOR;
    double y = ZERO;

    vertical_add_label(x, y, " ADDRESS OF HOME:", "SansSerif-14", gw);
    vertical_add_label(x, y, "        " + home.address, "SansSerif-14", gw);
    vertical_add_label(x, y, " TOTAL COST OF HOUSE:", "SansSerif-14", gw);
    vertical_add_label(x, y, "       $" + doubleToString(home.total_cost), "SansSerif-14", gw);
    vertical_add_label(x, y, " COST OF LAND:", "SansSerif-14", gw);
    vertical_add_label(x, y, "       $" + doubleToString(home.cost_of_land), "SansSerif-14", gw);
    vertical_add_label(x, y, " COST OF CONSTRUCTION:", "SansSerif-14", gw);
    vertical_add_label(x, y, "       $" + doubleToString(home.cost_of_construction), "SansSerif-14",
            gw);
    vertical_add_label(x, y, " CASH INITIALLY OUT FOR HOUSE:", "SansSerif-14", gw);
    vertical_add_label(x, y, "       $" + doubleToString(home.cash_flow_data.cash_out_initially),
            "SansSerif-14", gw);
}

/*
 */
void graphically_represent_home_data(int index) {

    Home home = general_cash_flow_data.individual_home_data[index + SENTINEL];
    GWindow home_data_window = init_data_window(home.address);

    GTable* upper_home_data_table = init_data_table(home.cash_flow_data.schedule.size() - SENTINEL,
        HOME_DATA_TABLE_NUM_COLUMNS, home_data_window);
    GTable* lower_home_data_table = init_data_table(home.cash_flow_data.schedule.size() - SENTINEL,
        HOME_DATA_TABLE_NUM_COLUMNS, home_data_window);

    set_upper_home_data_table_data(upper_home_data_table, home.cash_flow_data.schedule);
    set_lower_home_data_table_data(lower_home_data_table, home.cash_flow_data.schedule);

    add_home_label_data(home, home_data_window);

    home_data_window.add(upper_home_data_table, ZERO, ZERO);
    home_data_window.add(lower_home_data_table, ZERO, home_data_window.getCanvasHeight() / 2);

    waitForClick();
    home_data_window.close();
}

/*
 * Temporal Unit Data Window: Canvas GUI Functions
 *--------------------------------------------------------------------------------------------------
 */

/*
 */
int temporal_unit_data_table_get_rows_needed(Temporal_Unit unit) {

    int rows_needed = ZERO;

    if (unit.houses_delayed.size() > rows_needed) rows_needed = unit.houses_delayed.size();
    if (unit.houses_finished.size() > rows_needed) rows_needed = unit.houses_finished.size();
    if (unit.houses_loan_dispersements_made_for.size() > rows_needed) rows_needed =
            unit.houses_loan_dispersements_made_for.size();
    if (unit.houses_sold.size() > rows_needed) rows_needed = unit.houses_sold.size();
    if (unit.houses_under_construction.size() > rows_needed) rows_needed =
            unit.houses_under_construction.size();
    if (unit.houses_land_is_bought_for.size() > rows_needed) rows_needed =
            unit.houses_land_is_bought_for.size();

    return rows_needed - SENTINEL;
}

/*
 */
void set_temporal_unit_table_headers(GTable* table) {

    table->set(ZERO, ZERO, "HOMES UNDER CONSTRUCTION");
    table->set(ZERO, -SENTINEL, "HOMES DELAYED");
    table->set(ZERO, INDEX_TWO, "LAND BOUGHT FOR");
    table->set(ZERO, INDEX_THREE, "LOAN DISPERSEMENTS MADE FOR");
    table->set(ZERO, INDEX_FOUR, "HOMES FINISHED");
    table->set(ZERO, INDEX_FIVE, "HOMES SOLD");

    center_data_table_headers(table);
}

/*
 */
void set_temporal_unit_table_data(GTable* table, Temporal_Unit unit) {

    set_temporal_unit_table_headers(table);

    for (int i = ZERO; i < (table->numRows() + SENTINEL); i++) {

        if (unit.houses_under_construction.size() > i) {
            table->set(i - SENTINEL, ZERO, " " + unit.houses_under_construction[i].address);
        }

        if (unit.houses_delayed.size() > i) {
            table->set(i - SENTINEL, -SENTINEL, unit.houses_delayed[i].address);
        }

        if (unit.houses_land_is_bought_for.size() > i) {
            table->set(i - SENTINEL, INDEX_TWO, unit.houses_land_is_bought_for[i].address);
        }

        if (unit.houses_loan_dispersements_made_for.size() > i) {
            table->set(i - SENTINEL, INDEX_THREE,
                       unit.houses_loan_dispersements_made_for[i].address);
        }

        if (unit.houses_finished.size() > i) {
            table->set(i - SENTINEL, INDEX_FOUR, unit.houses_finished[i].address);
        }

        if (unit.houses_sold.size() > i) {
            table->set(i - SENTINEL, INDEX_FIVE, unit.houses_sold[i].address);
        }
    }

}

/*
 */
void add_temporal_unit_label_data(GWindow& gw, Temporal_Unit unit, int index) {

    double x = (TABLE_WIDTH_COEFFICIENT * gw.getWidth()) / TABLE_WIDTH_DIVISOR;
    double y = ZERO;

    vertical_add_label(x, y, " PERIOD NAME (" + values.unit_type_all_caps + " " +
            integerToString(index) + "):", "SansSerif-14", gw);
    if (values.unit_type == "week") {
        vertical_add_label(x, y, "       " + unit.name.substr((index < 10 ? 5 : 6)), "SansSerif-14",
            gw);
    } else {
        vertical_add_label(x, y, "       " + unit.name, "SansSerif-14", gw);
    }

    vertical_add_label(x, y, " CASH ON HAND:", "SansSerif-14", gw);
    vertical_add_label(x, y, "       $" + doubleToString(unit.cash_on_hand), "SansSerif-14", gw);
    vertical_add_label(x, y, " CASH OUT TOTAL:", "SansSerif-14", gw);
    vertical_add_label(x, y, "       $" + doubleToString(unit.cash_out_total), "SansSerif-14", gw);
    vertical_add_label(x, y, " CASH OUT THIS " + values.unit_type_all_caps + ":", "SansSerif-14",
            gw);
    vertical_add_label(x, y, "       $" + doubleToString(unit.cash_out_this_temporal_unit),
            "SansSerif-14", gw);
    vertical_add_label(x, y, " CASH IN THIS " + values.unit_type_all_caps + ":", "SansSerif-14",
            gw);
    vertical_add_label(x, y, "       $" + doubleToString(unit.cash_in), "SansSerif-14", gw);
}

/*
 */
void graphically_represent_temporal_unit_data(int index) {

    Temporal_Unit unit = general_cash_flow_data.schedule[index + SENTINEL];
    GWindow temporal_unit_data_window = init_data_window(values.unit_type_caps + ": " + unit.name);
    GTable* temporal_unit_data_table = init_data_table(temporal_unit_data_table_get_rows_needed
            (unit), TEMPORAL_UNIT_TABLE_NUM_COLUMNS, temporal_unit_data_window);

    set_temporal_unit_table_data(temporal_unit_data_table, unit);
    add_temporal_unit_label_data(temporal_unit_data_window, unit, index);

    temporal_unit_data_window.add(temporal_unit_data_table, ZERO, ZERO);

    waitForClick();
    temporal_unit_data_window.close();
}

/*--------------------------------------------------------------------------------------------------
 * Splice Function: Generic Data Window: Canvas - Temporal Unit Data Window vs. Home Data Window
 *--------------------------------------------------------------------------------------------------
 */

/*
 */
void create_data_table(int column, int row) {

    if (row == ZERO) return;

    if (column == ZERO) {
        graphically_represent_home_data(row);
    } else {
        graphically_represent_temporal_unit_data(row);
    }
}

/*
 * Main Window: Canvas GRAPH GUI Functions
 *--------------------------------------------------------------------------------------------------
 */

Map<int, Vector<double>> extract_data(General_Cash_Flow_Data data) {

    Map<int, Vector<double>> data_vectors;

    Vector<double> cash_on_hand_vector;
    Vector<double> cash_out_total_vector;
    Vector<double> cash_out_this_temporal_unit_vector;
    Vector<double> cash_in_vector;

    cash_on_hand_vector += data.cash_initially_in_acounts;
    cash_out_total_vector += data.cash_out_initially;

    for (Temporal_Unit unit: data.schedule) {
        cash_on_hand_vector += unit.cash_on_hand;
        cash_out_total_vector += unit.cash_out_total;
        cash_out_this_temporal_unit_vector += unit.cash_out_this_temporal_unit;
        cash_in_vector += unit.cash_in;
    }

    cash_out_this_temporal_unit_vector.insert(ZERO, cash_out_this_temporal_unit_vector[ZERO]);
    cash_in_vector.insert(ZERO, cash_in_vector[ZERO]);

    if (cash_on_hand_checkbox->isSelected()) data_vectors[ZERO] += cash_on_hand_vector;
    if (cash_out_total_checkbox->isSelected()) data_vectors[-SENTINEL] = cash_out_total_vector;
    if (cash_out_this_temporal_unit_checkbox->isSelected()) data_vectors[INDEX_TWO] +=
            cash_out_this_temporal_unit_vector;
    if (cash_in_checkbox->isSelected()) data_vectors[INDEX_THREE] += cash_in_vector;

    return data_vectors;
}

/*
 */
void add_horizontal_axis_scale(int num_periods, GPoint* bottom, double x_length) {

    GLabel* unit_type = new GLabel(values.unit_type_all_caps_plural + ":");
    main_window.add(unit_type, SIDE_MARGIN - unit_type->getWidth(), main_window.getCanvasHeight() -
            (BOTTOM_MARGIN / 2) + (unit_type->getFontAscent() / 2));

    for (int i = -SENTINEL; i <= num_periods; ((num_periods < MAX_NUM_HORIZONTAL_LABELS) ? i++ :
            i += (num_periods / MAX_NUM_HORIZONTAL_LABELS) - SENTINEL)) {

        GLabel* label = new GLabel(integerToString(i));
        label->setFont("SansSerif-11");

        main_window.add(label, bottom->getX() + i * (x_length / num_periods) -
                (label->getWidth() / 2), bottom->getY() + label->getHeight());
    }
}

/*
 */
void add_vertical_axis_scale(double max_value, double min_value, GPoint* bottom, double y_length) {

    double y = (main_window.getCanvasHeight() / 2) + UPPER_MARGIN;

    for (double i = ZERO; i < NUM_VERTICAL_LABELS; i++) {

        double factor = ((max_value - min_value) / (NUM_VERTICAL_LABELS + SENTINEL));
        // integerToString() used to truncate decimals
        GLabel* label = new GLabel("$" + integerToString(max_value - factor * i) + " ");
        label->setFont("SansSerif-11");

        main_window.add(label, bottom->getX() - label->getWidth(), y + i * (y_length /
                (NUM_VERTICAL_LABELS + SENTINEL)) + (label->getFontAscent() / 2));
    }
}

/*
 */
Graph_Data create_axes(Map<int, Vector<double>> data_vectors, int num_periods) {

    GPoint* bottom = new GPoint(SIDE_MARGIN, (main_window.getCanvasHeight() - BOTTOM_MARGIN));
    double x_length = (PERIOD_TABLE_WIDTH + SENTINEL + (MAX_WIDTH_INDEX * GENERIC_TABLE_WIDTH)) -
            SIDE_MARGIN;
    double y_length = (main_window.getCanvasHeight() - BOTTOM_MARGIN) -
            (main_window.getCanvasHeight() / 2) - UPPER_MARGIN;

    double max = ZERO;
    double min = ZERO;
    for (Vector<double> vector: data_vectors.values()) {
        for (double num: vector) {
            if (num < min) min = num;
            if (num > max) max = num;
        }
    }

    double range = max - min;
    double y_min = (range == ZERO ? ZERO : (min / range) * y_length);
    y_min = (y_min > ZERO ? ZERO : y_min);

    add_vertical_axis_scale(max, min, bottom, y_length);
    add_horizontal_axis_scale(num_periods, bottom, x_length);

    GLine* x_axis = new GLine(bottom->getX(), bottom->getY() + y_min, bottom->getX() + x_length +
            BOTTOM_MARGIN, bottom->getY() + y_min);
    x_axis->setLineWidth(AXIS_LINE_WIDTH);
    GLine* y_axis = new GLine(bottom->getX(), bottom->getY(), bottom->getX(), bottom->getY() -
            y_length);
    y_axis->setLineWidth(AXIS_LINE_WIDTH);

    return {x_axis, y_axis, x_length, y_length, num_periods, max, y_min, range,
            new GPoint(bottom->getX(), bottom->getY() + y_min)};
}

/*
 */
void graph_vector(Vector<double> data, Graph_Data scale, std::string color) {

    if (scale.range == ZERO) return;
    for (int i = ZERO; i < scale.x_max; i++) {
        GLine* line = new GLine(scale.origin->getX() + i * (scale.x_length / scale.x_max),
                scale.origin->getY() - data[i] * (scale.y_length / scale.range),
                scale.origin->getX() + (i - SENTINEL) * (scale.x_length / scale.x_max),
                scale.origin->getY() - data[i - SENTINEL] * (scale.y_length / scale.range));

        line->setColor(color);
        line->setLineWidth(LINE_WIDTH);

        main_window.add(line);
    }
}

/*
 */
void graph_data(Map<int, Vector<double>> data_vectors, Graph_Data scale) {

    main_window.add(scale.x_axis);
    main_window.add(scale.y_axis);

    if (data_vectors.containsKey(ZERO)) graph_vector(data_vectors[ZERO], scale, "BLUE");
    if (data_vectors.containsKey(-SENTINEL)) graph_vector(data_vectors[-SENTINEL], scale,
            "BLACK");
    if (data_vectors.containsKey(INDEX_TWO)) graph_vector(data_vectors[INDEX_TWO], scale, "RED");
    if (data_vectors.containsKey(INDEX_THREE)) graph_vector(data_vectors[INDEX_THREE], scale,
            "GRAY");


}

/*
 */
void create_graph(General_Cash_Flow_Data data) {

    Map<int, Vector<double>> data_vectors = extract_data(data);

    Graph_Data scale = create_axes(data_vectors, data.schedule.size());
    graph_data(data_vectors, scale);
}

/*
 * Main Window: Canvas GUI Functions
 *--------------------------------------------------------------------------------------------------
 */

/*
 */
double add_main_canvas_label_data(General_Cash_Flow_Data data) {

    double x = PERIOD_TABLE_WIDTH + SENTINEL + (MAX_WIDTH_INDEX * GENERIC_TABLE_WIDTH);
    double y = ZERO;

    vertical_add_label(x, y, " CASH INITIALLY OUT:", "SansSerif-14", main_window);
    vertical_add_label(x, y, "         $" + doubleToString(data.cash_out_initially), "SansSerif-14",
            main_window);
    vertical_add_label(x, y, " CASH INITIALLY IN ACCOUNTS:", "SansSerif-14", main_window);
    vertical_add_label(x, y, "         $" + doubleToString(data.cash_initially_in_acounts),
            "SansSerif-14", main_window);
    vertical_add_label(x, y, " CASH AVAILABLE FOR BUILDING:", "SansSerif-14", main_window);
    vertical_add_label(x, y, "         $" + doubleToString(data.total_cash_available_for_building),
            "SansSerif-14", main_window);

    vertical_add_label(x, y, "---------------------------------", "SansSerif-14", main_window);

    return y;
}

/*
 */
void set_column_titles(GTable* table) {

    table->set(ZERO, -SENTINEL, " PERIOD DATES (" + values.unit_type_all_caps_plural + ")");
    table->set(ZERO, INDEX_TWO, "CASH ON HAND ($)");
    table->set(ZERO, INDEX_THREE, "CASH OUT TOTAL ($)");
    table->set(ZERO, INDEX_FOUR, "CASH OUT THIS " + values.unit_type_all_caps + " ($)");
    table->set(ZERO, INDEX_FIVE, "CASH IN THIS " + values.unit_type_all_caps + " ($)");
}

/*
 */
void set_cash_flow_table_data(GTable* table, General_Cash_Flow_Data data) {

    for (int i = ZERO; i < data.schedule.size(); i++) {

        table->set(i - SENTINEL, -SENTINEL, " " + data.schedule[i].name);

        table->set(i - SENTINEL, INDEX_TWO, doubleToString(data.schedule[i].cash_on_hand));
        table->set(i - SENTINEL, INDEX_THREE, doubleToString(data.schedule[i].cash_out_total));
        table->set(i - SENTINEL, INDEX_FOUR,
                doubleToString(data.schedule[i].cash_out_this_temporal_unit));
        table->set(i - SENTINEL, INDEX_FIVE, doubleToString(data.schedule[i].cash_in));
    }
}

/*
 */
GTable* init_main_cash_flow_table(General_Cash_Flow_Data data) {

    GTable* table = new GTable(data.schedule.size() - SENTINEL, MAIN_TABLE_NUM_COLUMNS,
            -BUFFER_WIDTH + SENTINEL, ZERO);

    double height = get_table_height(table);
    double display_height = (height < main_window.getCanvasHeight() / 2 ? height :
        main_window.getCanvasHeight() / 2);
    table->setSize(BUFFER_WIDTH + PERIOD_TABLE_WIDTH, display_height);

    set_column_titles(table);

    set_cash_flow_table_data(table, data);

    table->setEditable(false);
    table->setRowColumnHeadersVisible(false);

    table->setSize(BUFFER_WIDTH + PERIOD_TABLE_WIDTH + (MAX_WIDTH_INDEX * GENERIC_TABLE_WIDTH),
            display_height);
    table->setColumnWidth(ZERO, BUFFER_WIDTH);
    table->setColumnWidth(-SENTINEL, PERIOD_TABLE_WIDTH);
    for (int j = INITIAL_NUM_COLUMNS; j < MAIN_TABLE_NUM_COLUMNS; j++) {
        table->setColumnWidth(j, GENERIC_TABLE_WIDTH);
    }

    return table;
}

/*
 */
GTable* init_main_homes_table(General_Cash_Flow_Data data, double y) {

    double x = PERIOD_TABLE_WIDTH + SENTINEL + (MAX_WIDTH_INDEX * GENERIC_TABLE_WIDTH);
    GTable* table = new GTable(data.individual_home_data.size() - SENTINEL, -SENTINEL, x, y);

    double height = get_table_height(table);
    table->setSize(main_window.getCanvasWidth() - x, (height < main_window.getCanvasHeight()
            - y ? height : main_window.getCanvasHeight() - y));

    table->setEditable(false);
    table->setRowColumnHeadersVisible(false);

    table->set(ZERO, ZERO, "HOMES IN DATABASE");
    table->setCellAlignment(ZERO, ZERO, GTable::Alignment::CENTER);

    for (int i = ZERO; i < data.individual_home_data.size(); i++) {
        table->set(i - SENTINEL, ZERO, " " + data.individual_home_data[i].address);
    }

    return table;
}

/*
 */
void graphically_represent_data(General_Cash_Flow_Data data) {

    main_window.clearCanvas();

    // If there is nothing to put on canvas, do nothing
    if (data.schedule.isEmpty()) return;

    GTable* cash_flow_table = init_main_cash_flow_table(data);
    main_window.add(cash_flow_table);
    double y = add_main_canvas_label_data(data);

    GTable* homes_table = init_main_homes_table(data, y);
    main_window.add(homes_table);

    create_graph(data);
}

/*
 * Generic Home Window: GUI Functions
 *--------------------------------------------------------------------------------------------------
 */

/*
 */
void default_window_size(GWindow& gw) {
    gw.setSize(getScreenWidth() / HOME_WINDOW_DIVISOR, getScreenHeight() -
            (HOME_WINDOW_OFFSET_FACTOR * WINDOW_OFFSET));
    gw.setLocation((HOME_CENTERING_DIVIDEND * getScreenWidth()) /
            HOME_CENTERING_DIVISOR, ZERO);
}

/*
 */
GWindow init_home_window() {

    GWindow new_home_window;
    default_window_size(new_home_window);
    new_home_window.setWindowTitle("New Home");

    new_home_window.setRegionAlignment("WEST", "LEFT");
    new_home_window.setRegionAlignment("EAST", "LEFT");

    return new_home_window;

}

/*
 */
void init_home_window_interactors() {

    address_field = new GTextField;

    under_construction_chooser = new GChooser;
    under_construction_chooser->addItems({"Yes", "No"});

    delay_chooser = new GChooser;
    delay_chooser->addItems({"Yes", "No"});

    delay_field = new GTextField;

    temporal_units_to_finish_or_start_field = new GTextField;
    temporal_units_to_sale_field = new GTextField;

    lot_price_field = new GTextField;
    construction_price_field = new GTextField;

    lot_payment_chooser = new GChooser;
    lot_payment_chooser->addItems({"Yes", "No"});

    temporal_units_to_land_payment_field = new GTextField;

    loan_draw_chooser = new GChooser;
    loan_draw_chooser->addItems({"Yes", "No"});

    one = new GCheckBox("1");
    two = new GCheckBox("2");
    three = new GCheckBox("3");
    four = new GCheckBox("4");
    five = new GCheckBox("5");
    six = new GCheckBox("6");
    seven = new GCheckBox("7");
    eight = new GCheckBox("8");
    nine = new GCheckBox("9");
    ten = new GCheckBox("10");
    eleven = new GCheckBox("11");
    twelve = new GCheckBox("12");
    thirteen = new GCheckBox("13");
    fourteen = new GCheckBox("14");
    fifteen = new GCheckBox("15");
    sixteen = new GCheckBox("16");
    seventeen = new GCheckBox("17");
    eighteen = new GCheckBox("18");
    nineteen = new GCheckBox("19");
    twenty = new GCheckBox("20");

    save_button = new GButton ("Save");
    cancel_button = new GButton ("Cancel");

}

/*
 */
void deselect_dispersements() {

    one->setSelected(false);
    two->setSelected(false);
    three->setSelected(false);
    four->setSelected(false);
    five->setSelected(false);
    six->setSelected(false);
    seven->setSelected(false);
    eight->setSelected(false);
    nine->setSelected(false);
    ten->setSelected(false);
    eleven->setSelected(false);
    twelve->setSelected(false);
    thirteen->setSelected(false);
    fourteen->setSelected(false);
    fifteen->setSelected(false);
    sixteen->setSelected(false);
    seventeen->setSelected(false);
    eighteen->setSelected(false);
    nineteen->setSelected(false);
    twenty->setSelected(false);

}

/*
 */
void add_delay_interactors(GWindow gw) {

    if (under_construction_chooser->getSelectedItem() == "Yes") {

        gw.addToRegion(new GLabel("Is It Delayed? :"), "WEST");
        gw.addToRegion(delay_chooser, "WEST");

        if (delay_chooser->getSelectedItem() == "Yes") {
            gw.addToRegion(new GLabel("By How Many " + values.unit_type_caps_plural + "? :"),
                           "WEST");
            gw.addToRegion(delay_field, "WEST");
        }

    }
}

/*
 */
void add_lot_interactors(GWindow gw) {

    if (under_construction_chooser->getSelectedItem() == "No") {

        gw.addToRegion(new GLabel("Have You Bought The Lot? :"), "WEST");
        gw.addToRegion(lot_payment_chooser, "WEST");

        if (lot_payment_chooser->getSelectedItem() == "No") {

            gw.addToRegion(new GLabel(values.unit_type_caps_plural + " Until It Is Bought:"),
                "WEST");
            gw.addToRegion(temporal_units_to_land_payment_field, "WEST");

        }

    }

}

/*
 */
void add_loan_draw_interactors(GWindow gw) {

    gw.addToRegion(new GLabel("Does This Home Use"), "WEST");
    gw.addToRegion(new GLabel("Loan Draws? :"), "WEST");
    gw.addToRegion(loan_draw_chooser, "WEST");

    if (loan_draw_chooser->getSelectedItem() == "Yes") {

        gw.setSize(getScreenWidth() / HOME_WINDOW_EXTENDED_DIVISOR, getScreenHeight() -
                (HOME_EXTENDED_WINDOW_OFFSET_FACTOR * WINDOW_OFFSET));
        gw.setLocation((HOME_EXTENDED_CENTERING_DIVIDEND * getScreenWidth()) /
                HOME_EXTENDED_CENTERING_DIVISOR, ZERO);

        gw.addToRegion(new GLabel("Select " + values.unit_type_caps_plural +" For Which"),
                "EAST");
        gw.addToRegion(new GLabel("Dispersements Have Been/  "), "EAST");
        gw.addToRegion(new GLabel("Will Be Made:"), "EAST");

        gw.addToRegion(one, "EAST");
        gw.addToRegion(two, "EAST");
        gw.addToRegion(three, "EAST");
        gw.addToRegion(four, "EAST");
        gw.addToRegion(five, "EAST");

        if (values.unit_type == "week") {

            gw.addToRegion(six, "EAST");
            gw.addToRegion(seven, "EAST");
            gw.addToRegion(eight, "EAST");
            gw.addToRegion(nine, "EAST");
            gw.addToRegion(ten, "EAST");
            gw.addToRegion(eleven, "EAST");
            gw.addToRegion(twelve, "EAST");
            gw.addToRegion(thirteen, "EAST");
            gw.addToRegion(fourteen, "EAST");
            gw.addToRegion(fifteen, "EAST");
            gw.addToRegion(sixteen, "EAST");
            gw.addToRegion(seventeen, "EAST");
            gw.addToRegion(eighteen, "EAST");
            gw.addToRegion(nineteen, "EAST");
            gw.addToRegion(twenty, "EAST");

        }

    }
}

/*
 */
void add_home_window_interactors(GWindow gw) {

    default_window_size(gw);

    gw.addToRegion(new GLabel("Address:"), "WEST");
    gw.addToRegion(address_field, "WEST");

    gw.addToRegion(new GLabel("Is It Under Construction? :"), "WEST");
    gw.addToRegion(under_construction_chooser, "WEST");

    add_delay_interactors(gw);

    if (under_construction_chooser->getSelectedItem() == "Yes") {

        if (delay_chooser->getSelectedItem() == "Yes") {
            gw.addToRegion(new GLabel(values.unit_type_caps_plural + " Until It Is Finished,  "),
                    "WEST");
            gw.addToRegion(new GLabel("Including Any Delay:"),
                    "WEST");
        } else {
            gw.addToRegion(new GLabel(values.unit_type_caps_plural + " Until It Is Finished:  "),
                    "WEST");
        }

    } else {
        gw.addToRegion(new GLabel(values.unit_type_caps_plural + " Until It Is Started:"), "WEST");
    }
    gw.addToRegion(temporal_units_to_finish_or_start_field, "WEST");

    gw.addToRegion(new GLabel(values.unit_type_caps_plural + " Until It Is Sold:"), "WEST");
    gw.addToRegion(temporal_units_to_sale_field, "WEST");

    gw.addToRegion(new GLabel("Lot Price:"), "WEST");
    gw.addToRegion(lot_price_field, "WEST");

    gw.addToRegion(new GLabel("Construction Price:"), "WEST");
    gw.addToRegion(construction_price_field, "WEST");

    add_lot_interactors(gw);
    add_loan_draw_interactors(gw);

    gw.addToRegion(new GLabel(""), "SOUTH");
    gw.addToRegion(save_button, "SOUTH");
    gw.addToRegion(cancel_button, "SOUTH");
    gw.addToRegion(new GLabel(""), "SOUTH");

}

/*
 */
void evaluate_home_textfields(Home& home) {

    home.address = address_field->getText();
    home.temporal_units_delayed = stringToInteger(delay_field->getText());

    if (under_construction_chooser->getSelectedItem() == "Yes") {
        home.temporal_units_to_end = stringToInteger(temporal_units_to_finish_or_start_field->
                getText());
    } else if (under_construction_chooser->getSelectedItem() == "No") {
        home.temporal_units_to_end = stringToInteger(temporal_units_to_finish_or_start_field->
                getText()) + values.units_to_complete_house;
    }

    home.temporal_units_to_sale = stringToInteger(temporal_units_to_sale_field->getText());
    home.cost_of_land = stringToDouble(lot_price_field->getText());
    home.cost_of_construction = stringToDouble(construction_price_field->getText());
    home.total_cost = home.cost_of_land + home.cost_of_construction;
    home.temporal_units_to_land_payment = stringToInteger(temporal_units_to_land_payment_field->
            getText());

}

/*
 */
void evaluate_checkbox(GCheckBox* checkbox, Home& home, int num) {

    if (checkbox->isSelected()) {
        home.loan_draw_schedule += num;
    } else {
        home.loan_draw_schedule -= num;
    }
}

/*
 */
bool evaluate_home_action(GActionEvent e, GWindow& gw, Home& home, bool& check, bool is_new) {

    if (e.getSource() == address_field) {

        home.address = address_field->getText();
        gw.setTitle(home.address);
        return true;

    } else if (e.getSource() == under_construction_chooser) {

        gw.clear();
        if (under_construction_chooser->getSelectedItem() == "Yes") {
            home.temporal_units_to_land_payment = ZERO;
            temporal_units_to_land_payment_field->setText("0");
        } else {
            home.temporal_units_delayed = ZERO;
            delay_field->setText("0");
        }

        home.temporal_units_to_end = ZERO;
        home.temporal_units_to_sale = ZERO;
        temporal_units_to_finish_or_start_field->setText("0");
        temporal_units_to_sale_field->setText("0");

        add_home_window_interactors(gw);
        return true;

    } else if (e.getSource() == delay_chooser) {

        gw.clear();
        if (delay_chooser->getSelectedItem() == "No") {
            home.temporal_units_delayed = ZERO;
            delay_field->setText("0");
        }
        add_home_window_interactors(gw);
        return true;

    } else if (e.getSource() == delay_field) {

        home.temporal_units_delayed = stringToInteger(delay_field->getText());
        return true;

    } else if (e.getSource() == temporal_units_to_finish_or_start_field) {

        if (under_construction_chooser->getSelectedItem() == "Yes") {
            home.temporal_units_to_end = stringToInteger(temporal_units_to_finish_or_start_field->
                    getText());
        } else if (under_construction_chooser->getSelectedItem() == "No") {
            home.temporal_units_to_end = stringToInteger(temporal_units_to_finish_or_start_field->
                    getText()) + values.units_to_complete_house;
        }
        return true;

    } else if (e.getSource() == temporal_units_to_sale_field) {

        home.temporal_units_to_sale = stringToInteger(temporal_units_to_sale_field->getText());
        return true;

    } else if (e.getSource() == lot_price_field) {

        home.cost_of_land = stringToDouble(lot_price_field->getText());
        home.total_cost = home.cost_of_land + home.cost_of_construction;
        return true;

    } else if (e.getSource() == construction_price_field) {

        home.cost_of_construction = stringToDouble(construction_price_field->getText());
        home.total_cost = home.cost_of_land + home.cost_of_construction;
        return true;

    } else if (e.getSource() == lot_payment_chooser) {

        gw.clear();
        if (lot_payment_chooser->getSelectedItem() == "Yes") {
            home.temporal_units_to_land_payment = ZERO;
            temporal_units_to_land_payment_field->setText("0");
        }
        add_home_window_interactors(gw);
        return true;

    } else if (e.getSource() == temporal_units_to_land_payment_field) {

        home.temporal_units_to_land_payment = stringToInteger(temporal_units_to_land_payment_field->
                getText());
        return true;

    } else if (e.getSource() == loan_draw_chooser) {

        gw.clear();
        if (loan_draw_chooser->getSelectedItem() == "No") {
            home.loan_draw_schedule.clear();
            home.number_of_loan_draws = ZERO;
            deselect_dispersements();
        }
        add_home_window_interactors(gw);
        return true;

    } else if (e.getSource() == one) {

        evaluate_checkbox(one, home, 1);
        return true;

    } else if (e.getSource() == two) {

        evaluate_checkbox(two, home, 2);
        return true;

    } else if (e.getSource() == three) {

        evaluate_checkbox(three, home, 3);
        return true;

    } else if (e.getSource() == four) {

        evaluate_checkbox(four, home, 4);
        return true;

    } else if (e.getSource() == five) {

        evaluate_checkbox(five, home, 5);
        return true;

    } else if (e.getSource() == six) {

        evaluate_checkbox(six, home, 6);
        return true;

    } else if (e.getSource() == seven) {

        evaluate_checkbox(seven, home, 7);
        return true;

    } else if (e.getSource() == eight) {

        evaluate_checkbox(eight, home, 8);
        return true;

    } else if (e.getSource() == nine) {

        evaluate_checkbox(nine, home, 9);
        return true;

    } else if (e.getSource() == ten) {

        evaluate_checkbox(ten, home, 10);
        return true;

    } else if (e.getSource() == eleven) {

        evaluate_checkbox(eleven, home, 11);
        return true;

    } else if (e.getSource() == twelve) {

        evaluate_checkbox(twelve, home, 12);
        return true;

    } else if (e.getSource() == thirteen) {

        evaluate_checkbox(thirteen, home, 13);
        return true;

    } else if (e.getSource() == fourteen) {

        evaluate_checkbox(fourteen, home, 14);
        return true;

    } else if (e.getSource() == fifteen) {

        evaluate_checkbox(fifteen, home, 15);
        return true;

    } else if (e.getSource() == sixteen) {

        evaluate_checkbox(sixteen, home, 16);
        return true;

    } else if (e.getSource() == seventeen) {

        evaluate_checkbox(seventeen, home, 17);
        return true;

    } else if (e.getSource() == eighteen) {

        evaluate_checkbox(eighteen, home, 18);
        return true;

    } else if (e.getSource() == nineteen) {

        evaluate_checkbox(nineteen, home, 19);
        return true;

    } else if (e.getSource() == twenty) {

        evaluate_checkbox(twenty, home, 20);
        return true;

    } else if (e.getSource() == save_button) {

        evaluate_home_textfields(home);

        if (validate_entry(home, (under_construction_chooser->getSelectedItem() == "Yes"),
                (delay_chooser->getSelectedItem() == "Yes"),
                (lot_payment_chooser->getSelectedItem() == "No"),
                (loan_draw_chooser->getSelectedItem() == "Yes"))) {

            home.number_of_loan_draws = home.loan_draw_schedule.size();

            if (is_new && database_contains_address(home.address)) {
                if (GOptionPane::showConfirmDialog("This address already exists in the database. "
                "Are you sure you want to overwrite it?")) {
                    return false;
                } else {
                    return true;
                }
            }

            return false;
        }

        return true;

    } else if (e.getSource() == cancel_button) {

        check = false;
        return false;

    }

    check = false;
    return false;

}

/*
 */
void init_action_evaluator(GWindow& gw, Home& home, bool& check, bool is_new, int num) {

    int count = 0;
    while (true) {

        GActionEvent e  = waitForEvent(ACTION_EVENT);
        if (count >= num) {
            if (!evaluate_home_action(e, gw, home, check, is_new)) break;
        }
        count++;

    }

    gw.close();
}

/*
 * Edit Home Window: GUI Functions
 *--------------------------------------------------------------------------------------------------
 */

/*
 */
void init_edit_window_under_construction_interactor_data(Home home){

    under_construction_chooser->setSelectedItem("Yes");

    if (is_delayed(home)) {
        delay_chooser->setSelectedItem("Yes");
        delay_field->setText(integerToString(home.temporal_units_delayed));
    }

    temporal_units_to_finish_or_start_field->setText(integerToString
            (home.temporal_units_to_end));

    lot_payment_chooser->setSelectedItem("Yes");
    temporal_units_to_land_payment_field->setText("0");
}

/*
 */
void init_edit_window_to_be_built_interactor_data(Home home) {

    under_construction_chooser->setSelectedItem("No");
    delay_chooser->setSelectedItem("No");
    delay_field->setText("0");

    temporal_units_to_finish_or_start_field->setText(integerToString
            (home.temporal_units_to_end - values.units_to_complete_house));

    if (land_is_to_be_bought(home)) {
        lot_payment_chooser->setSelectedItem("No");
        temporal_units_to_land_payment_field->setText
                (integerToString(home.temporal_units_to_land_payment));
    }
}

/*
 */
void init_edit_window_checkbox(GCheckBox* checkbox, Home& home, int num) {

    if (loan_draw_schedule_contains(home, num)) {
        checkbox->setSelected(true);
    } else {
        checkbox->setSelected(false);
    }
}

/*
 */
void init_edit_window_loan_dispersement_interactor_data(Home home) {

    if (!is_using_loan_draws(home)) {
        loan_draw_chooser->setSelectedItem("No");
        deselect_dispersements();
    } else {
        loan_draw_chooser->setSelectedItem("Yes");

        init_edit_window_checkbox(one, home, 1);
        init_edit_window_checkbox(two, home, 2);
        init_edit_window_checkbox(three, home, 3);
        init_edit_window_checkbox(four, home, 4);
        init_edit_window_checkbox(five, home, 5);
        init_edit_window_checkbox(six, home, 6);
        init_edit_window_checkbox(seven, home, 7);
        init_edit_window_checkbox(eight, home, 8);
        init_edit_window_checkbox(nine, home, 9);
        init_edit_window_checkbox(ten, home, 10);
        init_edit_window_checkbox(eleven, home, 11);
        init_edit_window_checkbox(twelve, home, 12);
        init_edit_window_checkbox(thirteen, home, 13);
        init_edit_window_checkbox(fourteen, home, 14);
        init_edit_window_checkbox(fifteen, home, 15);
        init_edit_window_checkbox(sixteen, home, 16);
        init_edit_window_checkbox(seventeen, home, 17);
        init_edit_window_checkbox(eighteen, home, 18);
        init_edit_window_checkbox(nineteen, home, 19);
        init_edit_window_checkbox(twenty, home, 20);

    }
}

/*
 */
void init_edit_window_interactor_data(Home home) {

    address_field->setText(home.address);

    if (is_under_construction(home, values)) {
        init_edit_window_under_construction_interactor_data(home);
    } else {
        init_edit_window_to_be_built_interactor_data(home);
    }

    temporal_units_to_sale_field->setText(integerToString(home.temporal_units_to_sale));

    lot_price_field->setText(integerToString(home.cost_of_land));
    construction_price_field->setText(integerToString(home.cost_of_construction));

    init_edit_window_loan_dispersement_interactor_data(home);

}

/*
 */
void alter_data(Home& home, bool& check) {

    GWindow edit_home_window = init_home_window();
    edit_home_window.setTitle(home.address);

    init_edit_window_interactor_data(home);
    add_home_window_interactors(edit_home_window);

    init_action_evaluator(edit_home_window, home, check, false, EDIT_HOME_PRELIMINARY_EVENT_NUMBER);
}


/*
 * Add Home Window: GUI Functions
 *--------------------------------------------------------------------------------------------------
 */

/*
 */
Home init_new_home_data() {

    Home home;

    home.address = "";
    home.total_cost = ZERO;
    home.cost_of_land = ZERO;
    home.cost_of_construction = ZERO;
    home.number_of_loan_draws = ZERO;
    home.temporal_units_to_land_payment = ZERO;
    home.temporal_units_to_end = ZERO;
    home.temporal_units_delayed = ZERO;
    home.temporal_units_to_sale = ZERO;
    home.cash_flow_data.cash_out_initially = ZERO;
    home.cash_flow_data.cumulative_cash_out_total = ZERO;

    return home;
}

/*
 */
void init_add_home_window_interactor_data() {

    address_field->setText("");

    under_construction_chooser->setSelectedItem("Yes");
    delay_chooser->setSelectedItem("No");
    delay_field->setText("0");

    temporal_units_to_finish_or_start_field->setText("0");
    temporal_units_to_sale_field->setText("0");

    lot_price_field->setText("0");
    construction_price_field->setText("0");

    lot_payment_chooser->setSelectedItem("Yes");
    temporal_units_to_land_payment_field->setText("0");

    loan_draw_chooser->setSelectedItem("No");
    deselect_dispersements();

}

/*
 */
Home create_home(bool& check) {

    Home new_home = init_new_home_data();
    GWindow new_home_window = init_home_window();

    init_add_home_window_interactor_data();
    add_home_window_interactors(new_home_window);

    init_action_evaluator(new_home_window, new_home, check, true,
        ADD_HOME_PRELIMINARY_EVENT_NUMBER);

    return new_home;
}

/*
 * Main Window: GUI Functions
 *--------------------------------------------------------------------------------------------------
 */

/*
 */
void init_interactors() {

    value_chooser = new GChooser;
    value_chooser->addItems({"Weekly", "Monthly"});

    month_chooser = new GChooser;
    month_chooser->addItems({"January", "February", "March", "April", "May", "June", "July",
            "August", "September", "October", "November", "December"});

    day_field = new GTextField();

    add_home_button = new GButton("Add Home");
    reset_button = new GButton("Reset");

    money_market_field = new GTextField;
    checking_account_field = new GTextField;
    other_sources_field = new GTextField;

    cash_on_hand_checkbox = new GCheckBox("Cash On Hand (BLUE)");
    cash_out_total_checkbox = new GCheckBox("Cash Out Total (BLACK)");
    cash_out_this_temporal_unit_checkbox = new GCheckBox("Cash Out This Week/Month (RED)");
    cash_in_checkbox = new GCheckBox("Cash In (GRAY)");
}

/*
 */
void init_east_interactors() {
    home_chooser_label = new GLabel("Homes:");
    home_chooser = new GChooser;
    run_analysis_button = new GButton("Run Analysis");
    clear_database_button = new GButton("Clear Database");
    east_spaceholder = new GLabel("");
}

/*
 */
void init_interactor_data() {

    value_chooser->setSelectedItem("Weekly");
    month_chooser->setSelectedItem("January");
    day_field->setText("1");
    money_market_field->setText("0");
    checking_account_field->setText("0");
    other_sources_field->setText("0");

    cash_on_hand_checkbox->setSelected(true);
    cash_out_total_checkbox->setSelected(false);
    cash_out_this_temporal_unit_checkbox->setSelected(false);
    cash_in_checkbox->setSelected(false);

}

/*
 */
void add_interactors() {

    int buffer = SPACEHOLDER;

    main_window.addToRegion(new GLabel("Date Structure:"), "WEST");
    main_window.addToRegion(value_chooser, "WEST");

    main_window.addToRegion(new GLabel("Current Month:"), "WEST");
    main_window.addToRegion(month_chooser, "WEST");

    if (values.unit_type == "week") {
        main_window.addToRegion(new GLabel("Day:"), "WEST");
        main_window.addToRegion(day_field, "WEST");
        buffer -= SPACEHOLDER_SUBTRACTION;
    }

    for (int i = 0; i < buffer; i++) {
        main_window.addToRegion(new GLabel("\n"), "WEST");
    }

    main_window.addToRegion(add_home_button, "WEST");
    main_window.addToRegion(reset_button, "WEST");

    main_window.setRegionAlignment("NORTH", "RIGHT");

    main_window.addToRegion(new GLabel("Amount In Money Market ($):"), "NORTH");
    main_window.addToRegion(money_market_field, "NORTH");

    main_window.addToRegion(new GLabel("      Amount In Checking Account ($):"), "NORTH");
    main_window.addToRegion(checking_account_field, "NORTH");

    main_window.addToRegion(new GLabel("      Amount From Other Sources ($):"), "NORTH");
    main_window.addToRegion(other_sources_field, "NORTH");

    main_window.addToRegion(cash_on_hand_checkbox, "SOUTH");
    main_window.addToRegion(cash_out_total_checkbox, "SOUTH");
    main_window.addToRegion(cash_out_this_temporal_unit_checkbox, "SOUTH");
    main_window.addToRegion(cash_in_checkbox, "SOUTH");
    main_window.addToRegion(new GLabel(""), "SOUTH");
}

/*
 */
void add_east_interactors() {
    main_window.addToRegion(home_chooser_label, "EAST");
    main_window.addToRegion(home_chooser, "EAST");
    main_window.addToRegion(run_analysis_button, "EAST");
    main_window.addToRegion(clear_database_button, "EAST");
    main_window.addToRegion(east_spaceholder, "EAST");
}

/*
 */
void remove_east_interactors() {

    main_window.removeFromRegion(home_chooser_label, "EAST");
    main_window.removeFromRegion(home_chooser, "EAST");
    main_window.removeFromRegion(run_analysis_button, "EAST");
    main_window.removeFromRegion(clear_database_button, "EAST");
    main_window.removeFromRegion(east_spaceholder, "EAST");

    // Needed to wipe old homes from the chooser
    home_chooser = new GChooser;

}

/*
 */
void update_home_chooser() {

    was_internals = true;
    remove_east_interactors();

    for (Home home: database) {
        home_chooser->addItem(home.address);
    }

    add_east_interactors();
}

/*
 */
void init_window() {

    main_window.setSize(getScreenWidth(), getScreenHeight() - WINDOW_OFFSET);
    main_window.setLocation(ZERO, ZERO);
    main_window.setWindowTitle("Cash Flow Analyzer");

}
