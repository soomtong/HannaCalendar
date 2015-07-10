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
  bitmap_w1,
  bitmap_w2,
  bitmap_w3,
  bitmap_w4,
  bitmap_w5,
  bitmap_w6,
  bitmap_w7,
  bitmaps_length
};

static Window *window;
static Layer *image_layer;
static GBitmap *bitmaps[bitmaps_length];
static struct tm *t;

static int8_t get_added_year(int8_t year) {
  int8_t added;

  if(year % 400 == 0) added = 1;
  else if(year % 100 == 0) added = 0;
  else if(year % 4 == 0) added = 1;
  else added = 0;

  return added;
}

static int8_t get_last_day(int8_t year, int8_t month) {
  int8_t last;

  switch(month) {
    case 2:
      last = (int8_t) (get_added_year(year) == 0 ? 28 : 29);
      break;

    case 4: case 6: case 9: case 11:
      last = 30;
      break;

    default:
      last = 31;
  }

  return last;
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  time_t now = time(NULL);
  t = localtime(&now);

  layer_mark_dirty(image_layer);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  (*t).tm_mon--;

  if ((*t).tm_mon < 0) {
    (*t).tm_mon = 11;
    (*t).tm_year--;
  }
  time_t prev = mktime(t);

  t = localtime(&prev);

  layer_mark_dirty(image_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  (*t).tm_mon++;

  if ((*t).tm_mon > 11) {
    (*t).tm_mon = 0;
    (*t).tm_year++;
  }
  time_t next = mktime(t);

  t = localtime(&next);

  layer_mark_dirty(image_layer);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void draw_calendar(Layer *layer, GContext* ctx) {

  const int8_t start_h = -14, start_v = 40;
  const int8_t space_h = 19, space_v = 18;

  // draw header
  const int8_t size_weeks_h = 16, size_weeks_v = 18;

  for (int8_t j = 0; j < 7; ++j) {
    graphics_draw_bitmap_in_rect(ctx, bitmaps[j + 32],
                                 (GRect) {
                                     .origin = {(int16_t) ((start_h + 19) + (space_h * j)), start_v - 2 },
                                     .size = { size_weeks_h, size_weeks_v }
                                 });
  }

  // draw days
  const int8_t size_days_h = 18, size_days_v = 15;

  int8_t count_week = (int8_t) ((*t).tm_wday - ((*t).tm_mday % 7));
  count_week = (int8_t) (count_week < -1 ? count_week + 7 : count_week);

  int8_t reset_week = (int8_t) (count_week + 2);
  int8_t vertical_return = 1;
  int8_t last_day = get_last_day((int8_t) ((*t).tm_year), (int8_t) ((*t).tm_mon + 1));

  for (int8_t i = 1; i <= last_day; ++i) {
    graphics_draw_bitmap_in_rect(ctx, bitmaps[i],
                                 (GRect) {
                                     .origin = { reset_week * space_h + start_h, vertical_return * space_v + start_v },
                                     .size = {size_days_h, size_days_v}
                                 });

    reset_week++;

    if (reset_week > 7) {
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
  t = localtime(&now);

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

  app_event_loop();
  deinit();
}
