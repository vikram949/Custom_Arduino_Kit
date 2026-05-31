#include <Arduino.h>
#include <DHT.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // Change if needed

// DHT Sensor
#define DHTPIN 12
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Servo
#define SERVO_PIN 6
Servo myServo;

// Analog Sensors    
#define LDR_PIN A2
#define HALL_PIN A0
#define IR_PIN A1
IRrecv irrecv(IR_PIN);
decode_results results;

#define POT1 A7
#define POT2 A6
#define POT3 A3

// Ultrasonic
#define TRIG_PIN 7
#define ECHO_PIN 8

// Buzzer & Relay
#define BUZZER_PIN 13
#define RELAY_PIN 5

// LEDs
#define LED_RED 9
#define LED_GREEN 10
#define LED_YELLOW 11

// Buttons
#define BTN1 3
#define BTN2 4
#define BTN3 2

// Motor
#define MOTOR_IN1 10
#define MOTOR_IN2 11
#define MOTOR_ENA 9

// Timing for DHT reading (to avoid frequent reads)
unsigned long lastDHTReadTime = 0;
const unsigned long DHTReadInterval = 2000; // 2 seconds

void setup() {
  Serial.begin(9600); 
  dht.begin();
  myServo.attach(SERVO_PIN);
  irrecv.enableIRIn();

  pinMode(LDR_PIN, INPUT);
  pinMode(HALL_PIN, INPUT);
  pinMode(POT1, INPUT);
  pinMode(POT2, INPUT);
  pinMode(POT3, INPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);

  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_ENA, OUTPUT);

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Ready");

  // Buzzer beep once at startup
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    // Analog sensors
    if (command == "LDR") {
      Serial.println(analogRead(LDR_PIN));
    }
    else if (command == "HALL") {
      Serial.println(analogRead(HALL_PIN));
    }
    else if (command == "POT1") {
      Serial.println(analogRead(POT1));
    }
    else if (command == "POT2") {
      Serial.println(analogRead(POT2));
    }
    else if (command == "POT3") {
      Serial.println(analogRead(POT3));
    }

    // Buttons
    else if (command == "BTN1") {
      Serial.println(digitalRead(BTN1));
    }
    else if (command == "BTN2") {
      Serial.println(digitalRead(BTN2));
    }
    else if (command == "BTN3") {
      Serial.println(digitalRead(BTN3));
    }

    // IR sensor
    else if (command == "IR") {
      if (irrecv.decode(&results)) {
        Serial.println(results.value, HEX);
        irrecv.resume();
      }
      else {
        Serial.println("0");
      }
    }

    // LEDs control
    else if (command == "LED_RED_ON") digitalWrite(LED_RED, HIGH);
    else if (command == "LED_RED_OFF") digitalWrite(LED_RED, LOW);
    else if (command == "LED_GREEN_ON") digitalWrite(LED_GREEN, HIGH);
    else if (command == "LED_GREEN_OFF") digitalWrite(LED_GREEN, LOW);
    else if (command == "LED_YELLOW_ON") digitalWrite(LED_YELLOW, HIGH);
    else if (command == "LED_YELLOW_OFF") digitalWrite(LED_YELLOW, LOW);

    // Buzzer & Relay control
    else if (command == "BUZZER_ON") digitalWrite(BUZZER_PIN, HIGH);
    else if (command == "BUZZER_OFF") digitalWrite(BUZZER_PIN, LOW);
    else if (command == "RELAY_ON") digitalWrite(RELAY_PIN, HIGH);
    else if (command == "RELAY_OFF") digitalWrite(RELAY_PIN, LOW);

    // Motor control
    else if (command == "MOTOR_FORWARD") {
      digitalWrite(MOTOR_IN1, HIGH);
      digitalWrite(MOTOR_IN2, LOW);
      analogWrite(MOTOR_ENA, 255);
    }
    else if (command == "MOTOR_BACKWARD") {
      digitalWrite(MOTOR_IN1, LOW);
      digitalWrite(MOTOR_IN2, HIGH);
      analogWrite(MOTOR_ENA, 255);
    }
    else if (command == "MOTOR_STOP") {
      digitalWrite(MOTOR_IN1, LOW);
      digitalWrite(MOTOR_IN2, LOW);
      analogWrite(MOTOR_ENA, 0);
    }

    // DHT reading with timing control
    else if (command == "DHT") {
      unsigned long now = millis();
      if (now - lastDHTReadTime < DHTReadInterval) {
        Serial.println("WAIT");
      } else {
        lastDHTReadTime = now;
        float h = dht.readHumidity();
        float t = dht.readTemperature();
        if (isnan(h) || isnan(t)) {
          Serial.println("ERROR");
        } else {
          Serial.print(t, 2);
          Serial.print(",");
          Serial.println(h, 2);
        }
      }
    }

    // Ultrasonic sensor
    else if (command == "ULTRASONIC") {
      digitalWrite(TRIG_PIN, LOW);
      delayMicroseconds(2);
      digitalWrite(TRIG_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIG_PIN, LOW);
      long duration = pulseIn(ECHO_PIN, HIGH);
      float distance = duration * 0.034 / 2;
      Serial.println(distance, 2);
    }

    // Servo control: expects "SERVO<angle>", e.g. "SERVO90"
    else if (command.startsWith("SERVO")) {
      int angle = command.substring(5).toInt();
      if (angle >= 0 && angle <= 180) {
        myServo.write(angle);
        Serial.println("OK");
      }
      else {
        Serial.println("INVALID");
      }
    }

    // LCD commands
    else if (command.startsWith("LCD_CURSOR")) {
      int i1 = command.indexOf(',');
      int i2 = command.indexOf(',', i1 + 1);
      if (i1 > 0 && i2 > i1) {
        int row = command.substring(i1 + 1, i2).toInt();
        int col = command.substring(i2 + 1).toInt();
        if ((row == 0 || row == 1) && (col >= 0 && col <= 15)) {
          lcd.setCursor(col, row);
          Serial.println("CURSOR OK");
        } else {
          Serial.println("INVALID");
        }
      }
      else {
        Serial.println("INVALID");
      }
    }
    else if (command.startsWith("LCD_CLEAR_LINE")) {
      int row = command.substring(14).toInt();
      if (row == 0 || row == 1) {
        lcd.setCursor(0, row);
        for (int i = 0; i < 16; i++) lcd.print(" ");
        lcd.setCursor(0, row);
        Serial.println("LINE CLEARED");
      }
      else {
        Serial.println("INVALID");
      }
    }
    else if (command.startsWith("LCD")) {
      String text = command.substring(3);
      lcd.print(text);
      Serial.println("LCD OK");
    }

    // Unknown command
    else {
      Serial.println("INVALID");
    }
  }
}
