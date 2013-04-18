#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0xC0, 0xD9, 0xA1, 0x12, 0x78, 0xF8, 0x47, 0xA7, 0x8D, 0x7F, 0x01, 0x24, 0x96, 0xED, 0xC1, 0xA1 }
PBL_APP_INFO(MY_UUID,
             "Habs Watchface", "mail@juliengrenier.cc",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
BmpContainer background_image_container;
TextLayer time_information_layer;
TextLayer date_information_layer;

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) ;
void display_time(PblTm *time);

void handle_deinit(AppContextRef ctx) {
  (void)ctx;
  //bmp_deinit_container(&background_image_container);
}

void handle_init(AppContextRef ctx) {

  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);
  Layer *root_layer = window_get_root_layer(&window);
  // Set up a layer for the static watch face background
  resource_init_current_app(&APP_RESOURCES);
  bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND, &background_image_container);
  layer_add_child(root_layer, &background_image_container.layer.layer);

  GFont droid = fonts_get_system_font(FONT_KEY_DROID_SERIF_28_BOLD);

  text_layer_init(&date_information_layer, GRect(0,98, 144, 35));
  layer_add_child(root_layer, &date_information_layer.layer);
  text_layer_set_text_alignment(&date_information_layer, GTextAlignmentCenter);

  text_layer_set_font(&date_information_layer, droid);

  text_layer_init(&time_information_layer, GRect(0,98+36, 144, 30));
  layer_add_child(root_layer, &time_information_layer.layer);
  text_layer_set_text_alignment(&time_information_layer, GTextAlignmentCenter);
  text_layer_set_font(&time_information_layer, droid);

  PblTm time;
  get_time(&time);
  display_time(&time);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }
  };
  app_event_loop(params, &handlers);
}


void display_time(PblTm *time){

  // Need to be static because they're used by the system later.
  static char time_text[] = "00:00";
  static char date_text[] = "Xxxxxxxxx 00";

  char *time_format;


  // TODO: Only update the date when it's changed.
  string_format_time(date_text, sizeof(date_text), "%b %e", time);
  text_layer_set_text(&date_information_layer, date_text);


  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  string_format_time(time_text, sizeof(time_text), time_format, time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(&time_information_layer, time_text);
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)ctx;
  display_time(t->tick_time);
}
