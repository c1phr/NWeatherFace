#include <pebble.h>
#define KEY_TEMPERATURE 0
#define KEY_HIGH_LOW 1
#define KEY_CONDITION 2
#define KEY_WINDS 3
#define KEY_WINDD 4
static Window *s_main_window;

static GFont s_time_font;
static GFont s_text_font;

static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_weather_layer;
static TextLayer *s_wind_layer;

static char temperature_buffer[8];
static char high_low_buffer[32];
static char weather_layer_buffer[32];
static char condition_buffer[32];
static char winds_buffer[4];
static char windd_buffer[4];
static char wind_layer_buffer[8];

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

static void set_text_colors(int color)
{
    if (color == 1)
    {
        text_layer_set_text_color(s_date_layer, GColorWhite);
        text_layer_set_text_color(s_time_layer, GColorWhite);
        text_layer_set_text_color(s_weather_layer, GColorWhite);
        text_layer_set_text_color(s_wind_layer, GColorWhite);
    }
    else
    {
        text_layer_set_text_color(s_date_layer, GColorBlack);
        text_layer_set_text_color(s_time_layer, GColorBlack);
        text_layer_set_text_color(s_weather_layer, GColorBlack);
        text_layer_set_text_color(s_wind_layer, GColorBlack);
    }
}

static void set_condition_color(char* condition)
{
    #ifdef PBL_COLOR
        if (strcmp(condition, "sun") == 0)
        {
            window_set_background_color(s_main_window, GColorYellow);
            set_text_colors(0);
        }
        else if (strcmp(condition, "clear") == 0)
        {
            window_set_background_color(s_main_window, GColorCeleste);
            set_text_colors(0);
        }
        else if (strcmp(condition, "mix") == 0)
        {
            window_set_background_color(s_main_window, GColorCyan);
            set_text_colors(0);
        }
        else if (strcmp(condition, "cloud") == 0)
        {
            window_set_background_color(s_main_window, GColorCobaltBlue);
            set_text_colors(1);
        }
        else if (strcmp(condition, "rain") == 0)
        {
            window_set_background_color(s_main_window, GColorBlue);
            set_text_colors(1);
        }
        else if (strcmp(condition, "thunderstorm") == 0)
        {
            window_set_background_color(s_main_window, GColorLightGray);
            set_text_colors(0);
        }
        else if (strcmp(condition, "wind") == 0)
        {
            window_set_background_color(s_main_window, GColorPictonBlue);
            set_text_colors(1);
        }
        else if (strcmp(condition, "snow") == 0)
        {
            window_set_background_color(s_main_window, GColorWhite);
            set_text_colors(0);
        }
        else
        {
            window_set_background_color(s_main_window, GColorDarkCandyAppleRed);
            set_text_colors(1);
        }
    #else
        window_set_background_color(s_main_window, GColorBlack);
    #endif
}

static void main_window_load(Window *window) {
    // Create time TextLayer
    s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UBUNTU_REG_42));
    s_text_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UBUNTU_REG_20));
    
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
    text_layer_set_font(s_date_layer, s_text_font);
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
    
    // Create weather TextLayer
    s_weather_layer = text_layer_create(GRect(0, 5, 144, 40));
    text_layer_set_background_color(s_weather_layer, GColorClear);
    text_layer_set_text_color(s_weather_layer, GColorWhite);
    text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
    text_layer_set_text(s_weather_layer, "");
    text_layer_set_font(s_weather_layer, s_text_font);
    
    // Create wind TextLayer
    s_wind_layer = text_layer_create(GRect(0, 30, 144, 40));
    text_layer_set_background_color(s_wind_layer, GColorClear);
    text_layer_set_text_color(s_wind_layer, GColorWhite);
    text_layer_set_text_alignment(s_wind_layer, GTextAlignmentCenter);
    text_layer_set_text(s_wind_layer, "");
    text_layer_set_font(s_wind_layer, s_text_font);
    
    #ifdef PBL_COLOR
        window_set_background_color(s_main_window, GColorDarkCandyAppleRed);
    #else
        window_set_background_color(s_main_window, GColorBlack);
    #endif
    
    // Add children to the main window layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_wind_layer));
}

static void main_window_unload(Window *window) {
    text_layer_destroy(s_time_layer);
    fonts_unload_custom_font(s_time_font);
    fonts_unload_custom_font(s_text_font);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
    update_time();
    
    if (tick_time->tm_min % 30 == 0)
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
                
            case KEY_HIGH_LOW:
                snprintf(high_low_buffer, sizeof(high_low_buffer), "%s", t->value->cstring);
                break;
            
            case KEY_CONDITION:
                snprintf(condition_buffer, sizeof(condition_buffer), "%s", t->value->cstring);
                set_condition_color(condition_buffer);
                break;
            
            case KEY_WINDS:
                snprintf(winds_buffer, sizeof(winds_buffer), "%s", t->value->cstring);
                break;
            
            case KEY_WINDD:
                snprintf(windd_buffer, sizeof(windd_buffer), "%s", t->value->cstring);
                break;
                
            default:
                APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
                break;
        }
        
        t = dict_read_next(iterator);
    }
    snprintf(wind_layer_buffer, sizeof(wind_layer_buffer), "%s %s", winds_buffer, windd_buffer);
    text_layer_set_text(s_wind_layer, wind_layer_buffer);
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s | %s", temperature_buffer, high_low_buffer);
    text_layer_set_text(s_weather_layer, weather_layer_buffer);
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

static void init()
{
    // Create main Window element and assign to pointer
    s_main_window = window_create();
    
    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
    
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