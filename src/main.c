/*

  Polar Clock Lite watch (SDK 2.0)

  Thanks to Team Pebble's Segment Six watchface...it was a big help!

 */

#include "pebble.h"

/* 
#define SHOW_TEXT_TIME 1
#define SHOW_TEXT_DATE 1
#define ROW_DATE 0
*/
	
enum {
        KEY_TIME = 0x0,
        KEY_DATE = 0x1,
        KEY_ROW = 0x2,
		KEY_INVERT = 0x3
};

static bool SHOW_TEXT_TIME = true;
static bool SHOW_TEXT_DATE = true;
static bool ROW_DATE = false;
static bool INVERT = false;

/*
#define BACKGROUND_COLOR GColorBlack
#define FOREGROUND_COLOR GColorWhite
*/

static GColor BACKGROUND_COLOR = GColorBlack;
static GColor FOREGROUND_COLOR = GColorWhite;

static char time_text[] = "00:00";
static char date_text[] = "00 Xxx";
static char date_row_text[] = "00 Xxx";

Window *window;

Layer *minute_display_layer;
Layer *hour_display_layer;

TextLayer *text_time_layer;
TextLayer *text_date_layer;
bool time_layer_exists = false;
bool date_layer_exists = false;

const GPathInfo MINUTE_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-7, -70}, // 70 = radius + fudge; 7 = 70*tan(6 degrees); 6 degrees per minute;
    {7,  -70},
  }
};

static GPath *minute_segment_path;


const GPathInfo HOUR_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-6, -58}, // 58 = radius + fudge; 6 = 58*tan(6 degrees); 30 degrees per hour;
    {6,  -58},
  }
};

static GPath *hour_segment_path;


static void minute_display_layer_update_callback(Layer *layer, GContext* ctx) {

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  unsigned int angle = t->tm_min * 6;

  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  graphics_context_set_fill_color(ctx, FOREGROUND_COLOR);

  graphics_fill_circle(ctx, center, 65);

  graphics_context_set_fill_color(ctx, BACKGROUND_COLOR);

  for(; angle < 355; angle += 6) {

    gpath_rotate_to(minute_segment_path, (TRIG_MAX_ANGLE / 360) * angle);

    gpath_draw_filled(ctx, minute_segment_path);

  }

  graphics_fill_circle(ctx, center, 60);

}


static void hour_display_layer_update_callback(Layer *layer, GContext* ctx) {

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  unsigned int angle = (( t->tm_hour % 12 ) * 30) + (t->tm_min / 2);
	
  angle = angle - (angle % 6);

  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  graphics_context_set_fill_color(ctx, FOREGROUND_COLOR);

  graphics_fill_circle(ctx, center, 55);

  graphics_context_set_fill_color(ctx, BACKGROUND_COLOR);

  for(; angle < 355; angle += 6) {

    gpath_rotate_to(hour_segment_path, (TRIG_MAX_ANGLE / 360) * angle);

    gpath_draw_filled(ctx, hour_segment_path);

  }

  graphics_fill_circle(ctx, center, 50);

}


static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  //(void)ctx;
	
  layer_mark_dirty(minute_display_layer);
  layer_mark_dirty(hour_display_layer);

  if (SHOW_TEXT_TIME && time_layer_exists)
  {
	  // Need to be static because it's used by the system later.
	  //static char time_text[] = "00:00";
	
	  char *time_format;
	
	  if (clock_is_24h_style()) {
		time_format = "%R";
	  } else {
		time_format = "%I:%M";
	  }
	
	  strftime(time_text, sizeof(time_text), time_format, tick_time);
	
	  text_layer_set_text(text_time_layer, time_text);
  }

  if (SHOW_TEXT_DATE && date_layer_exists)
  {
	  //static char date_text[] = "00 Xxx";
	  if (ROW_DATE)
	  {
		  //date_text = "xx Xxx";
	  	  strftime(date_row_text, sizeof(date_row_text), "%d %b", tick_time);
		  strftime(date_text, sizeof(date_text), "%b %d", tick_time);
		  text_layer_set_text(text_date_layer, date_row_text);
	  } else {
		  //date_text = "Xxx xx";
		  strftime(date_row_text, sizeof(date_row_text), "%d %b", tick_time);
	  	  strftime(date_text, sizeof(date_text), "%b %d", tick_time);
		  text_layer_set_text(text_date_layer, date_text);
	  }
	  //text_layer_set_text(text_date_layer, date_text);
  }
}

