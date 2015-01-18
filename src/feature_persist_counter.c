#include "pebble.h"

// This is a custom defined key for saving our count field
#define NUM_DRINKS_PKEY 0
#define TOTAL_DRINKS_PKEY 1
#define CURRENT_DAY_PKEY 2
#define STREAK_PKEY 3
#define BEST_STREAK_PKEY 4

// You can define defaults for values in persistent storage
#define NUM_DRINKS_DEFAULT 0
#define TOTAL_DRINKS_DEFAULT 0
#define CURRENT_DAY_DEFAULT 0
#define STREAK_DEFAULT 0
#define BEST_STREAK_DEFAULT 0
  
#define WAKEUP_REASON 5
#define PERSIST_KEY_WAKEUP_ID 42

static Window *window;
static Window *profile;

static GBitmap *action_icon_plus;
static GBitmap *action_icon_minus;
static GBitmap *empty_cup;
static GBitmap *fill_cup_1;
static GBitmap *fill_cup_2;
static GBitmap *fill_cup_3;
static GBitmap *fill_cup_4;
static GBitmap *fill_cup_5;
static GBitmap *fill_cup_6;
static GBitmap *fill_cup_7;
static GBitmap *fill_cup_8;
static GBitmap *toilet_cup;

static GBitmap *choice;

static GBitmap *select_button;

static ActionBarLayer *action_bar;
static TextLayer *header_text_layer;
static TextLayer *body_text_layer;
static TextLayer *label_text_layer;
static BitmapLayer *cup_image_layer;

static TextLayer *profile_layer;
static TextLayer *profile_stats_layer;

// We'll save the count in memory from persistent storage
static int num_drinks = NUM_DRINKS_DEFAULT;
static int total_drinks = TOTAL_DRINKS_DEFAULT;
static int weekday = CURRENT_DAY_DEFAULT;
static int streak = STREAK_DEFAULT;
static int best_streak = BEST_STREAK_DEFAULT;

static WakeupId s_wakeup_id;

static void wakeup_handler(WakeupId id, int32_t reason) {
  // The app has woken!
  text_layer_set_text(label_text_layer, "Drink more water!");
  vibes_short_pulse();
  // Delete the ID
  persist_delete(PERSIST_KEY_WAKEUP_ID);
}
static void update_nextNotification() {
  wakeup_cancel_all();
  // Get a tm structure
  time_t future_time = time(NULL) + 10;
  //struct tm *tick_time = localtime(&temp);

  //(*tick_time).tm_hour = tick_time->tm_hour + 3;
  
  // Current time + 30 seconds

  // Schedule wakeup event and keep the WakeupId
  s_wakeup_id = wakeup_schedule(future_time, WAKEUP_REASON, true); 
  persist_write_int(PERSIST_KEY_WAKEUP_ID, s_wakeup_id);

  // Prepare for waking up later
  //text_layer_set_text(label_text_layer, "This app will now wake up in 30 seconds.\n\nClose me!");
}

static void update_text() {
  static char body_text[30];
  snprintf(body_text, sizeof(body_text), "%u Servings:", num_drinks);
  text_layer_set_text(body_text_layer, body_text);
}

static void update_stats_text() {
  static char body_text[110];
  //aho fix
  snprintf(body_text, sizeof(body_text), "Total Drinks: %u \n Streak: %u \n Best Streak: %u \n Built with love by \n A Hoang and E Tian \n PennApps w2015", total_drinks, streak, best_streak);
  text_layer_set_text(profile_stats_layer, body_text);
}

static void update_BMP(){
  
  //choose cup position:
  switch(num_drinks){
    case 0:
       choice = empty_cup;
       break;
    case 1:
       choice = fill_cup_1;
       break;
    case 2:
       choice = fill_cup_2;
       break;
    case 3:
       choice = fill_cup_3;
       break;
    case 4:
       choice = fill_cup_4;
       break;
    case 5:
       choice = fill_cup_5;
       break;
    case 6:
       choice = fill_cup_6;
       break;
    case 7:
      choice = fill_cup_7;
      break;
    case 8:
      choice = fill_cup_8;
      break;
    case 9:
      choice = fill_cup_8;
      break;
    case 10:
      choice = fill_cup_8;
      break;
    case 11:
      choice = fill_cup_8;
      break;
    case 12:
      choice = fill_cup_8;
      break;
    case 13:
      choice = fill_cup_8;
      break;
    case 14:
      choice = fill_cup_8;
      break;
    default:
      choice = toilet_cup;
      break;
  }
  bitmap_layer_set_bitmap(cup_image_layer, choice);	
}

