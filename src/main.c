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
  bitmap_year,
  bitmap_y0,
  bitmap_y1,
  bitmap_y2,
  bitmap_y3,
  bitmap_y4,
  bitmap_y5,
  bitmap_y6,
  bitmap_y7,
  bitmap_y8,
  bitmap_y9,
  bitmap_y10,
  bitmap_y11,
  bitmap_y12,
  bitmaps_length
};

enum PNGOverlayBitmaps {
  overlay_bitmap_plate = 0,
  overlay_bitmap_d1,
  overlay_bitmap_d2,
  overlay_bitmap_d3,
  overlay_bitmap_d4,
  overlay_bitmap_d5,
  overlay_bitmap_d6,
  overlay_bitmap_d7,
  overlay_bitmap_d8,
  overlay_bitmap_d9,
  overlay_bitmap_d10,
  overlay_bitmap_d11,
  overlay_bitmap_d12,
  overlay_bitmap_d13,
  overlay_bitmap_d14,
  overlay_bitmap_d15,
  overlay_bitmap_d16,
  overlay_bitmap_d17,
  overlay_bitmap_d18,
  overlay_bitmap_d19,
  overlay_bitmap_d20,
  overlay_bitmap_d21,
  overlay_bitmap_d22,
  overlay_bitmap_d23,
  overlay_bitmap_d24,
  overlay_bitmap_d25,
  overlay_bitmap_d26,
  overlay_bitmap_d27,
  overlay_bitmap_d28,
  overlay_bitmap_d29,
  overlay_bitmap_d30,
  overlay_bitmap_d31,
  overlay_bitmaps_length
};

static Window *window;
static Layer *image_layer;
static GBitmap *bitmaps[bitmaps_length];
static GBitmap *overlay_bitmaps[overlay_bitmaps_length];
static struct tm *t;
static const GPoint round_origin = {19, 12};

static int8_t get_added_year(int16_t year) {
  int8_t added;

  if(year % 400 == 0) added = 1;
  else if(year % 100 == 0) added = 0;
  else if(year % 4 == 0) added = 1;
  else added = 0;

  return added;
}

