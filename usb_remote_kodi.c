#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_usb.h>
#include <furi_hal_hid.h>
#include <gui/gui.h>
#include <gui/view_port.h>

typedef struct {
    FuriMessageQueue* queue;
    bool space;
    bool landscape;
} App;

static void draw(Canvas* c, void* ctx) {
    App* a = ctx;
    canvas_clear(c);
    canvas_set_font(c, FontPrimary);
    canvas_draw_str(c, 2, 10, "USB Remote for Kodi");
    canvas_set_font(c, FontSecondary);
    canvas_draw_str(c, 2, 22, "D-pad: navigate  OK: select");
    canvas_draw_str(c, 2, 32, "Hold OK: toggle Enter/Space");
    canvas_draw_str(c, 2, 42, "Back: Escape | Hold Back: layout");
    canvas_draw_str(c, 2, 58, a->space ? "Center key: Space" : "Center key: Enter");
}

static void input(InputEvent* e, void* ctx) {
    App* a = ctx;
    furi_message_queue_put(a->queue, e, 0);
}

static void tap(uint16_t key) {
    furi_hal_hid_kb_press(key);
    furi_delay_ms(30);
    furi_hal_hid_kb_release();
}

int32_t usb_remote_kodi_app(void* p) {
    UNUSED(p);
    App a = {0};
    a.queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    furi_hal_usb_set_config(&usb_hid, NULL);

    Gui* gui = furi_record_open(RECORD_GUI);
    ViewPort* vp = view_port_alloc();
    view_port_draw_callback_set(vp, draw, &a);
    view_port_input_callback_set(vp, input, &a);
    gui_add_view_port(gui, vp, GuiLayerFullscreen);

    bool running = true;
    while(running) {
        InputEvent e;
        if(furi_message_queue_get(a.queue, &e, FuriWaitForever) != FuriStatusOk) continue;

        if(e.key == InputKeyBack && e.type == InputTypePress) {
            /* Short Back is handled below; long Back changes layout. */
        }

        if(e.type == InputTypeLong && e.key == InputKeyOk) {
            a.space = !a.space;
            view_port_update(vp);
        } else if(e.type == InputTypeLong && e.key == InputKeyBack) {
            a.landscape = !a.landscape;
            view_port_update(vp);
        } else if(e.type == InputTypeShort) {
            switch(e.key) {
            case InputKeyUp:    tap(HID_KEYBOARD_ARROW_UP); break;
            case InputKeyDown:  tap(HID_KEYBOARD_ARROW_DOWN); break;
            case InputKeyLeft:  tap(HID_KEYBOARD_ARROW_LEFT); break;
            case InputKeyRight: tap(HID_KEYBOARD_ARROW_RIGHT); break;
            case InputKeyOk:    tap(a.space ? HID_KEYBOARD_SPACE : HID_KEYBOARD_RETURN); break;
            case InputKeyBack:  tap(HID_KEYBOARD_ESCAPE); break;
            default: break;
            }
        }
    }

    furi_hal_hid_kb_release();
    gui_remove_view_port(gui, vp);
    view_port_free(vp);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(a.queue);
    furi_hal_usb_set_config(&usb_cdc, NULL);
    return 0;
}