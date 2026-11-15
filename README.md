# USB Remote for Kodi (Flipper Zero)

A tiny Flipper Zero external app that makes the Flipper act as a USB keyboard remote for Kodi.

## Button mapping

| Flipper button | PC keyboard event |
|---|---|
| Up | Arrow Up |
| Down | Arrow Down |
| Left | Arrow Left |
| Right | Arrow Right |
| OK tap | Enter |
| **Hold OK** | **Space (held until OK is released)** |
| Back tap | Escape |

**Hold OK for Space; tap OK for Enter.** Space is released when you release OK.
This is useful for Kodi's play/pause action (Space).

## Layout

- **Hold LEFT** to switch between the two compact remote layouts shown on the Flipper display.
- The Flipper Zero's physical display is fixed in landscape (128×64); this switches the on-screen arrangement, it cannot physically rotate the Flipper display to portrait.


## Important notes

- This app sends keyboard events over USB HID; it does not control Kodi over Wi-Fi/network.
- Your PC may need to be focused on Kodi for the keys to affect Kodi.
- When the app is active, Flipper's USB connection changes to HID keyboard mode. Exit/restart the app or unplug/replug to return to normal USB mode.
- The Back button is mapped to Escape while this app is running. Hold Back to exit the app; a short Back press sends Escape.
- This source is a starter implementation and has not been compiled/tested on hardware in this chat. Check compiler output for firmware API differences.
