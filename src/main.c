#include <pebble.h>

enum PNGBitmaps {
  bitmap_plate = 0,
  bitmap_d1,
  bitmap_d2,
  bitmap_d3,
  bitmap_d4,
  bitmap_d5,
  bitmap_d6,
  bitmap_d7,
  bitmap_d8,
  bitmap_d9,
  bitmap_d10,
  bitmap_d11,
  bitmap_d12,
  bitmap_d13,
  bitmap_d14,
  bitmap_d15,
  bitmap_d16,
  bitmap_d17,
  bitmap_d18,
  bitmap_d19,
  bitmap_d20,
  bitmap_d21,
  bitmap_d22,
  bitmap_d23,
  bitmap_d24,
  bitmap_d25,
  bitmap_d26,
  bitmap_d27,
  bitmap_d28,
  bitmap_d29,
  bitmap_d30,
  bitmap_d31,
  bitmaps_length
};

static Window *window;
static Layer *image_layer;
static GBitmap *bitmaps[bitmaps_length];
static struct tm *t;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
//  text_layer_set_text(text_layer, "Select");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "year  : %d", (*t).tm_year);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "month : %d", (*t).tm_mon + 1);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "day : %d", (*t).tm_mday);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "week : %d", (*t).tm_wday);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
//  text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
//  text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void draw_calendar(Layer *layer, GContext* ctx) {
  const int8_t start_h = -10, start_v = 18;
  const int8_t size_h = 18, size_v = 15;
  const int8_t space_h = 21, space_v = 20;

  int8_t reset_week = 5, vertical_return = 1;
  int8_t last_day = 31;

  for (int i = 1; i <= last_day; ++i) {
    graphics_draw_bitmap_in_rect(ctx, bitmaps[i],
                                 (GRect) {
                                     .origin = { reset_week * space_h + start_h, vertical_return * space_v + start_v },
                                     .size = { size_h, size_v }
                                 });

    reset_week++;

    if (reset_week == 7) {
      vertical_return++;
      reset_week = 1;
    }
  }
}

static void load_layers(Layer *root_layer) {
  GRect bounds = layer_get_bounds(root_layer);

  image_layer = layer_create(bounds);
  layer_add_child(root_layer, image_layer);

  layer_set_update_proc(image_layer, draw_calendar);
}

static void unload_layers() {
  layer_destroy(image_layer);
}

static void window_load(Window *window) {
  window_set_background_color(window, GColorDarkGray);

  time_t now = time(NULL);
  t = localtime(&now);  //todo: fix memory leaks

  Layer *window_layer = window_get_root_layer(window);

  uint8_t resource_id = (uint8_t) RESOURCE_ID_DAY_1;

  for (uint8_t i = 1; i < bitmaps_length; ++i) {
    bitmaps[i] = gbitmap_create_with_resource((uint8_t) (resource_id + i - 1));
  }

  load_layers(window_layer);
}

static void window_unload(Window *window) {
  unload_layers();

  for (uint8_t i = 0; i < bitmaps_length; ++i) {
    if (bitmaps[i]) gbitmap_destroy(bitmaps[i]);
  }
}

static void init(void) {
  window = window_create();

  window_set_click_config_provider(window, click_config_provider);

  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
