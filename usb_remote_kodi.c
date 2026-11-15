#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_usb.h>
#include <furi_hal_usb_hid.h>
#include <gui/gui.h>
#include <gui/view_port.h>
#include <input/input.h>

#define EVENT_QUEUE_SIZE 8
#define HOLD_HINT_MS 500

typedef struct {
    FuriMessageQueue* queue;
    bool connected;
    bool space_mode;
} KodiRemote;

static void kodi_draw_callback(Canvas* canvas, void* context) {
    KodiRemote* app = context;
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "USB REMOTE FOR KODI");
    canvas_draw_line(canvas, 0, 13, 127, 13);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 23, app->connected ? "USB HID: READY" : "USB HID: CONNECTING");

    // Compact directional-pad illustration for the Flipper's physical D-pad.
    canvas_draw_frame(canvas, 49, 19, 30, 30);
    canvas_draw_str(canvas, 59, 28, "^");
    canvas_draw_str(canvas, 59, 45, "v");
    canvas_draw_str(canvas, 51, 37, "<");
    canvas_draw_str(canvas, 72, 37, ">");
    canvas_draw_box(canvas, 59, 32, 10, 5);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_str(canvas, 60, 37, " ");

    canvas_draw_str(canvas, 2, 55, "OK: ENTER  |  HOLD: SPACE");
    canvas_draw_str(canvas, 2, 63, "BACK: ESC  |  Hold OK > 0.5s");
}

static void kodi_input_callback(InputEvent* event, void* context) {
    KodiRemote* app = context;
    furi_message_queue_put(app->queue, event, 0);
}

static void send_press(uint16_t key) {
    furi_hal_hid_kb_press(key);
}

static void send_release(uint16_t key) {
    furi_hal_hid_kb_release(key);
}

static void handle_input(KodiRemote* app, InputEvent* event, bool* running) {
    if(event->key == InputKeyBack) {
        if(event->type == InputTypePress) {
            // A normal Back press sends Escape to Kodi.
            send_press(HID_KEYBOARD_ESCAPE);
        } else if(event->type == InputTypeRelease) {
            send_release(HID_KEYBOARD_ESCAPE);
        } else if(event->type == InputTypeLong) {
            // Long-press Back exits the remote.
            *running = false;
        }
        return;
    }

    if(event->key == InputKeyOk) {
        if(event->type == InputTypePress) {
            app->space_mode = false;
            send_press(HID_KEYBOARD_RETURN);
        } else if(event->type == InputTypeLong) {
            // Convert the held OK key into a held Space key.
            send_release(HID_KEYBOARD_RETURN);
            app->space_mode = true;
            send_press(HID_KEYBOARD_SPACEBAR);
        } else if(event->type == InputTypeRelease) {
            send_release(app->space_mode ? HID_KEYBOARD_SPACEBAR : HID_KEYBOARD_RETURN);
            app->space_mode = false;
        }
        return;
    }

    uint16_t key = HID_KEYBOARD_NONE;
    switch(event->key) {
    case InputKeyUp:    key = HID_KEYBOARD_UP; break;
    case InputKeyDown:  key = HID_KEYBOARD_DOWN; break;
    case InputKeyLeft:  key = HID_KEYBOARD_LEFT; break;
    case InputKeyRight: key = HID_KEYBOARD_RIGHT; break;
    default: break;
    }

    if(key == HID_KEYBOARD_NONE) return;
    if(event->type == InputTypePress) send_press(key);
    else if(event->type == InputTypeRelease) send_release(key);
}

int32_t usb_remote_kodi_app(void* p) {
    UNUSED(p);

    KodiRemote app = {0};
    app.queue = furi_message_queue_alloc(EVENT_QUEUE_SIZE, sizeof(InputEvent));

    // Switch the Flipper USB interface to a standard HID keyboard.
    furi_hal_usb_set_config(&usb_hid, NULL);

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, kodi_draw_callback, &app);
    view_port_input_callback_set(view_port, kodi_input_callback, &app);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    bool running = true;
    while(running) {
        app.connected = furi_hal_hid_is_connected();
        view_port_update(view_port);

        InputEvent event;
        if(furi_message_queue_get(app.queue, &event, 100) == FuriStatusOk) {
            handle_input(&app, &event, &running);
        }
    }

    // Never leave a key held down when the app exits.
    furi_hal_hid_kb_release_all();
    furi_hal_usb_set_config(NULL, NULL);

    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(app.queue);
    return 0;
}
