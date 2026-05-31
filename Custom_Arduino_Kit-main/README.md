# Custom Arduino Robo Kit

Welcome to the **Custom Arduino Robo Kit** project! 
This kit allows you to control Arduino hardware (Sensors, LEDs, Motors, LCDs, etc.) directly using Python scripts.

## 🚀 Features
- **pyVArduino Library**: A custom Python library to talk to Arduino seamlessly over USB Serial.
- **Smart Door**: Ultrasonic sensor-based automated door using a Servo motor and Buzzer.
- **RGB Color Mixer**: 3-Potentiometer based RGB LED color mixing with real-time LCD display.
- **Sensor Readings**: DHT22 (Temp/Humidity), LDR (Light), IR Remote, Hall Sensor, etc.

## 🛠️ Setup Instructions
1. **Upload `All.ino`**: Open `All.ino` in Arduino IDE and upload it to your board.
2. **Install Python Library**: Run the `install_library.bat` file to install the `pyVArduino` library.
3. **Run Projects**: Open any Python file in the `application/` folder (like `smart_door.py` or `colorMixer3potsRGBLED.py`) and click Run!

## 📝 Developer Notes
*(यहाँ आप अपने प्रोजेक्ट्स, आइडियाज़, और कोडिंग से जुड़ी कोई भी ज़रूरी बातें लिख सकते हैं)*

- **Hardware Fixes Note**: 
  - RGB LED pins mapping: (Red & Blue LEDs are physically swapped on the PCB).
  - Potentiometer mapping: (Green & Blue Pots are physically swapped on the PCB).
  - The `All.ino` has been specially customized to avoid Timer conflicts between `Servo.h` and `IRremote.h`.

---
**Created by**: [Vikram / Your Name]
