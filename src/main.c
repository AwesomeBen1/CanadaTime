// Canada Time
// Made by Ben Chapman-Kish on 2015-04-18
#include "pebble.h"

static Window *s_main_window;
static TextLayer *s_date_layer, *s_time_layer;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // Need to be static because they're used by the system later.
  static char s_time_text[] = "12:00";
  static char s_date_text[] = "25/12";

	// Date is currently updated every minute, should be changed later.
  strftime(s_date_text, sizeof(s_date_text), "%e/%m", tick_time);
  text_layer_set_text(s_date_layer, s_date_text);

  char *time_format;
  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }
  strftime(s_time_text, sizeof(s_time_text), time_format, tick_time);

  // Handle lack of non-padded hour format string for twelve hour clock.
  if (!clock_is_24h_style() && (s_time_text[0] == '0')) {
    memmove(s_time_text, &s_time_text[1], sizeof(s_time_text) - 1);
  }
  text_layer_set_text(s_time_layer, s_time_text);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
	
	// Background image
	s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
	s_background_layer = bitmap_layer_create(layer_get_bounds(window_layer));
	bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
	layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
	
	// Month and day
  s_date_layer = text_layer_create(GRect(0, 96, 144, 168-95));
 	#ifdef PBL_COLOR
		text_layer_set_text_color(s_date_layer, GColorDarkCandyAppleRed);
	#else
  	text_layer_set_text_color(s_date_layer, GColorBlack);
	#endif
	text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));

	// The time
  s_time_layer = text_layer_create(GRect(0, 66, 144, 168-66));
  #ifdef PBL_COLOR
		text_layer_set_text_color(s_time_layer, GColorDarkCandyAppleRed);
	#else
  	text_layer_set_text_color(s_time_layer, GColorBlack);
	#endif
	text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
	gbitmap_destroy(s_background_bitmap);
	bitmap_layer_destroy(s_background_layer);
	text_layer_destroy(s_date_layer);
  text_layer_destroy(s_time_layer);
}

static void init() {
  s_main_window = window_create();
  #ifdef PBL_COLOR
		window_set_background_color(s_main_window, GColorDarkCandyAppleRed);
	#else
  	window_set_background_color(s_main_window, GColorBlack);
	#endif
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
  
  // Prevent starting blank
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  handle_minute_tick(t, MINUTE_UNIT);
}

static void deinit() {
  window_destroy(s_main_window);
  tick_timer_service_unsubscribe();
}

int main() {
  init();
  app_event_loop();
  deinit();
}