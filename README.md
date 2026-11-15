# FlipperZeroUSBKeyboard
Turn your Flipper Zero into an USB keyboard. Works with the [latest Unleashed firmware](https://github.com/Eng1n33r/flipperzero-firmware). See the release on how to install the external app on the sdcard. For building instructions please refer the [official FAP guide](https://github.com/Eng1n33r/flipperzero-firmware/blob/dev/documentation/AppsOnSDCard.md).

How it works: https://twitter.com/hookgab/status/1572537933210718211

Credits:

All the good people that made the [BT HID remote](https://github.com/flipperdevices/flipperzero-firmware/tree/873e1f114b7ca55a72dc68bf1b1fa6d169e7c17e/applications/plugins/bt_hid_app) and the [USB Mouse](https://github.com/flipperdevices/flipperzero-firmware/tree/873e1f114b7ca55a72dc68bf1b1fa6d169e7c17e/applications/debug/usb_mouse).


## Kodi Remote modification

This repository copy has been modified for Kodi use in the Dirpad screen:
- Arrow directions send the corresponding keyboard arrow keys.
- OK short sends Enter.
- OK long presses and holds Space; releasing OK releases Space.
- Back short sends Escape.
- Hold Back retains the upstream exit-confirm behavior.
- On-screen instructions: "Tap OK=Enter / Hold OK=Space"; OK label changes to Enter/Space; Back label is Esc.

The upstream USB HID transport, app framework, keyboard/media/mouse screens, assets, headers and manifest structure are retained. This is a source modification; it has not been run through FlipperFAP Actions or tested on a Flipper device in this environment.
