#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_usb.h>
#include <furi_hal_usb_hid.h>
#include <gui/gui.h>
#include <gui/view_port.h>
#include <input/input.h>

#define TAG "KodiUSBRemote"

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    bool portrait_layout;
    bool space_held;
    bool connected;
} KodiRemoteApp;

static void key_tap(uint16_t key) {
    furi_hal_hid_kb_press(key);
    furi_delay_ms(35);
    furi_hal_hid_kb_release(key);
}

static void draw_button(Canvas* canvas, int x, int y, int w, int h, const char* label, bool filled) {
    if(filled) canvas_draw_box(canvas, x, y, w, h);
    else canvas_draw_rbox(canvas, x, y, w, h, 2);
    if(filled) canvas_set_color(canvas, ColorWhite);
    canvas_draw_str_aligned(canvas, x + w/2, y + h/2, AlignCenter, AlignCenter, label);
    if(filled) canvas_set_color(canvas, ColorBlack);
}

static void draw_callback(Canvas* canvas, void* context) {
    KodiRemoteApp* app = context;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "USB REMOTE FOR KODI");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 21, furi_hal_hid_is_connected() ? "USB: connected" : "USB: plug into PC");
    if(!app->portrait_layout) {
        // Landscape-style D-pad layout
        draw_button(canvas, 48, 24, 22, 13, "^", false);
        draw_button(canvas, 25, 37, 22, 13, "<", false);
        draw_button(canvas, 48, 37, 22, 13, "OK", true);
        draw_button(canvas, 71, 37, 22, 13, ">", false);
        draw_button(canvas, 48, 50, 22, 12, "v", false);
        canvas_draw_str(canvas, 98, 34, "BACK");
        canvas_draw_str(canvas, 98, 44, "ESC");
    } else {
        // Compact alternate/portrait-style arrangement (screen itself stays landscape)
        draw_button(canvas, 53, 23, 22, 12, "^", false);
        draw_button(canvas, 30, 35, 22, 12, "<", false);
        draw_button(canvas, 53, 35, 22, 12, "OK", true);
        draw_button(canvas, 76, 35, 22, 12, ">", false);
        draw_button(canvas, 53, 47, 22, 12, "v", false);
        canvas_draw_str(canvas, 2, 34, "BACK");
        canvas_draw_str(canvas, 2, 44, "ESC");
    }
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 61, "Hold OK = SPACE | Hold LEFT = layout");
}

static void input_callback(InputEvent* event, void* context) {
    KodiRemoteApp* app = context;
    if(event->key == InputKeyOk) {
        if(event->type == InputTypeShort) {
            key_tap(HID_KEYBOARD_RETURN);
        } else if(event->type == InputTypeLong && !app->space_held) {
            app->space_held = true;
            furi_hal_hid_kb_press(HID_KEYBOARD_SPACEBAR);
        } else if(event->type == InputTypeRelease && app->space_held) {
            furi_hal_hid_kb_release(HID_KEYBOARD_SPACEBAR);
            app->space_held = false;
        }
    } else if(event->key == InputKeyUp && event->type == InputTypeShort) {
        key_tap(HID_KEYBOARD_UP);
    } else if(event->key == InputKeyDown && event->type == InputTypeShort) {
        key_tap(HID_KEYBOARD_DOWN);
    } else if(event->key == InputKeyLeft) {
        if(event->type == InputTypeShort) key_tap(HID_KEYBOARD_LEFT);
        else if(event->type == InputTypeLong) {
            app->portrait_layout = !app->portrait_layout;
            view_port_update(app->view_port);
        }
    } else if(event->key == InputKeyRight && event->type == InputTypeShort) {
        key_tap(HID_KEYBOARD_RIGHT);
    } else if(event->key == InputKeyBack && event->type == InputTypeShort) {
        key_tap(HID_KEYBOARD_ESC);
    }
}

int32_t usb_remote_kodi_app(void* p) {
    UNUSED(p);
    KodiRemoteApp* app = malloc(sizeof(KodiRemoteApp));
    app->gui = furi_record_open(RECORD_GUI);
    app->view_port = view_port_alloc();
    app->portrait_layout = false;
    app->space_held = false;

    // Switch the Flipper USB port into standard USB keyboard mode.
    if(!furi_hal_usb_set_config(&usb_hid, NULL)) {
        FURI_LOG_E(TAG, "Could not switch USB to HID");
    }

    view_port_draw_callback_set(app->view_port, draw_callback, app);
    view_port_input_callback_set(app->view_port, input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    view_port_update(app->view_port);

    while(true) {
        furi_delay_ms(100);
        // Back is deliberately mapped to Escape; use a long Back press to exit.
        // The app remains active for reliable remote control use.
    }

    // Cleanup is retained for completeness (the loop above is the active UI).
    if(app->space_held) furi_hal_hid_kb_release(HID_KEYBOARD_SPACEBAR);
    furi_hal_hid_kb_release_all();
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    furi_hal_usb_set_config(NULL, NULL);
    free(app);
    return 0;
}