// static void determine_achievements() {
//   switch(best_streak){
//     case 1:

//        break;
//     case 2:
//        //1/8
//        break;
//     case 7:
//        //2/8
//        break;
//     case 14:
//        //3/8
//        break;
//     case 30:
//        //4/8
//        break;
//     case 180:
//        //5/8
//        break;
//     case 356:
//        //6/8
//        break;
//   }
// }
// static void determine_total_achievements() {
//   switch(total_drinks){
//     case 1:
//        vibes_short_pulse();
       
//        break;
//     case 8:
//        vibes_short_pulse();
//        break;
//     case 25:
//        vibes_short_pulse();
//        break;
//     case 50:
//        vibes_short_pulse();
//        break;
//     case 100:
//        vibes_short_pulse();
//        break;
//     case 500:
//        vibes_short_pulse();
//        break;
//     case 1000:
//       vibes_short_pulse();
//       break;
//     case 5000:
//        vibes_short_pulse();
//        break;
//   }
// }

static void update_counter() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  int thisWeekday = tick_time->tm_wday;
  if(weekday != thisWeekday){
    weekday = thisWeekday;
    if(num_drinks >= 8){
      streak = streak + 1;
      if(streak>best_streak){
        best_streak = best_streak + 1;
//        determine_achievements();
      }
    }
    else if(num_drinks == 0){
      streak = 0;
    }
    num_drinks = 0;
    update_text();
  }
}

static void increment_click_handler(ClickRecognizerRef recognizer, void *context) {
  num_drinks++;
  total_drinks++;
  update_text();
  update_BMP();
//  determine_total_achievements();
  update_nextNotification();
}

static void decrement_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (num_drinks <= 0) {
    // Keep the counter at zero
    return;
  }

  num_drinks--;
  total_drinks--;
  update_text();
  update_BMP();
}

//use select button for something
static void select_click_handler(ClickRecognizerRef recognizer, void *context){
  //vibes_short_pulse();
  window_stack_push(profile, true);
  update_stats_text();
}

