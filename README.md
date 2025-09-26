# Robocon Natsu 2025 — ESP32 Robot Controller

ESP32‑based robot control firmware for our Robocon 2025 build. The main controller runs **Arduino on ESP32** (PlatformIO) and drives four CAN ESCs (DJI/RoboMaster C620 style) with closed‑loop speed control, reads a **PS4 controller** over Bluetooth, and exposes additional subsystems (intake, flywheel, lift, claw). An **SSD1306 OLED** is used for simple status/odometry display. A companion sketch (`nanoshooter/`) targets an **Arduino Nano** as a small **PWM bridge** for a REV Spark MAX, commanded over CAN.

---

## Highlights

- **Drive**: 4‑motor (M1…M4) holonomic drive with per‑wheel **PID** on RPM feedback read from ESCs over CAN (`0x201…0x204`).  
- **Input**: PS4 wireless controller (Bluetooth); **precision mode** and **front flip** supported.  
- **Actuators**: intake, flywheel (via PWM bridge), lift, and claw over CAN.  
- **Display**: 128×64 I²C SSD1306 (addr `0x3C`) for boot/status; optional odometry overlay.  
- **Modular code**: each subsystem lives in `include/` + `src/` pairs for quick hacking.

---

## Repo layout

```
Robocon Natsu 2025/
├─ platformio.ini                 # ESP32 build config (Arduino)
├─ include/ …                     # headers for each subsystem
├─ src/ …                         # implementations (main loop + modules)
│  ├─ main.cpp
│  ├─ MotorControl.*              # kinematics + CAN ESC speed commands (0x200)
│  ├─ MotorRPMReader.*            # reads ESC feedback frames (0x201–0x204)
│  ├─ PS4ControllerHandler.*      # maps sticks/buttons to set‑points
│  ├─ CANBusHandler.*             # MCP2515 (1 Mbps @ 8 MHz), CS=GPIO 27, INT=GPIO 35
│  ├─ DisplayHandler.*            # SSD1306 @ 0x3C via I²C
│  ├─ Odometry.*                  # 2‑pod quadrature odometry (interrupt‑based)
│  ├─ IntakeControl.*             # CAN group 5–8 (0x1FF), ID5 = intake
│  ├─ LiftControl.*               # CAN group 5–8 (0x1FF), ID7 = lift
│  ├─ ClawControl.*               # CAN group 5–8 (0x1FF), ID6 = claw
│  └─ FlywheelControl.*           # custom CAN cmd 0x301 to PWM bridge
├─ nanoshooter/                   # Arduino Nano PWM bridge for Spark MAX
│  ├─ platformio.ini              # Nano (ATmega328P) env
│  └─ src/main.cpp                # listens on 0x301 and sets PWM (kick‑start logic)
└─ test/                          # PlatformIO test scaffold
```

---

## Hardware overview

### Main controller (ESP32)
- **Board**: `board = esp32dev` (Arduino framework, PlatformIO).  
- **CAN**: MCP2515 (8 MHz crystal) via SPI @ **1 Mbps**; pins from `include/config.h`:
  - `canCs = 27` (CS), `canInt = 35` (INT). SPI uses ESP32 default pins (SCK=18, MOSI=23, MISO=19).
- **OLED**: SSD1306 128×64 @ `0x3C` (I²C; ESP32 defaults SCL=22, SDA=21).  
- **Buzzer**: `buzzerPin = 15`.  
- **ESCs**: RoboMaster C620 style:
  - **Tx** speed group: `0x200` (M1/M2/M3/M4 high/low bytes in order).
  - **Rx** feedback: `0x201…0x204` (per‑motor RPM).  
  - Aux group (5–8) at `0x1FF`: ID5=intake, ID6=claw, ID7=lift, ID8=unused (as coded).

### PWM bridge (Arduino Nano) — `nanoshooter/`
- **Purpose**: bridge CAN command `0x301` → PWM to **REV Spark MAX**.
- **Protocol** (from `nanoshooter/src/main.cpp`):
  - Receives `0x301` with first byte command: `0x00`=stop, `0x01`≈1700 µs, `0x02`≈1800 µs (plus non‑blocking “kick‑start”).  
- **Wiring (Nano ↔ MCP2515)**: D13=SCK, D12=MISO, D11=MOSI, D10=CS. Spark MAX PWM on D3.

> If you use MCP2515 boards with **16 MHz** crystals, adjust the library init or bitrate accordingly.

---

## Controls (PS4)

Sticks:
- **Left‑Y** → forward/backward, **Left‑X** → strafe, **Right‑X** → yaw.

Modes:
- **D‑pad Up** = normal front, **D‑pad Down** = **reverse front** (flips drive mapping).
- **D‑pad Right** = **precision mode** (scales wheel outputs by 50%).

Actuators (when PS4 connected):
- **Lift**: **L2** up / **L1** down (active when **front is NOT reversed**).
- **Claw**: **R2** open / **R1** close (active when **front is NOT reversed**).
- **Intake**: uses **R2 / R1** when **front IS reversed** (see `src/IntakeControl.cpp`).
- **Flywheel** (via PWM bridge): face buttons; e.g. **Triangle ⇒ command 0x01**; other buttons map similarly (`src/FlywheelControl.cpp`).

