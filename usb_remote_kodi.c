#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_usb.h>
#include <furi_hal_usb_hid.h>

#include <gui/gui.h>
#include <gui/view_port.h>
#include <input/input.h>

typedef struct {
    FuriMessageQueue* queue;
    bool space;
    bool running;
    FuriHalUsbInterface* previous_usb;
} App;

static void draw(Canvas* canvas, void* context) {
    App* app = context;

    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "USB Remote for Kodi");

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 22, "D-pad: Navigate");
    canvas_draw_str(canvas, 2, 32, "OK: Select");
    canvas_draw_str(canvas, 2, 42, "Hold OK: Enter/Space");
    canvas_draw_str(canvas, 2, 52, "Back: Escape");
    canvas_draw_str(
        canvas,
        2,
        62,
        app->space ? "Center: Space" : "Center: Enter"
    );
}

static void input_callback(InputEvent* event, void* context) {
    App* app = context;

    furi_message_queue_put(app->queue, event, 0);
}

static void send_key(uint16_t key) {
    furi_hal_hid_kb_press(key);
    furi_delay_ms(30);
    furi_hal_hid_kb_release(key);
}

int32_t usb_remote_kodi_app(void* p) {
    UNUSED(p);

    App app = {0};

    app.queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app.running = true;

    // Remember existing USB configuration.
    app.previous_usb = furi_hal_usb_get_config();

    // Enable USB HID keyboard.
    if(!furi_hal_usb_set_config(&usb_hid, NULL)) {
        furi_message_queue_free(app.queue);
        return -1;
    }

    // Open GUI.
    Gui* gui = furi_record_open(RECORD_GUI);

    ViewPort* viewport = view_port_alloc();

    view_port_draw_callback_set(viewport, draw, &app);
    view_port_input_callback_set(viewport, input_callback, &app);

    gui_add_view_port(gui, viewport, GuiLayerFullscreen);

    // Main application loop.
    while(app.running) {
        InputEvent event;

        if(furi_message_queue_get(
               app.queue,
               &event,
               FuriWaitForever) != FuriStatusOk) {
            continue;
        }

        // Long press handling.
        if(event.type == InputTypeLong) {
            if(event.key == InputKeyOk) {
                app.space = !app.space;
                view_port_update(viewport);
            } else if(event.key == InputKeyBack) {
                app.running = false;
            }

            continue;
        }

        // Only process short presses.
        if(event.type != InputTypeShort) {
            continue;
        }

        switch(event.key) {
        case InputKeyUp:
            send_key(HID_KEYBOARD_UP_ARROW);
            break;

        case InputKeyDown:
            send_key(HID_KEYBOARD_DOWN_ARROW);
            break;

        case InputKeyLeft:
            send_key(HID_KEYBOARD_LEFT_ARROW);
            break;

        case InputKeyRight:
            send_key(HID_KEYBOARD_RIGHT_ARROW);
            break;

        case InputKeyOk:
            send_key(
                app.space
                    ? HID_KEYBOARD_SPACEBAR
                    : HID_KEYBOARD_RETURN
            );
            break;

        case InputKeyBack:
            send_key(HID_KEYBOARD_ESCAPE);
            break;

        default:
            break;
        }
    }

    // Release any held keyboard keys.
    furi_hal_hid_kb_release_all();

    // Clean up GUI.
    gui_remove_view_port(gui, viewport);
    view_port_free(viewport);
    furi_record_close(RECORD_GUI);

    // Restore previous USB configuration.
    furi_hal_usb_set_config(app.previous_usb, NULL);

    // Free message queue.
    furi_message_queue_free(app.queue);

    return 0;
}