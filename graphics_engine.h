/*
 * File: graphics_engine.h
 * -------------------------------------------------------------------------------------------------
 *
 */

#ifndef Graphics_Included
#define Graphics_Included

#include "generics.h"
#include "gwindow.h"
#include "ginteractors.h"
#include "gtable.h"

extern GWindow main_window;
extern bool was_internals;

extern GChooser* value_chooser;
extern GChooser* month_chooser;
extern GTextField* day_field;

extern GButton* add_home_button;
extern GButton* reset_button;

extern GTextField* money_market_field;
extern GTextField* checking_account_field;
extern GTextField* other_sources_field;

extern GChooser* home_chooser;
extern GButton* run_analysis_button;
extern GButton* clear_database_button;

extern GCheckBox* cash_on_hand_checkbox;
extern GCheckBox* cash_out_total_checkbox;
extern GCheckBox* cash_out_this_temporal_unit_checkbox;
extern GCheckBox* cash_in_checkbox;

struct Graph_Data {

    GLine* x_axis;
    GLine* y_axis;

    double x_length;
    double y_length;
    int x_max;
    double y_max;
    double y_min;

    double range;
    GPoint* origin;

};

/*
 */
void create_data_table(int column, int row);

/*
 */
void graphically_represent_data(General_Cash_Flow_Data data);

/*
 */
void alter_data(Home& home, bool &check);

/*
 */
void init_home_window_interactors();

/*
 */
Home create_home(bool& check);

/*
 */
void init_interactors();

/*
 */
void init_east_interactors();

/*
 */
void init_interactor_data();

/*
 */
void add_interactors();

/*
 */
void add_east_interactors();

/*
 */
void update_home_chooser();

/*
 */
void init_window();

#endif