> Exact button bindings and speed limits are visible in each `*Control.cpp` file.

---

## PID & tuning

- PID gains live in `include/config.h`:
  ```cpp
  const float kP = 0.46, kI = 0.034, kD = 0.15;
  const int maxSpeed = 8000;  // overall set‑point clamp
  ```
- Anti‑windup: integrator clamped to ±5000 inside `src/pidControl.cpp`.
- Call `resetPid()` if you add stateful mode switches that should clear the controller.

---

## Building & flashing (ESP32)

1. **Install** [PlatformIO](https://platformio.org/install) (VS Code or CLI).  
2. Open the folder `Robocon Natsu 2025/` and review `platformio.ini`:
   ```ini
; PlatformIO Project Configuration File
;
;   Build options: build flags, source filter
;   Upload options: custom upload port, speed and extra flags
;   Library options: dependencies, extra library storages
;   Advanced options: extra scripting
;
; Please visit documentation for the other options and examples
; https://docs.platformio.org/page/projectconf.html

[env:nodemcu-32s]
platform = espressif32
board = esp32dev
framework = arduino
upload_port = COM4
platform_packages = 
	platformio/framework-arduinoespressif32 @ 3.20005.220925
	platformio/toolchain-xtensa32          @ 2.80400.210211
lib_deps = 
	coryjfowler/mcp_can@^1.5.1
	pablomarquez76/PS4_Controller_Host@^1.0.9
	madhephaestus/ESP32Servo@^1.1.4
	mike-matera/FastPID@^1.3.1
	adafruit/Adafruit GFX Library@^1.11.9
	adafruit/Adafruit SSD1306@^2.5.10
	jandelgado/JLed@^4.13.1
monitor_speed = 115200
   ```
   - Change `upload_port` from `COM4` to your device port (e.g., `/dev/ttyUSB0` on Linux/macOS).
3. **Build**: `PlatformIO: Build` (or `pio run`).  
4. **Flash**: `PlatformIO: Upload` (or `pio run -t upload`).  
5. **Monitor**: 115200 baud.

### PS4 pairing tips
- Firmware calls `removePairedDevices()` on boot and prints the ESP32 **BT MAC**.  
- Put the PS4 pad in pairing (hold **Share + PS**) and connect using the **PS4_Controller_Host** flow. If pairing fails, reboot the ESP32 so it re‑advertises, and try again.

---

## Building & flashing (Nano PWM bridge)

1. Open `nanoshooter/` in PlatformIO.  
2. Confirm the environment in `project4/platformio.ini` (`nanoatmega328`, 57600 upload, 115200 monitor).  
3. Wire Nano ↔ MCP2515 as listed above, connect the Spark MAX PWM to D3, and **upload**.  
4. When the ESP32 publishes `0x301` commands, the Nano will set the PWM and apply a brief kick‑start to overcome motor stiction.

---

## Odometry (optional)

`src/Odometry.cpp` implements **two‑pod** quadrature odometry using GPIO interrupts. In `src/main.cpp` these calls are present:
```cpp
beginOdometry();           // in setup()
// pollOdometry();         // in loop() — uncomment to integrate each pass
```
There’s a commented display snippet to print `X/Y/heading` at ~10 Hz. Enable when you wire encoder pods and want pose feedback.

---

## Pin & wiring quick reference

- **ESP32**: `canCs=27`, `canInt=35`, `buzzer=15`, I²C OLED at `0x3C`.  
- **CAN ESCs**: drive motors M1–M4 use Tx `0x200` and Rx `0x201–0x204`.  
- **Aux ESCs** (`0x1FF`): ID5=intake, ID6=claw, ID7=lift, ID8=unused (by default).  
- **PWM bridge**: Nano D3 → Spark MAX PWM; Nano ↔ MCP2515 SPI on D10–D13.

---

## Troubleshooting

- **No CAN traffic**: verify MCP2515 **8 MHz** vs **16 MHz**, CS=27 wiring, INT=35 pulled up, and 120 Ω termination on the bus.  
- **ESCs don’t spin**: check you’re sending to `0x200` and your ESC CAN IDs are 1–4; confirm supply/battery is on.  
- **PS4 not pairing**: reboot ESP32, watch serial for MAC, hold **Share + PS** to pair.  
- **OLED blank**: confirm `0x3C`, SDA/SCL lines, and that `initDisplay()` returns `true`.  
- **Jittery motion**: re‑tune `kP/kI/kD`, ensure **motorRPMData** is updating, and consider raising `maxSpeed` only after tuning.

---

## Contributing

- Keep one logical subsystem per `*Control.cpp`.  
- Prefer small PRs and add a short test or a serial log snippet that proves the behavior.  
- Document any new CAN IDs or button bindings at the top of the file you touch.

---

## License

MIT License

---

## Acknowledgments

- MCP2515 CAN library: **coryjfowler/mcp_can**  
- PS4 controller host: **pablomarquez76/PS4_Controller_Host**  
- OLED: **Adafruit GFX** + **Adafruit SSD1306**  
- PID: **mike‑matera/FastPID** (currently custom PID used), LEDs: **JLed**, servo: **ESP32Servo**.

