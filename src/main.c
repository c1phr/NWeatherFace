#include <pebble.h>
#include <pebble.h>
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_HIGH_LOW 2
static Window *s_main_window;

static GFont s_time_font;
static GFont s_text_font;
static GFont s_high_low_font;

static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_weather_layer;
static TextLayer *s_high_low_layer;

static char temperature_buffer[8];
static char high_low_buffer[32];
static char weather_layer_buffer[32];
static char high_low_layer_buffer[32];

static Layer *line_layer;

static void update_time()
{
    // Get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    
    // Create a long-lived buffer
    static char buffer[] = "00:00";
    
    // Write the current hours and minutes into the buffer
    if(clock_is_24h_style() == true)
    {
        // Use 24 hour format
        strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
    }
    else
    {
        // Use 12 hour format
        clock_copy_time_string(buffer, sizeof("00:00"));
        //Strip meridan, because I don't care
        if (buffer[2] != ':')
        {
            strcpy(&buffer[4], "");
        }
        
    }
    static char date_buf[80];
    strftime(date_buf, 80, "%a %b %d", tick_time);

    text_layer_set_text(s_date_layer, date_buf);
    // Display this time on the TextLayer
    text_layer_set_text(s_time_layer, buffer);
}

static void main_window_load(Window *window) {
    // Create time TextLayer
    s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UBUNTU_REG_42));
    s_text_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UBUNTU_REG_28));
    s_high_low_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UBUNTU_REG_20));
    
    s_time_layer = text_layer_create(GRect(0, 114, 144, 50));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorWhite);
    text_layer_set_text(s_time_layer, "00:00");
    text_layer_set_font(s_time_layer, s_time_font);
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    
    // Create date TextLayer
    s_date_layer = text_layer_create(GRect(0, 95, 144, 50));
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_color(s_date_layer, GColorWhite);
    text_layer_set_text(s_date_layer, "");
    text_layer_set_font(s_date_layer, s_high_low_font);
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
    
    // Create weather TextLayer
    s_weather_layer = text_layer_create(GRect(0, 5, 144, 40));
    text_layer_set_background_color(s_weather_layer, GColorClear);
    text_layer_set_text_color(s_weather_layer, GColorWhite);
    text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
    text_layer_set_text(s_weather_layer, "...");
    text_layer_set_font(s_weather_layer, s_text_font);
    
    // Create high_low TextLayer
    s_high_low_layer = text_layer_create(GRect(0, 15, 144, 40));
    text_layer_set_background_color(s_high_low_layer, GColorClear);
    text_layer_set_text_color(s_high_low_layer, GColorWhite);
    text_layer_set_text_alignment(s_high_low_layer, GTextAlignmentCenter);
    text_layer_set_text(s_high_low_layer, "");
    text_layer_set_font(s_high_low_layer, s_high_low_font);

    
    window_set_background_color(s_main_window, GColorBlack);
    
    // Add children to the main window layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_high_low_layer));
}

static void main_window_unload(Window *window) {
    text_layer_destroy(s_time_layer);
    fonts_unload_custom_font(s_time_font);
    fonts_unload_custom_font(s_text_font);
    fonts_unload_custom_font(s_high_low_font);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
    update_time();
    
    if (tick_time->tm_min % 60 == 0)
    {
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        dict_write_uint8(iter, 0, 0);
        app_message_outbox_send();
    }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context)
{
    Tuple *t = dict_read_first(iterator);
    
    while (t != NULL)
    {
        switch (t->key) {
            case KEY_TEMPERATURE:
                snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", (int)t->value->int32);
                break;
                
            case KEY_CONDITIONS:
                //snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
                break;
                
            case KEY_HIGH_LOW:
                snprintf(high_low_buffer, sizeof(high_low_buffer), "%s", t->value->cstring);
                
            default:
                APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
                break;
        }
        
        t = dict_read_next(iterator);
    }
    
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s", temperature_buffer);
    snprintf(high_low_layer_buffer, sizeof(high_low_layer_buffer), "\n%s", high_low_buffer);
    text_layer_set_text(s_weather_layer, weather_layer_buffer);
    text_layer_set_text(s_high_low_layer, high_low_layer_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
                       
static void draw_horz_line(Layer *this_layer, GContext *ctx)
{
    GPoint p1 = GPoint(0, 100);
    GPoint p2 = GPoint(144, 100);
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_draw_line(ctx, p1, p2);
}

static void init()
{
    // Create main Window element and assign to pointer
    s_main_window = window_create();
    
    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
    
    line_layer = layer_create(GRect(0, 0, 144, 168));
    
    layer_set_update_proc(line_layer, draw_horz_line);
    
    // Show the Window on the watch, with animated=true
    window_stack_push(s_main_window, true);
    
    app_message_register_inbox_received(inbox_received_callback);
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    update_time();
}

static void deinit()
{
    window_destroy(s_main_window);
}

int main(void)
{
    init();
    app_event_loop();
    deinit();
    return 0;
}