static int8_t get_last_day(int16_t year, int8_t month) {
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
  t->tm_mon--;

  if (t->tm_mon < 0) {
    t->tm_mon = 11;
    t->tm_year--;
  }
  time_t prev = mktime(t);

  t = localtime(&prev);

  layer_mark_dirty(image_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  t->tm_mon++;

  if (t->tm_mon > 11) {
    t->tm_mon = 0;
    t->tm_year++;
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
  // draw year and month
  GPoint header = { 20, 0 };

#if defined(PBL_ROUND)
  // set layer position
  header.x = header.x + round_origin.x;
  header.y = header.y + round_origin.y;
#endif


  char year[5];
  int8_t char_year, char_month;
  snprintf(year, sizeof(year), "%d", t->tm_year + 1900);

  char_month = bitmap_y1 + t->tm_mon;

  if (t->tm_mon + 1 > 9) {
    header.x -= 2;

    for (int8_t i = 0; i < 4; ++i) {
      char_year = bitmap_y0 + (int)year[i] - 48;
      graphics_draw_bitmap_in_rect(ctx, bitmaps[char_year],
                                   (GRect) {
                                       .origin = {(int16_t) (header.x + i * 12), (int16_t) (header.y + 9)},
                                       .size = { 12, 17 }
                                   });
    }
    graphics_draw_bitmap_in_rect(ctx, bitmaps[bitmap_year],
                                 (GRect) {
                                     .origin = {(int16_t) (header.x + 50), (int16_t) (header.y + 8)},
                                     .size = { 15, 18 }
                                 });

    graphics_draw_bitmap_in_rect(ctx, bitmaps[char_month],
                                 (GRect) {
                                     .origin = {(int16_t) (header.x + 70), (int16_t) (header.y + 9)},
                                     .size = { 18, 17 }
                                 });
    graphics_draw_bitmap_in_rect(ctx, bitmaps[bitmap_w2],
                                 (GRect) {
                                     .origin = {(int16_t) (header.x + 92), (int16_t) (header.y + 8)},
                                     .size = { 15, 18 }
                                 });
  } else {
    for (int8_t i = 0; i < 4; ++i) {
      char_year = bitmap_y0 + (int)year[i] - 48;
      graphics_draw_bitmap_in_rect(ctx, bitmaps[char_year],
                                   (GRect) {
                                       .origin = {(int16_t) (header.x + i * 12), (int16_t) (header.y + 9)},
                                       .size = { 12, 17 }
                                   });
    }
    graphics_draw_bitmap_in_rect(ctx, bitmaps[bitmap_year],
                                 (GRect) {
                                     .origin = {(int16_t) (header.x + 50), (int16_t) (header.y + 8)},
                                     .size = { 15, 18 }
                                 });

    graphics_draw_bitmap_in_rect(ctx, bitmaps[char_month],
                                 (GRect) {
                                     .origin = {(int16_t) (header.x + 72), (int16_t) (header.y + 9)},
                                     .size = { 12, 17 }
                                 });
    graphics_draw_bitmap_in_rect(ctx, bitmaps[bitmap_w2],
                                 (GRect) {
                                     .origin = {(int16_t) (header.x + 87), (int16_t) (header.y + 8)},
                                     .size = { 15, 18 }
                                 });
  }

  // draw header
  GPoint calendar = { -14, 40 };

#if defined(PBL_ROUND)
  // set layer position
  calendar.x = calendar.x + round_origin.x;
  calendar.y = calendar.y + round_origin.y;
#endif

  const int8_t space_h = 19, space_v = 18;

  for (int8_t j = 0; j < 7; ++j) {
    graphics_draw_bitmap_in_rect(ctx, bitmaps[j + 32],
                                 (GRect) {
                                     .origin = {(int16_t) ((calendar.x + 19) + (space_h * j)),
                                                (int16_t) (calendar.y - 4)},
                                     .size = { 15, 18 }
                                 });
  }

  // draw days
  const int8_t today = (const int8_t) t->tm_mday;

  int8_t count_week = (int8_t) (t->tm_wday - (t->tm_mday % 7));
  count_week = (int8_t) (count_week < -1 ? count_week + 7 : count_week);

  int8_t reset_week = (int8_t) (count_week + 2);
  int8_t vertical_return = 1;
  int8_t last_day = get_last_day((int16_t) (t->tm_year), (int8_t) (t->tm_mon + 1));

  for (int8_t i = 1; i <= last_day; ++i) {
    if (i == today) {
      graphics_draw_bitmap_in_rect(ctx, overlay_bitmaps[i],
                                   (GRect) {
                                       .origin = {reset_week * space_h + calendar.x, vertical_return * space_v + calendar.y},
                                       .size = {18, 14}
                                   });
    } else {
      graphics_draw_bitmap_in_rect(ctx, bitmaps[i],
                                   (GRect) {
                                       .origin = {reset_week * space_h + calendar.x, vertical_return * space_v + calendar.y},
                                       .size = {18, 14}
                                   });
    }

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
  window_set_background_color(window, GColorBlack);

  time_t now = time(NULL);
  t = localtime(&now);

  Layer *window_layer = window_get_root_layer(window);

  uint8_t bitmap_resource_id = (uint8_t) RESOURCE_ID_DAY_1;
  uint8_t overlay_bitmap_resource_id = (uint8_t) RESOURCE_ID_O_DAY_1;

  for (uint8_t i = 1; i < bitmaps_length; ++i) {
    bitmaps[i] = gbitmap_create_with_resource((uint8_t) (bitmap_resource_id + i - 1));
  }

  for (uint8_t i = 1; i < overlay_bitmaps_length; ++i) {
    overlay_bitmaps[i] = gbitmap_create_with_resource((uint8_t) (overlay_bitmap_resource_id + i - 1));
  }

  load_layers(window_layer);
}

static void window_unload(Window *window) {
  unload_layers();

  for (uint8_t i = 0; i < bitmaps_length; ++i) {
    if (bitmaps[i]) gbitmap_destroy(bitmaps[i]);
  }

  for (uint8_t i = 0; i < overlay_bitmaps_length; ++i) {
    if (overlay_bitmaps[i]) gbitmap_destroy(overlay_bitmaps[i]);
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