static void setup_time_date_layers() {
	
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
	
  if(time_layer_exists) {
	  text_layer_destroy(text_time_layer);
	  time_layer_exists = false;	
  }
  
  if(date_layer_exists) {
	  text_layer_destroy(text_date_layer);
	  date_layer_exists = false;		
  }
	
  if (SHOW_TEXT_TIME)
  {
	  text_time_layer = text_layer_create(bounds);
	  time_layer_exists = true;
	  text_layer_set_text_color(text_time_layer, FOREGROUND_COLOR);
	  text_layer_set_background_color(text_time_layer, GColorClear);
	  text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
	  
	  text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));
	  	
	  if (SHOW_TEXT_DATE)
	  {
	  	layer_set_frame(text_layer_get_layer(text_time_layer), GRect(0, 57, 144, 168-57));
	  } else {
	  	layer_set_frame(text_layer_get_layer(text_time_layer), GRect(0, 70, 144, 168-70));
	  }
	  
  }

  if (SHOW_TEXT_DATE)
  {
	  text_date_layer = text_layer_create(bounds);
	  date_layer_exists = true;
	  text_layer_set_text_color(text_date_layer, FOREGROUND_COLOR);
	  text_layer_set_background_color(text_date_layer, GColorClear);
	  text_layer_set_text_alignment(text_date_layer, GTextAlignmentCenter);
	  
	  text_layer_set_font(text_date_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	  layer_add_child(window_layer, text_layer_get_layer(text_date_layer));
	  
	  if (SHOW_TEXT_TIME)
	  {
		  layer_set_frame(text_layer_get_layer(text_date_layer), GRect(0, 80, 144, 168-80));
	  } else {
		  layer_set_frame(text_layer_get_layer(text_date_layer), GRect(0, 70, 144, 168-70));
	  }
	  
  }
}

 void in_received_handler(DictionaryIterator *received, void *context) {
	 Tuple *time_tuple = dict_find(received, KEY_TIME);
	 Tuple *date_tuple = dict_find(received, KEY_DATE);
	 Tuple *row_tuple = dict_find(received, KEY_ROW);
	 Tuple *invert_tuple = dict_find(received, KEY_INVERT);
	 
	 //APP_LOG(APP_LOG_LEVEL_DEBUG, "KEY_TIME: %d, %d, %d", time_tuple->type, time_tuple->length, time_tuple->value->uint8 );
	 //APP_LOG(APP_LOG_LEVEL_DEBUG, "KEY_DATE: %d, %d, %d", date_tuple->type, date_tuple->length, date_tuple->value->int8 );
	 //APP_LOG(APP_LOG_LEVEL_DEBUG, "KEY_ROW: %d, %d, %d", row_tuple->type, row_tuple->length, row_tuple->value->int8 );
	 //APP_LOG(APP_LOG_LEVEL_DEBUG, "KEY_INVERT: %d, %d, %d", invert_tuple->type, invert_tuple->length, invert_tuple->value->uint8 );
	 
	 SHOW_TEXT_TIME = time_tuple->value->int8 ? true : false;
	 persist_write_bool(KEY_TIME, SHOW_TEXT_TIME);
	 SHOW_TEXT_DATE = date_tuple->value->int8 ? true : false;
	 persist_write_bool(KEY_DATE, SHOW_TEXT_DATE);
	 ROW_DATE = row_tuple->value->int8 ? true : false;
	 persist_write_bool(KEY_ROW, ROW_DATE);
	 INVERT = invert_tuple->value->int8 ? true : false;
	 persist_write_bool(KEY_INVERT, INVERT);
	 if(INVERT)
	 {
		BACKGROUND_COLOR = GColorWhite;
		FOREGROUND_COLOR = GColorBlack;
	 }
	 else
	 {
		BACKGROUND_COLOR = GColorBlack;
		FOREGROUND_COLOR = GColorWhite;		 		 
	 }
	 window_set_background_color(window, BACKGROUND_COLOR);
	 
	 setup_time_date_layers();
	 
	 if (SHOW_TEXT_TIME && time_layer_exists) text_layer_set_text(text_time_layer, time_text);
	 if (SHOW_TEXT_DATE && date_layer_exists)
	 {
		 if (ROW_DATE)
		 {
		  text_layer_set_text(text_date_layer, date_row_text);
		 } else {
		  text_layer_set_text(text_date_layer, date_text);
		 }
	 }
 }


 void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
 }


static void init(void) {
  //(void)ctx;
	
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_open(64, 0);

  if(persist_exists(KEY_TIME)) SHOW_TEXT_TIME = persist_read_bool(KEY_TIME);
  if(persist_exists(KEY_DATE)) SHOW_TEXT_DATE = persist_read_bool(KEY_DATE);
  if(persist_exists(KEY_ROW)) ROW_DATE = persist_read_bool(KEY_ROW);
  if(persist_exists(KEY_INVERT)) INVERT = persist_read_bool(KEY_INVERT);
	
  if(INVERT) {
	  BACKGROUND_COLOR = GColorWhite;
	  FOREGROUND_COLOR = GColorBlack;
  }
  else {
	  BACKGROUND_COLOR = GColorBlack;
	  FOREGROUND_COLOR = GColorWhite;		 		 
  }
	
  window = window_create();
  window_set_background_color(window, BACKGROUND_COLOR);
  window_stack_push(window, true);

  // No longer needed for SDK 2.0
  //resource_init_current_app(&APP_RESOURCES);
	
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
	
  // Init the layer for the minute display
  minute_display_layer = layer_create(bounds);
  layer_set_update_proc(minute_display_layer, minute_display_layer_update_callback);
  layer_add_child(window_layer, minute_display_layer);

  // Init the minute segment path
  minute_segment_path = gpath_create(&MINUTE_SEGMENT_PATH_POINTS);
  gpath_move_to(minute_segment_path, grect_center_point(&bounds));

  // Init the layer for the hour display
  hour_display_layer = layer_create(bounds);
  layer_set_update_proc(hour_display_layer, hour_display_layer_update_callback);
  layer_add_child(window_layer, hour_display_layer);

  // Init the hour segment path
  hour_segment_path = gpath_create(&HOUR_SEGMENT_PATH_POINTS);
  gpath_move_to(hour_segment_path, grect_center_point(&bounds));

  setup_time_date_layers();
	
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void deinit(void) {
  gpath_destroy(minute_segment_path);
  gpath_destroy(hour_segment_path);

  tick_timer_service_unsubscribe();
  window_destroy(window);
  layer_destroy(minute_display_layer);
  layer_destroy(hour_display_layer);
	
  if (time_layer_exists) text_layer_destroy(text_time_layer);
  if (date_layer_exists) text_layer_destroy(text_date_layer);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
