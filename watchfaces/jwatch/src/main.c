#include <pebble.h>
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1

static Window *main_window;
static TextLayer *time_layer;
static TextLayer *message_layer;
static TextLayer *weather_layer;
static TextLayer *date_layer;
static TextLayer *month_layer;
static GFont time_font;
static GFont day_font;
static GFont month_font;
static BitmapLayer *background_layer;
static GBitmap *background_bitmap;

static void update_time() {
    // Get a tm structure
    time_t temp = time(NULL); 
    struct tm *tick_time = localtime(&temp);

    // Create a long-lived buffer
    static char buffer[] = "00:00";
    // date buffers
    static char date_buffer[] = "00";
    static char month_buffer[] = "00";
    // Write the current hours and minutes into the buffer
    if(clock_is_24h_style() == true) {
        // Use 24 hour format
        strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
    } else {
        // Use 12 hour format
        strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
    }
    // Link up date buffer with layer
    strftime(date_buffer, sizeof(date_buffer), "%d", tick_time);
    text_layer_set_text(date_layer, date_buffer);
    // LInk up month buffer with layer
    strftime(month_buffer, sizeof(month_buffer), "%m", tick_time);
    text_layer_set_text(month_layer, month_buffer);
    // Display this time on the TextLayer
    text_layer_set_text(time_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void main_window_load(Window *window){
// Background Layer
    background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
    background_layer = bitmap_layer_create(GRect(0,0,144,168));
    bitmap_layer_set_bitmap(background_layer, background_bitmap);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(background_layer));
    
// Month Layer
    month_layer = text_layer_create(GRect(35,45,30,30));
    month_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Elegance_18));
    text_layer_set_font(month_layer,month_font);
    text_layer_set_text_color(month_layer, GColorWhite);
    text_layer_set_background_color(month_layer, GColorClear);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(month_layer));

// Date Layer
    date_layer = text_layer_create(GRect(88,45,30,30));
    text_layer_set_background_color(date_layer, GColorClear);
    text_layer_set_text_color(date_layer, GColorWhite);
    day_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Elegance_18));
    text_layer_set_font(date_layer, day_font);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));
// Weather Layer
    weather_layer = text_layer_create(GRect(0,130,144,25));
    text_layer_set_background_color(weather_layer,GColorClear);
    text_layer_set_text_color(weather_layer, GColorWhite);
    text_layer_set_text_alignment(weather_layer, GTextAlignmentLeft);
    text_layer_set_font(weather_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text(weather_layer,"Loading...");
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(weather_layer));
// Time Layer
    time_layer = text_layer_create(GRect(12,80,120,75));
    text_layer_set_background_color(time_layer, GColorClear);
    text_layer_set_text_color(time_layer, GColorWhite);
    time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Puma_42));
    text_layer_set_font(time_layer, time_font);
    text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
// Heading Layer

}

static void main_window_unload(Window *window){
    text_layer_destroy(time_layer);
    text_layer_destroy(message_layer);
    text_layer_destroy(weather_layer);
    text_layer_destroy(date_layer);
    text_layer_destroy(month_layer);
    gbitmap_destroy(background_bitmap);
    bitmap_layer_destroy(background_layer);
    fonts_unload_custom_font(time_font);
}

static void inbox_received_callback(DictionaryIteratory *iterator, void *context){
}

static void inbox_dropped_callback(AppMessageResult reason, void *context){
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context){
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
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

    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);

    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void deinit(void) {

	window_destroy(main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
