#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "math.h"

#define CENTER_RADIUS 20
#define PUPIL_RADIUS  10
#define IRIS_RADIUS   25
#define EYE_CENTER_X  52
#define EYE_CENTER_Y  44

#define MY_UUID { 0xD1, 0xA9, 0xE2, 0x55, 0x7F, 0x18, 0x48, 0x8C, 0x8A, 0x5A, 0x28, 0xC2, 0xC3, 0xAF, 0x07, 0x10 }

PBL_APP_INFO(MY_UUID,
             "Wazowski", "Joel",
             1, 0, /* App version */
             RESOURCE_ID_ICON,
             APP_INFO_WATCH_FACE);

static GPoint CENTER = {
    .x = EYE_CENTER_X,
    .y = EYE_CENTER_Y
};

//static char dbg_text[30];

Window window;
BmpContainer background_image_container;
Layer eyeball_layer;
//TextLayer txt_debug_layer;

uint16_t get_minutes(PblTm *t) {
    return ((t->tm_hour % 12) * 60) + t->tm_min;
}

void draw_eye(uint16_t minutes, GContext *ctx) {
    GPoint pt;
    int32_t angle = TRIG_MAX_ANGLE * minutes / 720;
    
    pt.x = (sin_lookup(angle) * CENTER_RADIUS / TRIG_MAX_RATIO) + CENTER.x;
    pt.y = (-cos_lookup(angle) * CENTER_RADIUS / TRIG_MAX_RATIO) + CENTER.y;
    
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_draw_circle(ctx, pt, IRIS_RADIUS);
    
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_circle(ctx, pt, PUPIL_RADIUS);
}

void eyeball_layer_update_callback(Layer *me, GContext* ctx) {
    PblTm t;
    get_time(&t);
    
    // Draw current hour/minute combo
    draw_eye(get_minutes(&t), ctx);
    

    /*
    snprintf(dbg_text, 20, "%d:%d:%d, %f", t.tm_hour, t.tm_min, t.tm_sec, get_angle(get_minutes(&t)));
    text_layer_set_text(&txt_debug_layer, dbg_text);
    */
    
}

void handle_init(AppContextRef ctx) {

    window_init(&window, "Window Name");
    window_stack_push(&window, true /* Animated */);
    
    resource_init_current_app(&APP_RESOURCES);
    
    bmp_init_container(RESOURCE_ID_WAZOWSKI, &background_image_container);
    layer_add_child(&window.layer, &background_image_container.layer.layer);
    
    layer_init(&eyeball_layer, window.layer.frame);
    eyeball_layer.update_proc = &eyeball_layer_update_callback;
    layer_set_bounds(&eyeball_layer, GRect(20, 27, 104, 88));
    layer_add_child(&window.layer, &eyeball_layer);
    
    /*
    text_layer_init(&txt_debug_layer, window.layer.frame);
    text_layer_set_text_color(&txt_debug_layer, GColorWhite);
    text_layer_set_background_color(&txt_debug_layer, GColorBlack);
    layer_set_frame(&txt_debug_layer.layer, GRect(0, 0, 144, 30));
    text_layer_set_font(&txt_debug_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    
    layer_add_child(&window.layer, &txt_debug_layer.layer);
    */
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
    layer_mark_dirty(&eyeball_layer);
}

void handle_deinit(AppContextRef ctx) {
    bmp_deinit_container(&background_image_container);
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
      .deinit_handler = &handle_deinit,
      .tick_info = {
          .tick_handler = &handle_minute_tick,
          .tick_units = MINUTE_UNIT
      }
  };
  app_event_loop(params, &handlers);
}
