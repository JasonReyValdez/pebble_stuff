#include <pebble.h>

static Window *main_window;
static TextLayer *time_layer;
static TextLayer *message_layer;


static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
     text_layer_set_text(time_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void main_window_load(Window *window){
    time_layer = text_layer_create(GRect(0,55,144,50));
	text_layer_set_background_color(time_layer, GColorClear);
    text_layer_set_text_color(time_layer, GColorBlack);
    text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
    text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));

    message_layer = text_layer_create(GRect(0,0,144,50));
	text_layer_set_background_color(message_layer, GColorClear);
    text_layer_set_text_color(message_layer, GColorBlack);
    text_layer_set_text(message_layer,"Time");
    text_layer_set_font(message_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
    text_layer_set_text_alignment(message_layer, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(message_layer));
    
}

static void main_window_unload(Window *window){
	text_layer_destroy(time_layer);
    text_layer_destroy(message_layer);
}

void init(void) {
	main_window = window_create();
	window_set_window_handlers(main_window,(WindowHandlers){
		.load = main_window_load,
		.unload = main_window_unload
	});

    // Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    
	window_stack_push(main_window, true);
    
    update_time();
}

void deinit(void) {

	window_destroy(main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
