#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *info_screen;
    lv_obj_t *menu_main;
    lv_obj_t *menu_dev;
    lv_obj_t *menu_settings_time_ndate;
    lv_obj_t *menu_settings_time_ndate2;
    lv_obj_t *menu_settings_light;
    lv_obj_t *terrarium_info_widget;
    lv_obj_t *terrarium_air_temp_value;
    lv_obj_t *terrarium_air_temp_bar;
    lv_obj_t *terrarium_humidity_bar;
    lv_obj_t *terrarium_humidity_value;
    lv_obj_t *terrarium_ground_humidity_bar;
    lv_obj_t *terrarium_ground_humidity_value;
    lv_obj_t *controller_temp_widget;
    lv_obj_t *controller_temp_widget_temp_label;
    lv_obj_t *controller_temp_widget_temp_value;
    lv_obj_t *button_open_menu;
    lv_obj_t *top_icon_bar_widget;
    lv_obj_t *img_door_opened;
    lv_obj_t *clock_widget;
    lv_obj_t *clock_date;
    lv_obj_t *clock_time;
    lv_obj_t *clock_day;
    lv_obj_t *menu_main_title;
    lv_obj_t *menu_main_button_set_time_ndate;
    lv_obj_t *menu_main_opt_light_settings;
    lv_obj_t *menu_main_opt3;
    lv_obj_t *button_exit_menu;
    lv_obj_t *menu_main_opt_open_dev_menu;
    lv_obj_t *menu_main_title_1;
    lv_obj_t *menu_dev_opt_reset_clock;
    lv_obj_t *button_back_menu_dev;
    lv_obj_t *menu_dev_opt1;
    lv_obj_t *menu_main_title_2;
    lv_obj_t *menu_time_ndate_button_cancel;
    lv_obj_t *menu_time_ndate_button_time_next;
    lv_obj_t *roller_time_ndate_time_hours;
    lv_obj_t *roller_time_ndate_time_minutes;
    lv_obj_t *menu_main_title_3;
    lv_obj_t *menu_main_title_4;
    lv_obj_t *menu_main_title_5;
    lv_obj_t *menu_time_ndate_button_prev;
    lv_obj_t *menu_time_ndate_button_save;
    lv_obj_t *roller_time_ndate_date_day;
    lv_obj_t *roller_time_ndate_date_month;
    lv_obj_t *menu_main_title_6;
    lv_obj_t *menu_main_title_7;
    lv_obj_t *roller_time_ndate_date_year;
    lv_obj_t *menu_main_title_8;
    lv_obj_t *menu_main_title_9;
    lv_obj_t *lights_off_widget;
    lv_obj_t *roller_lights_off_time_hour;
    lv_obj_t *roller_lights_off_time_minute;
    lv_obj_t *menu_main_title_11;
    lv_obj_t *button_back_menu_light_settings;
    lv_obj_t *lights_on_widget;
    lv_obj_t *roller_lights_on_time_hour;
    lv_obj_t *roller_lights_on_time_minute;
    lv_obj_t *menu_main_title_10;
    lv_obj_t *menu_light_settings_button_save;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_INFO_SCREEN = 1,
    SCREEN_ID_MENU_MAIN = 2,
    SCREEN_ID_MENU_DEV = 3,
    SCREEN_ID_MENU_SETTINGS_TIME_NDATE = 4,
    SCREEN_ID_MENU_SETTINGS_TIME_NDATE2 = 5,
    SCREEN_ID_MENU_SETTINGS_LIGHT = 6,
};

void create_screen_info_screen();
void tick_screen_info_screen();

void create_screen_menu_main();
void tick_screen_menu_main();

void create_screen_menu_dev();
void tick_screen_menu_dev();

void create_screen_menu_settings_time_ndate();
void tick_screen_menu_settings_time_ndate();

void create_screen_menu_settings_time_ndate2();
void tick_screen_menu_settings_time_ndate2();

void create_screen_menu_settings_light();
void tick_screen_menu_settings_light();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/