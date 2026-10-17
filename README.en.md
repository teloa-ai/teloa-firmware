# Teloa Firmware

Bring Teloa's squirrel assistant to ESP32-S3 hardware. This firmware connects over the same Wi-Fi to a compatible, self-hosted Teloa Community gateway for voice tasks, result playback, the squirrel display, and explicitly authorized touch approvals.

[简体中文](README.md) · [Build](docs/build.md) · [Pairing and protocol](docs/pairing.md) · [Recovery](docs/recovery.md)

![Teloa squirrel](main/teloa/assets/standing.png)

The default build uses Teloa's direct LAN protocol. It does not activate with, obtain startup updates from, or connect through the official Xiaozhi service. Models, ASR and TTS use your own host configuration; no API keys or free speech service are included.

**Development source: `0.1.0-dev-b2.1`.** The Bubble / ESP-VoCat configuration is a candidate, not a verified hardware installation target. PCB identity, touch, microphone, speaker and full device recovery still require physical validation. No universal flashable binary or automated installer is released here. Application OTA download and the complete update flow are not implemented.

A compatible development host must implement `teloa-device/v1`, owner-approved USB pairing IPC and the native approval bridge. Do not assume the currently published Teloa npm/Docker versions support these interfaces. Firmware source and protocol are being opened independently; a compatible host release follows separately. Offline preview and compilation do not require a host.

With the pinned ESP-IDF **v6.1** installed, run `idf.py build` at the repository root. A fresh checkout selects the Teloa candidate profile automatically; a non-direct configuration is rejected. See the build guide for exact inputs and tests. Build does not write to a device.

For an offline screen demo, run `python3 tools/preview.py --output "screen-preview.html"` and open that new file in a browser. No device or credentials are used, and demo approvals have no effect.

Based on [78/xiaozhi-esp32](https://github.com/78/xiaozhi-esp32) at `d7a1c05308fb6e116ffb44b9e2c6e67b3366302a`, with upstream attribution and original import hashes preserved. Firmware code/tools use [MIT](LICENSE); Teloa mascot assets retain Apache-2.0, the font derivative retains SIL OFL 1.1, and gifdec retains its public-domain notice. See [third-party notices](THIRD_PARTY_NOTICES.md). Software licenses do not grant Teloa trademark rights.

This public repository is a standalone, allowlisted source export from Teloa Community's `firmware/esp32`, with fresh publication history. `source-manifest.json` records the source revision and file hashes. Development has one source of truth in Community; no private development history, device backup, runtime data or credentials are included.
