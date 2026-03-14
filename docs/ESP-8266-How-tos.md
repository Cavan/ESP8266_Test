# ESP-8266 (NodeMCU v2) — How-Tos & Component Testing

> **Board:** NodeMCU v2 (ESP-12E module)
> **Framework:** Arduino (PlatformIO)
> **Monitor baud:** 115200

---

## Table of Contents

1. [NodeMCU v2 Pinout Reference](#nodemcu-v2-pinout-reference)
2. [Blink — Onboard LED](#1-blink--onboard-led)
3. [Digital Output — External LED](#2-digital-output--external-led)
4. [Digital Input — Push Button](#3-digital-input--push-button)
5. [Analog Input — Potentiometer](#4-analog-input--potentiometer)
6. [PWM — LED Dimming](#5-pwm--led-dimming)
7. [Servo Motor Control](#6-servo-motor-control)
8. [DHT11/DHT22 Temperature & Humidity Sensor](#7-dht11dht22-temperature--humidity-sensor)
9. [I²C — OLED Display (SSD1306)](#8-ic--oled-display-ssd1306)
10. [Ultrasonic Distance Sensor (HC-SR04)](#9-ultrasonic-distance-sensor-hc-sr04)
11. [Wi-Fi — Connect to a Network](#10-wi-fi--connect-to-a-network)
12. [Wi-Fi — Simple Web Server](#11-wi-fi--simple-web-server)
13. [Relay Module Control](#12-relay-module-control)
14. [Buzzer — Tone Generation](#13-buzzer--tone-generation)

---

## NodeMCU v2 Pinout Reference

| Label on Board | GPIO  | Notes                                 |
| -------------- | ----- | ------------------------------------- |
| D0             | GPIO16| Wake from deep sleep, no PWM/I²C     |
| D1             | GPIO5 | I²C SCL (default)                    |
| D2             | GPIO4 | I²C SDA (default)                    |
| D3             | GPIO0 | FLASH button, pulled HIGH             |
| D4             | GPIO2 | Onboard LED (active LOW), pulled HIGH |
| D5             | GPIO14| SPI SCLK                              |
| D6             | GPIO12| SPI MISO                              |
| D7             | GPIO13| SPI MOSI                              |
| D8             | GPIO15| SPI CS, pulled LOW                    |
| A0             | ADC0  | Analog input 0–1 V (10-bit)          |

> **Important:** The NodeMCU v2 onboard LED on **D4 (GPIO2)** is **active LOW** — writing `LOW` turns it **ON**.

---

## 1. Blink — Onboard LED

The simplest test to verify your board and toolchain work.

### Wiring Diagram

No external wiring needed — uses the onboard LED on D4 (GPIO2).

```mermaid
graph LR
    subgraph NodeMCU_v2
        D4["D4 (GPIO2)<br/>Onboard LED"]
    end
    D4 -->|"Active LOW"| LED["Built-in LED 💡"]
```

### Code

```cpp
#include <Arduino.h>

#define LED_BUILTIN_ESP 2  // GPIO2 = D4 = onboard LED

void setup() {
  pinMode(LED_BUILTIN_ESP, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN_ESP, LOW);   // LED ON  (active LOW)
  delay(500);
  digitalWrite(LED_BUILTIN_ESP, HIGH);  // LED OFF
  delay(500);
}
```

---

## 2. Digital Output — External LED

### Wiring Diagram

```mermaid
graph LR
    subgraph NodeMCU_v2
        D1["D1 (GPIO5)"]
        GND["GND"]
    end
    D1 -->|"Digital Out"| R["220Ω Resistor"]
    R --> ANODE["LED (+) Anode"]
    CATHODE["LED (-) Cathode"] --> GND

    style ANODE fill:#f66,stroke:#333
    style CATHODE fill:#333,stroke:#333,color:#fff
```

### Connections

| Component   | Pin         | NodeMCU Pin |
| ----------- | ----------- | ----------- |
| LED Anode   | + (long leg)| D1 via 220Ω resistor |
| LED Cathode | - (short leg)| GND        |

### Code

```cpp
#include <Arduino.h>

#define LED_PIN D1  // GPIO5

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
}
```

---

## 3. Digital Input — Push Button

### Wiring Diagram

```mermaid
graph TD
    subgraph NodeMCU_v2
        D2["D2 (GPIO4)"]
        GND["GND"]
        V3["3.3V"]
    end

    V3 -->|"Power"| BTN_A["Button Pin 1"]
    BTN_B["Button Pin 2"] --> R10K["10kΩ Pull-down<br/>Resistor"]
    R10K --> GND
    BTN_B -->|"Signal"| D2

    style BTN_A fill:#4af,stroke:#333
    style BTN_B fill:#4af,stroke:#333
```

### Connections

| Component          | Connect To     |
| ------------------ | -------------- |
| Button Pin 1       | 3.3V           |
| Button Pin 2       | D2 (GPIO4)     |
| Button Pin 2       | GND via 10kΩ (pull-down) |

### Code

```cpp
#include <Arduino.h>

#define BUTTON_PIN D2   // GPIO4
#define LED_PIN    D1   // GPIO5

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  int state = digitalRead(BUTTON_PIN);
  digitalWrite(LED_PIN, state);
  Serial.printf("Button: %s\n", state ? "PRESSED" : "RELEASED");
  delay(100);
}
```

> **Tip:** You can also use `INPUT_PULLUP` and wire the button to GND (no external resistor needed), but the logic will be inverted.

---

## 4. Analog Input — Potentiometer

The NodeMCU v2 has **one** analog pin (A0) with a 0–1V input range (an onboard voltage divider scales 0–3.3V down to 0–1V).

### Wiring Diagram

```mermaid
graph LR
    subgraph NodeMCU_v2
        A0["A0 (ADC)"]
        GND["GND"]
        V3["3.3V"]
    end

    subgraph Potentiometer
        P1["Pin 1 (outer)"]
        PW["Pin 2 (wiper)"]
        P3["Pin 3 (outer)"]
    end

    V3 --> P1
    PW -->|"Analog Signal"| A0
    P3 --> GND

    style PW fill:#fa0,stroke:#333
```

### Connections

| Potentiometer Pin | Connect To |
| ----------------- | ---------- |
| Pin 1 (outer)     | 3.3V       |
| Pin 2 (wiper/middle) | A0     |
| Pin 3 (outer)     | GND        |

### Code

```cpp
#include <Arduino.h>

void setup() {
  Serial.begin(115200);
}

void loop() {
  int value = analogRead(A0);  // 0–1023
  float voltage = value * (3.3 / 1023.0);
  Serial.printf("ADC: %d | Voltage: %.2f V\n", value, voltage);
  delay(250);
}
```

---

## 5. PWM — LED Dimming

All GPIOs on the ESP8266 support software PWM (except GPIO16).

### Wiring Diagram

Same wiring as [External LED](#2-digital-output--external-led).

```mermaid
graph LR
    subgraph NodeMCU_v2
        D1["D1 (GPIO5)"]
        GND["GND"]
    end

    D1 -->|"PWM Signal"| R["220Ω"]
    R --> LED["LED"]
    LED --> GND

    style LED fill:#ff6,stroke:#333
```

### Code

```cpp
#include <Arduino.h>

#define LED_PIN D1  // GPIO5

void setup() {
  pinMode(LED_PIN, OUTPUT);
  analogWriteRange(1023);  // 10-bit resolution (default)
  analogWriteFreq(1000);   // 1 kHz
}

void loop() {
  // Fade in
  for (int duty = 0; duty <= 1023; duty += 5) {
    analogWrite(LED_PIN, duty);
    delay(5);
  }
  // Fade out
  for (int duty = 1023; duty >= 0; duty -= 5) {
    analogWrite(LED_PIN, duty);
    delay(5);
  }
}
```

---

## 6. Servo Motor Control

### Wiring Diagram

```mermaid
graph LR
    subgraph NodeMCU_v2
        D5["D5 (GPIO14)"]
        GND["GND"]
        VIN["VIN (5V from USB)"]
    end

    subgraph Servo_Motor
        SIG["Signal (Orange/White)"]
        VCC["VCC (Red)"]
        SGND["GND (Brown/Black)"]
    end

    D5 -->|"PWM Signal"| SIG
    VIN -->|"5V Power"| VCC
    GND --- SGND

    style SIG fill:#fa0,stroke:#333
    style VCC fill:#f66,stroke:#333
    style SGND fill:#333,stroke:#333,color:#fff
```

### Connections

| Servo Wire         | Connect To       |
| ------------------- | ---------------- |
| Signal (orange)     | D5 (GPIO14)      |
| VCC (red)           | VIN (5V via USB) |
| GND (brown/black)   | GND              |

> **Note:** For multiple or larger servos, use an **external 5V supply** and share a common GND with the NodeMCU.

### Code

Add to `platformio.ini`:
```ini
lib_deps = 
    servo
```

```cpp
#include <Arduino.h>
#include <Servo.h>

#define SERVO_PIN D5  // GPIO14

Servo myServo;

void setup() {
  myServo.attach(SERVO_PIN);
}

void loop() {
  for (int angle = 0; angle <= 180; angle += 10) {
    myServo.write(angle);
    delay(200);
  }
  for (int angle = 180; angle >= 0; angle -= 10) {
    myServo.write(angle);
    delay(200);
  }
}
```

---

## 7. DHT11/DHT22 Temperature & Humidity Sensor

### Wiring Diagram

```mermaid
graph LR
    subgraph NodeMCU_v2
        D3["D3 (GPIO0)"]
        GND["GND"]
        V3["3.3V"]
    end

    subgraph DHT_Sensor["DHT11 / DHT22"]
        VCC_D["VCC (Pin 1)"]
        DATA["DATA (Pin 2)"]
        NC["NC (Pin 3)"]
        GND_D["GND (Pin 4)"]
    end

    V3 --> VCC_D
    DATA -->|"Data"| D3
    V3 ---|"4.7kΩ Pull-up"| DATA
    GND --- GND_D

    style DATA fill:#0c0,stroke:#333
```

### Connections

| DHT Pin   | Connect To                      |
| --------- | ------------------------------- |
| Pin 1 VCC | 3.3V                            |
| Pin 2 DATA| D3 (GPIO0) + 4.7kΩ pull-up to 3.3V |
| Pin 3 NC  | Not connected                   |
| Pin 4 GND | GND                             |

### Code

Add to `platformio.ini`:
```ini
lib_deps = 
    adafruit/DHT sensor library@^1.4.4
    adafruit/Adafruit Unified Sensor@^1.1.9
```

```cpp
#include <Arduino.h>
#include <DHT.h>

#define DHT_PIN  D3       // GPIO0
#define DHT_TYPE DHT11    // or DHT22

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  float humidity    = dht.readHumidity();
  float tempC       = dht.readTemperature();
  float tempF       = dht.readTemperature(true);

  if (isnan(humidity) || isnan(tempC)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  float heatIndex = dht.computeHeatIndex(tempF, humidity);

  Serial.printf("Humidity: %.1f%%  |  Temp: %.1f°C / %.1f°F  |  Heat Index: %.1f°F\n",
                humidity, tempC, tempF, heatIndex);
  delay(2000);
}
```

---

## 8. I²C — OLED Display (SSD1306)

### Wiring Diagram

```mermaid
graph LR
    subgraph NodeMCU_v2
        D1_SCL["D1 (GPIO5) — SCL"]
        D2_SDA["D2 (GPIO4) — SDA"]
        GND["GND"]
        V3["3.3V"]
    end

    subgraph OLED["SSD1306 OLED (128x64)"]
        O_VCC["VCC"]
        O_GND["GND"]
        O_SCL["SCL"]
        O_SDA["SDA"]
    end

    V3 --> O_VCC
    GND --- O_GND
    D1_SCL -->|"I²C Clock"| O_SCL
    D2_SDA -->|"I²C Data"| O_SDA

    style OLED fill:#115,stroke:#fff,color:#fff
```

### Connections

| OLED Pin | NodeMCU Pin        |
| -------- | ------------------ |
| VCC      | 3.3V               |
| GND      | GND                |
| SCL      | D1 (GPIO5)         |
| SDA      | D2 (GPIO4)         |

### Code

Add to `platformio.ini`:
```ini
lib_deps = 
    adafruit/Adafruit SSD1306@^2.5.7
    adafruit/Adafruit GFX Library@^1.11.5
```

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDR     0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 allocation failed!");
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Hello from");
  display.setTextSize(2);
  display.println("NodeMCU!");
  display.display();
}

void loop() {}
```

---

## 9. Ultrasonic Distance Sensor (HC-SR04)

> **Note:** The HC-SR04 operates at 5V. Use VIN (5V from USB) for power and a voltage divider on the Echo pin to bring 5V down to 3.3V for the NodeMCU.

### Wiring Diagram

```mermaid
graph LR
    subgraph NodeMCU_v2
        D6["D6 (GPIO12) — Trigger"]
        D7["D7 (GPIO13) — Echo"]
        GND["GND"]
        VIN["VIN (5V)"]
    end

    subgraph HC_SR04["HC-SR04"]
        TRIG["TRIG"]
        ECHO["ECHO"]
        VCC_U["VCC"]
        GND_U["GND"]
    end

    subgraph Voltage_Divider["Voltage Divider"]
        R1["1kΩ"]
        R2["2kΩ"]
    end

    VIN --> VCC_U
    GND --- GND_U
    D6 -->|"Trigger Pulse"| TRIG
    ECHO --> R1
    R1 --> D7
    R1 --> R2
    R2 --> GND

    style HC_SR04 fill:#059,stroke:#fff,color:#fff
```

### Connections

| HC-SR04 Pin | Connect To                                  |
| ----------- | ------------------------------------------- |
| VCC         | VIN (5V)                                    |
| TRIG        | D6 (GPIO12)                                 |
| ECHO        | D7 (GPIO13) via voltage divider (1kΩ + 2kΩ) |
| GND         | GND                                         |

### Code

```cpp
#include <Arduino.h>

#define TRIG_PIN D6  // GPIO12
#define ECHO_PIN D7  // GPIO13

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30ms
  float distance = (duration * 0.0343) / 2.0;     // cm
  return distance;
}

void loop() {
  float dist = measureDistance();
  if (dist > 0) {
    Serial.printf("Distance: %.1f cm\n", dist);
  } else {
    Serial.println("Out of range");
  }
  delay(500);
}
```

---

## 10. Wi-Fi — Connect to a Network

### Code

```cpp
#include <Arduino.h>
#include <ESP8266WiFi.h>

const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.printf("\nConnecting to %s", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi connected!");
  Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
  Serial.printf("Signal Strength (RSSI): %d dBm\n", WiFi.RSSI());
  Serial.printf("MAC Address: %s\n", WiFi.macAddress().c_str());
}

void loop() {}
```

---

## 11. Wi-Fi — Simple Web Server

### Code

```cpp
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

ESP8266WebServer server(80);

#define LED_PIN D1  // GPIO5

void handleRoot() {
  String html = "<!DOCTYPE html><html><head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1'>"
    "<title>NodeMCU Control</title></head><body>"
    "<h1>NodeMCU v2 Web Server</h1>"
    "<p><a href='/led/on'><button>LED ON</button></a></p>"
    "<p><a href='/led/off'><button>LED OFF</button></a></p>"
    "</body></html>";
  server.send(200, "text/html", html);
}

void handleLedOn() {
  digitalWrite(LED_PIN, HIGH);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleLedOff() {
  digitalWrite(LED_PIN, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nServer running at http://%s/\n",
                WiFi.localIP().toString().c_str());

  server.on("/", handleRoot);
  server.on("/led/on", handleLedOn);
  server.on("/led/off", handleLedOff);
  server.begin();
}

void loop() {
  server.handleClient();
}
```

---

## 12. Relay Module Control

### Wiring Diagram

```mermaid
graph LR
    subgraph NodeMCU_v2
        D5_R["D5 (GPIO14)"]
        GND["GND"]
        VIN_R["VIN (5V)"]
    end

    subgraph Relay_Module["Relay Module"]
        IN["IN (Signal)"]
        VCC_R["VCC"]
        GND_R["GND"]
        COM["COM"]
        NO_R["NO (Normally Open)"]
        NC_R["NC (Normally Closed)"]
    end

    D5_R -->|"Control Signal"| IN
    VIN_R --> VCC_R
    GND --- GND_R

    COM --- LOAD_A["Load Wire A"]
    NO_R --- LOAD_B["Load Wire B"]

    style Relay_Module fill:#520,stroke:#fff,color:#fff
```

### Connections

| Relay Module Pin | Connect To       |
| ---------------- | ---------------- |
| IN (signal)      | D5 (GPIO14)      |
| VCC              | VIN (5V)         |
| GND              | GND              |
| COM              | Load common      |
| NO               | Load switched    |

> **Safety:** Never handle mains-voltage wiring while the circuit is powered. Use appropriate-rated relays for high-voltage loads.

### Code

```cpp
#include <Arduino.h>

#define RELAY_PIN D5  // GPIO14

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // Most relay modules are active LOW
}

void loop() {
  Serial.println("Relay ON");
  digitalWrite(RELAY_PIN, LOW);   // Activate relay
  delay(3000);

  Serial.println("Relay OFF");
  digitalWrite(RELAY_PIN, HIGH);  // Deactivate relay
  delay(3000);
}
```

---

## 13. Buzzer — Tone Generation

### Wiring Diagram

```mermaid
graph LR
    subgraph NodeMCU_v2
        D8_B["D8 (GPIO15)"]
        GND["GND"]
    end

    D8_B -->|"PWM Signal"| BUZZ_P["Buzzer (+)"]
    BUZZ_N["Buzzer (-)"] --> GND

    style BUZZ_P fill:#fa0,stroke:#333
```

### Connections

| Buzzer Pin | Connect To    |
| ---------- | ------------- |
| + (positive)| D8 (GPIO15) |
| - (negative)| GND         |

### Code

```cpp
#include <Arduino.h>

#define BUZZER_PIN D8  // GPIO15

// Note frequencies (Hz)
#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C5 523

int melody[] = {
  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4,
  NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5
};
int noteDuration = 300;

void setup() {
  Serial.begin(115200);
}

void loop() {
  for (int i = 0; i < 8; i++) {
    tone(BUZZER_PIN, melody[i], noteDuration);
    delay(noteDuration + 50);
    noTone(BUZZER_PIN);
  }
  delay(2000);
}
```

---

## Full System Wiring Overview

Below is a combined diagram showing multiple components connected to a single NodeMCU v2 simultaneously:

```mermaid
graph TB
    subgraph NodeMCU_v2["NodeMCU v2"]
        direction LR
        V3["3.3V"]
        VIN["VIN (5V)"]
        GND["GND"]
        D1_PIN["D1 — GPIO5 (SCL)"]
        D2_PIN["D2 — GPIO4 (SDA)"]
        D3_PIN["D3 — GPIO0"]
        D4_PIN["D4 — GPIO2"]
        D5_PIN["D5 — GPIO14"]
        D6_PIN["D6 — GPIO12"]
        D7_PIN["D7 — GPIO13"]
        D8_PIN["D8 — GPIO15"]
        A0_PIN["A0 — ADC"]
    end

    D4_PIN -.-|"Onboard LED"| ONBOARD["Built-in LED"]
    D1_PIN -->|"I²C SCL"| OLED_SCL["OLED SCL"]
    D2_PIN -->|"I²C SDA"| OLED_SDA["OLED SDA"]
    D3_PIN -->|"Data"| DHT["DHT11/22"]
    D5_PIN -->|"Signal"| SERVO["Servo / Relay"]
    D6_PIN -->|"Trigger"| ULTRA_T["HC-SR04 TRIG"]
    D7_PIN -->|"Echo"| ULTRA_E["HC-SR04 ECHO"]
    D8_PIN -->|"PWM"| BUZZER["Buzzer"]
    A0_PIN -->|"Analog"| POT["Potentiometer"]

    V3 -->|"Power"| DHT
    V3 -->|"Power"| OLED_SCL
    VIN -->|"5V"| ULTRA_T
    VIN -->|"5V"| SERVO

    style NodeMCU_v2 fill:#135,stroke:#fff,color:#fff
```

---

## Tips & Troubleshooting

| Issue | Solution |
| ----- | -------- |
| Upload fails | Hold **FLASH** button during upload, or check USB cable (use data cable, not charge-only) |
| `GPIO0` stuck LOW at boot | D3 is pulled LOW — board enters flash mode. Avoid using D3 for inputs that are LOW at boot |
| `GPIO15` must be LOW at boot | D8 is pulled LOW at boot. Connecting certain modules may prevent boot |
| `GPIO2` must be HIGH at boot | D4 is pulled HIGH at boot. Don't pull it LOW externally during reset |
| Analog reads seem wrong | NodeMCU has onboard voltage divider on A0 — input range is 0–3.3V on the pin, 0–1V at chip |
| Wi-Fi won't connect | Double-check SSID/password; ensure 2.4 GHz network (ESP8266 does not support 5 GHz) |
| Brownouts / random resets | Use an external power supply; USB may not supply enough current for motors/relays |
| I²C device not found | Run an I²C scanner sketch to verify address; check SDA/SCL wiring |

---

*Last updated: 14 March 2026*
