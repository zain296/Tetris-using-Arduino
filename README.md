# 🟥 Arduino Tetris — 8×32 LED Matrix

A fully playable Tetris game running on an **Arduino Uno** with a **8×32 MAX7219 LED matrix**, 4 push buttons,
and a passive buzzer. No display shield needed — just raw LEDs and real-time gameplay.


## 🧰 Hardware Required

| Component                      | Quantity  | Notes |

| Arduino Uno                    | 1         | Any 5V AVR-based board works |
| MAX7219 LED Matrix (8×8, FC16) | 4 modules | Chained together = 8×32 display |
| Push buttons                   | 4         | Momentary NO type |
| Passive buzzer                 | 1         | NOT active — needs tone() signal |
| Jumper wires                   | ~20       | Male-to-male + male-to-female |
| Breadboard                     | 1         | |


## 🔌 Wiring

### LED Matrix (SPI)

| MAX7219 Pin | Arduino Pin |

| VCC | 5V |
| GND | GND |
| DIN | D11 (MOSI) |
| CLK | D13 (SCK) |
| CS  | D10 |

> Chain the 4 modules together: DOUT of module 1 → DIN of module 2, and so on. CS and CLK are shared across all 4.

### Buttons (INPUT_PULLUP — connect to GND when pressed)

| Button | Arduino Pin |

| Move Left | D3 |
| Move Right | D4 |
| Rotate | D5 |
| Drop / Restart | D6 |

> Buttons use `INPUT_PULLUP` — one leg to the Arduino pin, other leg to **GND**. No resistors needed.

### Buzzer

| Buzzer Pin | Arduino Pin |

| + (signal) | D8 |
| − (ground) | GND |

> Use a **passive** buzzer. Active buzzers will only produce a constant tone.

---

## 📦 Dependencies

Install these via the Arduino Library Manager (**Sketch → Include Library → Manage Libraries**):

| Library | Author | Purpose |

| `MD_MAX72XX` | MajicDesigns | LED matrix driver for MAX7219 |
| `SPI`        | Arduino (built-in) | Hardware SPI communication |

---

## ⚙️ Configuration

At the top of the `.ino` file, adjust these defines to match your hardware:

```cpp
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW  // Change if your matrix type differs
#define MAX_DEVICES   4                    // Number of 8×8 modules chained

#define CS_PIN     10   // Chip select
#define BUZZER_PIN  8   // Passive buzzer

#define LEFT_BTN    3
#define RIGHT_BTN   4
#define ROTATE_BTN  5
#define DROP_BTN    6
```

**Matrix orientation:** If the display appears upside-down or mirrored, try changing `HARDWARE_TYPE` to `MD_MAX72XX::DR0_HW`, `DR1_HW`, or `PAROLA_HW`.

---

## 🚀 Installation

1. Clone or download this repository
2. Open `tetris.ino` in the Arduino IDE
3. Install the `MD_MAX72XX` library (see Dependencies above)
4. Select your board: **Tools → Board → Arduino Uno**
5. Select your port: **Tools → Port → COMx / /dev/ttyUSBx**
6. Click **Upload**

---

## 🎮 Controls

| Button | Action |

| **Left** | Move piece left |
| **Right** | Move piece right |
| **Rotate** | Rotate piece clockwise |
| **Drop** (hold) | Fast drop |
| **Drop** (on game over) | Restart game |

---

## 🔊 Sound Effects

| Event | Frequency | Duration |

| Rotate piece | 700 Hz | 60 ms |
| Clear line | 1000 Hz | 120 ms |
| Game over (high) | 400 Hz | 200 ms |
| Game over (low) | 200 Hz | 400 ms |

---

## 🧠 How It Works

### Game Board
The board is stored as `byte gameBoard[32][8]` — a 2D array of `0` (empty) or `1` (filled). Each tick, the display is redrawn from scratch by calling `mx.setPoint()` for every active cell.

### Pieces
All 7 tetrominoes (I, O, T, S, Z, J, L) are stored as 4-byte bitmasks in a `shapes[7][4]` array. Each byte represents one row of the 4×4 bounding box.

### Rotation
Rotation uses a standard 2D matrix transpose + mirror. A wall-kick check shifts the piece ±1 on failure before giving up.

### Speed Scaling
The game starts at **400ms per tick** and decreases by **5ms every time a piece locks**, down to a floor of **100ms**. This creates natural difficulty progression.

### Line Clearing
After each piece locks, the code scans every row bottom-to-top. A full row is removed by shifting all rows above it down by one.

---

## 📁 File Structure

```
arduino-tetris/
├── tetris.ino       ← Main game file (everything is here)
└── README.md        ← This file
```

---

## 🛠️ Possible Improvements

- [ ] Add per-tetromino color with an RGB LED matrix (WS2812 / NeoPixel)
- [ ] Score display on a second matrix or 7-segment display
- [ ] EEPROM high-score saving
- [ ] Next-piece preview panel (second small display)
- [ ] Bluetooth input via HC-05 for wireless controller

---

## 📜 License

MIT License — free to use, modify, and share. A credit back to this repo is appreciated but not required.

---

## 🤝 Contributing

Pull requests are welcome! Please open an issue first to discuss major changes.

---

*Built with ❤️ on an Arduino Uno. Because Tetris should run everywhere.*
By ZAIN TARIQ
