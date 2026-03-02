#include <Arduino.h>

// ==================== GPIO Configuration ====================
#define LED_PIN 2           // Status LED on GPIO2
#define RELAY1_PIN 4        // Relay 1
#define RELAY2_PIN 16       // Relay 2
#define RELAY3_PIN 17       // Relay 3
#define SWITCH1_PIN 34      // Switch 1
#define SWITCH2_PIN 35      // Switch 2
#define SWITCH3_PIN 32      // Switch 3

// ==================== LED Status Modes ====================
enum LEDStatus {
  LED_OFF = 0,            // ปิด (อุปกรณ์ไม่ทำงาน)
  LED_ON = 1,             // เปิด (อุปกรณ์ทำงานปกติ)
  LED_SLOW_BLINK = 2,     // กระพริบช้า (กำลังรอสัญญาณ)
  LED_FAST_BLINK = 3,     // กระพริบเร็ว (เตือน)
  LED_VERY_FAST_BLINK = 4 // กระพริบเร็วมาก (Error)
};

// ==================== Global Variables ====================
LEDStatus currentStatus = LED_ON;
unsigned long lastBlinkTime = 0;
bool ledState = false;
int blinkInterval = 1000;  // Default interval in milliseconds

// ==================== Function Prototypes ====================
void initializePins();
void updateLED();
void setLEDStatus(LEDStatus status);
void handleSwitches();
void printStatus();

// ==================== Setup Function ====================
void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  delay(1000);  // Wait for Serial to initialize
  
  Serial.println("\n\n");
  Serial.println("===========================================");
  Serial.println("      mySmartDevice - Initializing");
  Serial.println("===========================================");
  
  // Initialize GPIO pins
  initializePins();
  
  // Set initial LED status
  setLEDStatus(LED_ON);
  
  Serial.println("✓ Initialization complete!");
  Serial.println("===========================================\n");
}

// ==================== Loop Function ====================
void loop() {
  // Update LED status without blocking
  updateLED();
  
  // Handle switch inputs
  handleSwitches();
  
  // Print status periodically
  static unsigned long lastPrintTime = 0;
  if (millis() - lastPrintTime >= 5000) {  // Print every 5 seconds
    printStatus();
    lastPrintTime = millis();
  }
  
  // Non-blocking loop (no delay!)
}

// ==================== Initialize Pins ====================
void initializePins() {
  Serial.println("Initializing GPIO pins...");
  
  // LED Output
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.println("  ✓ LED (GPIO2) as Output");
  
  // Relay Outputs (Active Low)
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);
  digitalWrite(RELAY3_PIN, HIGH);
  Serial.println("  ✓ Relay 1-3 (GPIO4, 16, 17) as Output");
  
  // Switch Inputs (Active Low with Pull-up)
  pinMode(SWITCH1_PIN, INPUT_PULLUP);
  pinMode(SWITCH2_PIN, INPUT_PULLUP);
  pinMode(SWITCH3_PIN, INPUT_PULLUP);
  Serial.println("  ✓ Switch 1-3 (GPIO34, 35, 32) as Input");
}

// ==================== Update LED - Non-Blocking ====================
void updateLED() {
  unsigned long currentTime = millis();
  
  switch (currentStatus) {
    case LED_OFF:
      // LED completely off
      digitalWrite(LED_PIN, LOW);
      break;
      
    case LED_ON:
      // LED always on
      digitalWrite(LED_PIN, HIGH);
      break;
      
    case LED_SLOW_BLINK:
      // Slow blink: 1000ms interval
      if (currentTime - lastBlinkTime >= 700) {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
        lastBlinkTime = currentTime;
      }
      break;
      
    case LED_FAST_BLINK:
      // Fast blink: 300ms interval
      if (currentTime - lastBlinkTime >= 350) {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
        lastBlinkTime = currentTime;
      }
      break;
      
    case LED_VERY_FAST_BLINK:
      // Very fast blink: 100ms interval (Error mode)
      if (currentTime - lastBlinkTime >= 200) {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
        lastBlinkTime = currentTime;
      }
      break;
  }
}

