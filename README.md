# Wireless Monitoring Node (ESP8266)

ESP8266-based low-power wireless monitoring node written in C++ (Arduino framework).  
Used hardware: ESP8266 + HC-SR501 PIR sensor + power source  
Node connects to Wi-Fi, sends an email alert through Gmail SMTP over TLS, then immediately enters deep sleep.  

`arduini.cpp` contains source code used as main firmware source (ESP8266 + SMTP alert + deep sleep behavior)

### Setup
Arduino IDE: Open Serial Monitor at `115200` baud.  
The top of the source contains editable hard-coded constants:

* Wi-Fi SSID/password
* SMTP host/port
* Sender email and app password (gmail) 
* Recipient email

## Behavior

Event-driven wireless node: it is awake only long enough to transmit, then returns to minimum-power state.

On wake/power-on, the firmware:
1. Connects to Wi-Fi
2. Opens a secure SMTP connection to Gmail (`smtp.gmail.com:465`)
3. Authenticates with app password
4. Sends a "Motion detected!" email
5. Enters deep sleep with `ESP.deepSleep(0)`

* The alert workflow runs in `setup()` as a one-shot task.
* After deep sleep, the next event is a fresh boot (starts again from `setup()`).

## S-MAC and Duty Cycle Notes

This implementation aligns with low-duty-cycle MAC ideas (like S-MAC goals) by minimizing radio active time:

* **Active window**: Wi-Fi association + SMTP transaction (wake, transmit -> sleep)
* **Sleep window**: deep sleep immediately after transmission (sleep avoids idle listening)

**Note**: `ESP.deepSleep(0)` means sleep indefinitely until reset/wake event.  
If you want periodic wake-ups (such as for mesh network implementations), use a timed deep sleep value in ms (e.g., wake every 16.6ms to update display) and ensure `GPIO16 -> RST` is wired on board.

## Hardware & Pin Mapping

* **ESP8266 board**: NodeMCU used for development.
* **HC-SR501 PIR**: 3.3V motion sensor.
* **Power source**: USB-C connected battery.
* **PIR_PIN**: `14` (GPIO14 / D5 on NodeMCU).

### Pre-deployment Wake Wiring Check
For `ESP.deepSleep(0)` wake behavior, ESP8266 wakes when `RST` is pulled LOW and released.

* If your design uses PIR-triggered reset wake, ensure the HC-SR501 output stage can create a valid pulse at `RST`.
* Depending on module behavior and timing, you may need a small transistor stage to condition/invert the signal.
* **Note**: In the current firmware, the PIR input pin is not used in runtime logic because detection/alert is modeled as a wake event (boot, alert, sleep).

## Software Requirements

* Arduino IDE 1.8+ or 2.x
* ESP8266 board package installed
* **Libraries**: `ESP8266WiFi`, `WiFiClientSecure`, `base64`.

## Possible Next Improvements

* Add retry/backoff for Wi-Fi and SMTP failures.
* Add timed wake cycle for periodic heartbeat/status.
* Add battery-voltage telemetry in alert payload.
