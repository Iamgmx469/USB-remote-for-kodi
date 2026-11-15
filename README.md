# USB Remote for Kodi — Flipper Zero

A native Flipper Zero external application (FAP) that makes the Flipper act as a USB HID keyboard for Kodi.

## Controls

| Flipper control | Kodi / keyboard action |
|---|---|
| D-pad Up | Up Arrow |
| D-pad Down | Down Arrow |
| D-pad Left | Left Arrow |
| D-pad Right | Right Arrow |
| Short press OK | Enter |
| Hold OK (long-press threshold) | Space |
| Release OK | Releases Space or Enter |
| Short press Back | Escape |
| Hold Back | Exit the remote app |

**Important:** Kodi's Space action is sent only while OK is held past the Flipper long-press threshold. A quick OK tap sends Enter.

## Requirements

- Flipper Zero
- Unleashed firmware
- USB data cable
- microSD card
- uFBT (micro Flipper Build Tool) and a compatible firmware SDK for compiling

## Build

Install uFBT using the current instructions in the official Flipper developer documentation:
https://developer.flipper.net/flipperzero/doxygen/

From this project directory, build with:

```bash
ufbt
```

If your uFBT setup expects an app folder inside a firmware checkout, copy this folder into `applications_user/usb_remote_kodi/` and build using the firmware build instructions.

The generated `.fap` is normally placed in `dist/`.

## Install

Copy `usb_remote_kodi.fap` to the Flipper microSD card:

```text
/ext/apps/USB/usb_remote_kodi.fap
```

Then safely eject/reboot the Flipper and open:

**Apps → USB → USB Remote Kodi**

Connect the Flipper to the PC with a USB data cable and launch the app. Close qFlipper while using the USB HID app if it has claimed the USB connection.

## Use

1. Connect the Flipper to the computer with a USB data cable.
2. Launch **USB Remote Kodi** on the Flipper.
3. Wait for `USB HID: READY`.
4. Focus Kodi on the computer.
5. Use the Flipper D-pad and OK/Back controls. Short Back sends Escape; hold Back to exit the app.

## Notes and limitations

- The Flipper Zero has a fixed 128×64 landscape display. Its screen cannot rotate into portrait mode; the interface is designed for the native landscape screen.
- This is a source project, not a precompiled `.fap`. Build it against the SDK matching your installed Unleashed firmware. FAP API compatibility depends on the firmware SDK/API version.
- The project uses the firmware's exported USB HID HAL (`furi_hal_hid_*`) and USB HID profile (`usb_hid`). If your exact Unleashed SDK does not export one of these symbols, build output will identify the mismatch; use the matching Unleashed SDK rather than an official-firmware SDK.
- Holding OK uses the firmware's `InputTypeLong` event. The long-press delay is determined by Flipper's input system.
- This application sends ordinary navigation keystrokes only. It does not run scripts or execute commands on the connected computer.

## GitHub

Create a new GitHub repository, upload the contents of this folder, then follow the Build section above.

## License

MIT — see `LICENSE`.
