
# IoT-IDS: Low-Power Wireless Monitoring Node

ESP8266-based low-power wireless monitoring node written in C++ (Arduino framework).
The node connects to Wi-Fi, sends an email alert through Gmail SMTP over TLS, then immediately enters deep sleep to reduce power usage.

Target hardware: ESP8266 + HC-SR501 PIR sensor.

## What is currently in this repository

- `arduini.cpp` - our source code used as main firmware source (ESP8266 + SMTP alert + deep sleep behavior)

## Current Behavior

On wake/power-on, the firmware:

1. Connects to Wi-Fi
2. Opens a secure SMTP connection to Gmail (`smtp.gmail.com:465`)
3. Authenticates with app password
4. Sends a "Motion detected!" email
5. Enters deep sleep with `ESP.deepSleep(0)`

Architecture details:

- The alert workflow runs in `setup()` as a one-shot task.
- `loop()` is intentionally empty in this design.
- After deep sleep, the next event is a fresh boot (starts again from `setup()`).

## S-MAC and Duty Cycle Notes

This implementation aligns with low-duty-cycle MAC ideas (like S-MAC goals) by minimizing radio active time:

- Active window: Wi-Fi association + SMTP transaction
- Sleep window: deep sleep immediately after transmission
- Benefit: lower idle listening and lower average current draw

Practical note: `ESP.deepSleep(0)` means sleep indefinitely until reset/wake event. If you want periodic wake-ups, use a timed deep sleep value in microseconds and ensure `GPIO16 -> RST` is wired on ESP8266 boards.

This is effectively an event-driven wireless node: it is awake only long enough to transmit, then returns to minimum-power state.

## Hardware

- ESP8266 board (we used NodeMCU for simplicity)
- HC-SR501 PIR motion sensor
- Power source (battery + regulator recommended for field deployment)

### Pin Mapping (current code)

- `PIR_PIN = 14` (GPIO14 / D5 on many NodeMCU layouts)

### Critical Wake Wiring Check

For `ESP.deepSleep(0)` wake behavior, ESP8266 wakes when `RST` is pulled LOW and released.

- If your design uses PIR-triggered reset wake, ensure the HC-SR501 output stage can create a valid pulse at `RST`.
- Depending on module behavior and timing, you may need a small transistor stage to condition/invert the signal.
- Verify this on bench before battery deployment.

Note: in the current firmware, the PIR input pin is not used in runtime logic because detection/alert is modeled as a wake event (boot, alert, sleep).

## Software Requirements

- Arduino IDE 1.8+ or 2.x
- ESP8266 board package installed
- Libraries used in code:
  - `ESP8266WiFi`
  - `ESP8266HTTPClient`
  - `WiFiClient`
  - `WiFiClientSecure`
  - `base64`

## Build and Flash (Arduino IDE)

1. Select your ESP8266 board and COM port.
2. Compile and upload.
3. Open Serial Monitor at `115200` baud.

## Configuration

The top of the source contains editable constants:

- Wi-Fi SSID/password
- SMTP host/port
- Sender email and app password
- Recipient email


## Possible Next Improvements

- Trigger email only on actual PIR edge events after wake
- Add retry/backoff for Wi-Fi and SMTP failures
- Add timed wake cycle for periodic heartbeat/status
- Add battery-voltage telemetry in alert payload
- Add a simple backend API/MQTT mode in addition to SMTP


