# Image Transfer Via Visual Light Communication

An end-to-end ecosystem for wireless communication using light. This project enables two Arduino Uno units to exchange text and image data across a laser beam. It includes dual-protocol firmware (Blink-Count and International Morse Code) and a Python-based utility suite for converting binary images into transmittable Base64 text strings.

---

##  System Architecture

The project is divided into two primary layers:
1.  **Hardware Layer (Physical):** Arduino-controlled Laser Diode (Transmitter) and BPW34 Photodiode (Receiver).
2.  **Software Layer (Data Processing):** Python scripts to handle binary-to-text conversion, allowing "image-over-light" capabilities.



---

## 🛠️ Hardware Requirements & Pinout

### Components Needed:
* **2x** Arduino Uno (or compatible)
* **2x** 5V Laser Diode
* **2x** BPW34 Photodiode (High-speed silicon PIN photodiode)
* **2x** Push Button (Mode Switch)
* **2x** LED (Status Indicator)
* **Resistors:** 220Ω (for Laser/LED) and 10kΩ (Pull-down for Photodiode)

### Wiring Table:
| Component | Arduino Pin | Configuration |
| :--- | :--- | :--- |
| **Laser Diode** | `9` | Output (PWM) |
| **Photodiode** | `A0` | Analog Input |
| **Mode Button** | `2` | Digital Input (Interrupt) |
| **Status LED** | `6` | Digital Output |

---

##  Communication Protocols

This repository provides two different methods of encoding data into light pulses.

### 1. International-Morse-Code 
The preferred method for alphanumeric data. It uses timing ratios to distinguish between short and long pulses.
* **Dit (`.`):** 200ms
* **Dah (`-`):** 600ms (3x Dit)
* **Character Gap:** 600ms
* **Word Gap:** 1400ms

( `International-Morse-Code.ino` contains all the arduino code for this architecture )


### 2. Blink-Count 
A simplified tally-based system where the number of pulses directly corresponds to a character's position in the alphabet.
* **'a'** = 1 blink | **'b'** = 2 blinks | **'z'** = 26 blinks.
* **Space/Punctuation:** Handled by 27–41 blinks.

( `blink-count.ino` contains all the arduino code for this architecture )

---

##  Python Image Processing Suite

To send images via light, the binary data must be converted into a format that can be sent through a Serial Monitor. These scripts facilitate that "round-trip" process.

### `image_converter1.py`
This script reads a local image file and converts it into a Base64 encoded string.
* **Input:** `.png`, `.jpg`, `.bmp`
* **Output:** A `.txt` file containing the Base64 representation.
* **Usage:** Perfect for generating the payload to be pasted into the Arduino Serial TX window.

### `base64_converter2.py`
The "De-coder" script that reconstructs the image on the receiving end.
* **Input:** Base64 text string.
* **Output:** A reconstructed `.png` file saved to your local directory.

---

##  Step-by-Step Operation Guide

### 1. Preparation & Alignment
On power-up, the system enters **Alignment Mode** for 5-10 seconds. The laser will stay active.
* Physically align the laser beam so it hits the center of the BPW34 photodiode.
* Open the Serial Monitor (9600 Baud) to verify the "Ready" message.

### 2. Calibrating the Threshold
Because room lighting varies, you must calibrate the `THRESHOLD` value in the Arduino code:
* Check the analog readings in the Serial Plotter.
* Set the `THRESHOLD` to a value higher than your room's ambient light but lower than the laser's direct intensity.

### 3. Transmitting an Image
1.  Run `image_converter1.py` on your PC to get the Base64 string of your target image.
2.  Press the **Mode Button** on the sending Arduino until the **Status LED** turns ON (TX Mode).
3.  Paste the Base64 string into the Serial Monitor and press Enter.

### 4. Receiving & Reconstructing
1.  Ensure the receiving Arduino is in **RX Mode** (Status LED OFF).
2.  Once the transmission finishes, copy the text from the RX Serial Monitor.
3.  Run `base64_converter2.py` and paste the string to recreate the original image.

---

## ⚠️ Performance Limitations & Accuracy
* **Atmospheric Interference:** Dust or smoke can scatter the laser, leading to "bit errors" (e.g., a Dah being read as a Dit).
* **Data Size:** Base64 increases file size by roughly 33%. For long transmissions, ensure the laser and sensor are mounted on stable surfaces to prevent vibration-induced data loss.
* **Ambient Light:** BPW34 photodiodes are sensitive. Avoid operating in direct sunlight; a dark or indoor environment provides the best signal-to-noise ratio.

---

## 📄 License
Distributed under the MIT License. See `LICENSE` for more information.
