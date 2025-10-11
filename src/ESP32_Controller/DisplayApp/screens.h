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
    lv_obj_t *menu_main_opt1;
    lv_obj_t *menu_main_opt2;
    lv_obj_t *menu_main_opt3;
    lv_obj_t *button_exit_menu;
    lv_obj_t *menu_main_opt_open_dev_menu;
    lv_obj_t *menu_main_title_1;
    lv_obj_t *menu_dev_opt1;
    lv_obj_t *button_back_menu_dev;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_INFO_SCREEN = 1,
    SCREEN_ID_MENU_MAIN = 2,
    SCREEN_ID_MENU_DEV = 3,
};

void create_screen_info_screen();
void tick_screen_info_screen();

void create_screen_menu_main();
void tick_screen_menu_main();

void create_screen_menu_dev();
void tick_screen_menu_dev();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/