// ==================== Set LED Status ====================
void setLEDStatus(LEDStatus status) {
  currentStatus = status;
  lastBlinkTime = millis();
  ledState = false;
  digitalWrite(LED_PIN, LOW);
  
  Serial.print("Status changed to: ");
  switch (status) {
    case LED_OFF:
      Serial.println("OFF (Device inactive)");
      break;
    case LED_ON:
      Serial.println("ON (Device active)");
      break;
    case LED_SLOW_BLINK:
      Serial.println("SLOW BLINK (Waiting for signal)");
      break;
    case LED_FAST_BLINK:
      Serial.println("FAST BLINK (Warning)");
      break;
    case LED_VERY_FAST_BLINK:
      Serial.println("VERY FAST BLINK (Error/Critical)");
      break;
  }
}

// ==================== Handle Switch Inputs ====================
void handleSwitches() {
  static unsigned long lastDebounceTime = 0;
  static unsigned long debounceDelay = 50;  // 50ms debounce
  
  // Check switches with debouncing
  if (millis() - lastDebounceTime > debounceDelay) {
    // Switch 1 - Toggle Relay 1 and change LED status
    if (digitalRead(SWITCH1_PIN) == LOW) {
      Serial.println(">> SW1 Pressed - Toggling Relay 1 & LED Status");
      digitalWrite(RELAY1_PIN, digitalRead(RELAY1_PIN) == LOW ? HIGH : LOW);
      
      // Cycle through LED statuses
      LEDStatus nextStatus = (LEDStatus)((currentStatus + 1) % 5);
      setLEDStatus(nextStatus);
      
      lastDebounceTime = millis();
      while (digitalRead(SWITCH1_PIN) == LOW) {
        delay(10);  // Wait for button release
      }
    }
    
    // Switch 2 - Toggle Relay 2
    if (digitalRead(SWITCH2_PIN) == LOW) {
      Serial.println(">> SW2 Pressed - Toggling Relay 2");
      digitalWrite(RELAY2_PIN, digitalRead(RELAY2_PIN) == LOW ? HIGH : LOW);
      lastDebounceTime = millis();
      while (digitalRead(SWITCH2_PIN) == LOW) {
        delay(10);  // Wait for button release
      }
    }
    
    // Switch 3 - Toggle Relay 3
    if (digitalRead(SWITCH3_PIN) == LOW) {
      Serial.println(">> SW3 Pressed - Toggling Relay 3");
      digitalWrite(RELAY3_PIN, digitalRead(RELAY3_PIN) == LOW ? HIGH : LOW);
      lastDebounceTime = millis();
      while (digitalRead(SWITCH3_PIN) == LOW) {
        delay(10);  // Wait for button release
      }
    }
  }
}

// ==================== Print Status Information ====================
void printStatus() {
  unsigned long uptime = millis() / 1000;
  
  Serial.println("\n--- Device Status Report ---");
  Serial.print("Uptime: ");
  Serial.print(uptime);
  Serial.println(" seconds");
  
  Serial.print("LED Status: ");
  switch (currentStatus) {
    case LED_OFF:
      Serial.println("OFF");
      break;
    case LED_ON:
      Serial.println("ON (steady)");
      break;
    case LED_SLOW_BLINK:
      Serial.println("SLOW BLINK");
      break;
    case LED_FAST_BLINK:
      Serial.println("FAST BLINK");
      break;
    case LED_VERY_FAST_BLINK:
      Serial.println("VERY FAST BLINK");
      break;
  }
  
  Serial.print("Relay 1: ");
  Serial.println(digitalRead(RELAY1_PIN) == LOW ? "ON (Active)" : "OFF");
  Serial.print("Relay 2: ");
  Serial.println(digitalRead(RELAY2_PIN) == LOW ? "ON (Active)" : "OFF");
  Serial.print("Relay 3: ");
  Serial.println(digitalRead(RELAY3_PIN) == LOW ? "ON (Active)" : "OFF");
  
  Serial.println("-----------------------------\n");
}