static void click_config_provider(void *context) {
  const uint16_t repeat_interval_ms = 50;
  window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, (ClickHandler) increment_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, (ClickHandler) decrement_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *me) {
  action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(action_bar, me);
  action_bar_layer_set_click_config_provider(action_bar, click_config_provider);

  action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, action_icon_plus);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, action_icon_minus);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, select_button);

  Layer *layer = window_get_root_layer(me);
  const int16_t width = layer_get_frame(layer).size.w - ACTION_BAR_WIDTH - 3;

  header_text_layer = text_layer_create(GRect(4, 0, width, 36));
  text_layer_set_font(header_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_background_color(header_text_layer, GColorClear);
  text_layer_set_text(header_text_layer, "Drinks today:");
  layer_add_child(layer, text_layer_get_layer(header_text_layer));

  body_text_layer = text_layer_create(GRect(4, 100, width, 36));
  text_layer_set_font(body_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_background_color(body_text_layer, GColorClear);
  layer_add_child(layer, text_layer_get_layer(body_text_layer));

  label_text_layer = text_layer_create(GRect(4, 124, width, 24));
  text_layer_set_font(label_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_background_color(label_text_layer, GColorClear);
  text_layer_set_text(label_text_layer, "Each serving is 8oz");
  layer_add_child(layer, text_layer_get_layer(label_text_layer));
  
  
  cup_image_layer = bitmap_layer_create(GRect(4,30, width,80));
  bitmap_layer_set_bitmap(cup_image_layer, choice);
  bitmap_layer_set_compositing_mode(cup_image_layer, GCompOpAssign);
  bitmap_layer_set_alignment(cup_image_layer, GAlignCenter);
  layer_add_child(layer, bitmap_layer_get_layer(cup_image_layer));

  update_text();
  update_BMP();
}

static void window_unload(Window *window) {
  text_layer_destroy(header_text_layer);
  text_layer_destroy(body_text_layer);
  text_layer_destroy(label_text_layer);
  action_bar_layer_destroy(action_bar);
  bitmap_layer_destroy(cup_image_layer);

}

static void profile_load(Window *window) {
  Layer *window_layer = window_get_root_layer(profile); 
  GRect bounds = layer_get_bounds(window_layer);
  
  profile_layer = text_layer_create((GRect) { .origin = { 4, 0 }, .size = { bounds.size.w, 30 } });
  text_layer_set_text(profile_layer, "Your Reservoir:");
  text_layer_set_font(profile_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_background_color(profile_layer, GColorClear);
  text_layer_set_text_alignment(profile_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(profile_layer));
  
  profile_stats_layer = text_layer_create((GRect) { .origin = { 4, 30 }, .size = { bounds.size.w, 110 } });
  text_layer_set_font(profile_stats_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_background_color(profile_stats_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(profile_stats_layer));
  
}

static void profile_unload(Window *window){
  text_layer_destroy(profile_layer);
  text_layer_destroy(profile_stats_layer);
}

static void init(void) {
  action_icon_plus = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_PLUS);
  action_icon_minus = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_MINUS);
  select_button = gbitmap_create_with_resource(RESOURCE_ID_SELECT);
  empty_cup = gbitmap_create_with_resource(RESOURCE_ID_CUP);
  fill_cup_1 = gbitmap_create_with_resource(RESOURCE_ID_CUP1);
  fill_cup_2 = gbitmap_create_with_resource(RESOURCE_ID_CUP2);
  fill_cup_3 = gbitmap_create_with_resource(RESOURCE_ID_CUP3);
  fill_cup_4 = gbitmap_create_with_resource(RESOURCE_ID_CUP4);
  fill_cup_5 = gbitmap_create_with_resource(RESOURCE_ID_CUP5);
  fill_cup_6 = gbitmap_create_with_resource(RESOURCE_ID_CUP6);
  fill_cup_7 = gbitmap_create_with_resource(RESOURCE_ID_CUP7);
  fill_cup_8 = gbitmap_create_with_resource(RESOURCE_ID_CUP8);
  toilet_cup = gbitmap_create_with_resource(RESOURCE_ID_TOILET_CUP);
  
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  
  //other window
  profile = window_create();
  window_set_window_handlers(profile, (WindowHandlers) {
  .load = profile_load,
  .unload = profile_unload,
  });

  // Get the count from persistent storage for use if it exists, otherwise use the default
  num_drinks = persist_exists(NUM_DRINKS_PKEY) ? persist_read_int(NUM_DRINKS_PKEY) : NUM_DRINKS_DEFAULT;
  total_drinks = persist_exists(TOTAL_DRINKS_PKEY) ? persist_read_int(TOTAL_DRINKS_PKEY) : TOTAL_DRINKS_DEFAULT;
  weekday = persist_exists(CURRENT_DAY_PKEY) ? persist_read_int(CURRENT_DAY_PKEY) : CURRENT_DAY_DEFAULT;
  streak = persist_exists(STREAK_PKEY) ? persist_read_int(STREAK_PKEY) : STREAK_DEFAULT;
  best_streak = persist_exists(BEST_STREAK_PKEY) ? persist_read_int(BEST_STREAK_PKEY) : BEST_STREAK_DEFAULT;
  window_stack_push(window, true /* Animated */);
  update_counter();
  
  // Subscribe to Wakeup API
  wakeup_service_subscribe(wakeup_handler);

  // Was this a wakeup launch?
  if (launch_reason() == APP_LAUNCH_WAKEUP) {
    // The app was started by a wakeup
    WakeupId id = 0;
    int32_t reason = 0;

    // Get details and handle the wakeup
    wakeup_get_launch_event(&id, &reason);
    wakeup_handler(id, reason);
  }
}

static void deinit(void) {
  // Save the count into persistent storage on app exit
  persist_write_int(NUM_DRINKS_PKEY, num_drinks);
  persist_write_int(TOTAL_DRINKS_PKEY, total_drinks);
  persist_write_int(CURRENT_DAY_PKEY, weekday);
  persist_write_int(STREAK_PKEY, streak);
  persist_write_int(BEST_STREAK_PKEY, best_streak);

  window_destroy(window);
  window_destroy(profile);

  gbitmap_destroy(action_icon_plus);
  gbitmap_destroy(action_icon_minus);
  gbitmap_destroy(select_button);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}