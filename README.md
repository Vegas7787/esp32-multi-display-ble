# esp32-multi-display-ble

# 🚀 ESP32 Multi-Display BLE Controller (E-Ink + Serial Protocol)

## 📌 Overview

This project is a custom ESP32 firmware that combines:

* 📡 **Custom UART protocol**
* 🖥️ **Multi E-Ink display control (3 displays)**
* 📲 **BLE keyboard emulation**
* ⚡ **Real-time multitasking (FreeRTOS)**

The device receives commands and images via Serial, updates E-Ink displays, and triggers BLE keyboard actions using physical buttons.

---

## ⚙️ Features

* 📡 Binary UART protocol with checksum validation
* 🖼️ Image upload and rendering on **3 independent E-Ink displays**
* 🎮 Button-triggered BLE keyboard actions
* ⚡ Dual-core task handling (Serial + BLE)
* 🧩 Modular and scalable architecture

---

## 🏗️ System Architecture

### Tasks (FreeRTOS)

* **Core 0 → Serial Task**

  * Handles UART communication
  * Parses incoming packets
  * Updates images and actions

* **Core 1 → BLE Task**

  * Handles button input
  * Sends BLE keyboard events

---

### Main Components

* **Protocol Parser (State Machine)**
* **Display Manager (3 displays)**
* **BLE Keyboard Controller**
* **Input Handling (buttons)**

---

## 📡 Communication Protocol

Custom binary protocol:

| Field    | Size | Description       |
| -------- | ---- | ----------------- |
| Header   | 2B   | 0xAA 0x55         |
| Type     | 1B   | Command type      |
| Size     | 4B   | Payload size      |
| Data     | N    | Payload           |
| Checksum | 1B   | Sum of data bytes |

---

### 📦 Packet Types

#### 🔹 Type 0xF* - Sending New Data

#### 🔹 Type 0x0* - Sending Again

#### 🔹 Type 0x*0 - Select Display

* Sets active display (key_id)

#### 🔹 Type 0x01 - Image Data

* Sends image bytes to display buffer

#### 🔹 Type 0x02 - Action Data

* Defines BLE keyboard action (key sequence)

---

### ✅ Response Codes

* `0x06` → ACK (valid packet)
* `0x15` → NACK (invalid checksum)

---

## 🖥️ Display System

* 3 independent E-Ink displays
* Each display has:

  * its own SPI pins
  * image buffer
  * button
  * action (key sequence)

### Update Flow:

1. Select display
2. Load image buffer
3. Trigger update
4. Wait for BUSY
5. Enter deep sleep

---

## 📲 BLE Keyboard Logic

* Uses BLE HID keyboard
* Supports:

  * single key press
  * key combinations (e.g. Ctrl + C)

### Behavior:

* Button press triggers assigned action
* Debounce logic (200ms)
* Proper key press/release handling

---

## 🔘 Input Handling

Each display has a dedicated button:

* Press → triggers BLE action
* Supports:

  * single key
  * multi-key combinations

---

## ⚙️ Hardware

* ESP32
* 3 × E-Ink displays
* 3 × buttons
* SPI interface
* BLE-compatible host (PC / phone)

---

## ▶️ How to Run

1. Build & upload:

```bash
pio run --target upload
```

2. Open Serial Monitor:

```bash
pio device monitor
```

3. Send binary packets via UART

---

## 🧪 Example Use Cases

* Custom macro keyboard
* Smart control panel
* IoT dashboard with buttons
* Remote-controlled display system

---

## ⚠️ Challenges

* Reliable UART parsing with partial data
* Synchronizing image transfer
* Handling corrupted packets
* Managing multiple displays over SPI

---

## ✅ Solutions

* Implemented **state machine parser**
* Added **checksum validation**
* Used **FreeRTOS tasks on separate cores**
* Structured display handling with objects

---

## 📈 Why this project matters

This project demonstrates:

* low-level embedded programming (C++)
* real-time systems (FreeRTOS)
* custom communication protocol design
* BLE HID integration
* multi-device hardware control

---

## 🎥 Demo

(Add video or GIF here)

---

## 🧑‍💻 Author

Your Name
