# mySmartDevice - ESP32 Smart Control System

คู่มือการใช้งาน Smart Device บนพื้นฐาน ESP32 DevKit V2

## 📋 สารบัญ
- [ข้อมูลทั่วไป](#ข้อมูลทั่วไป)
- [กำลังไฟและการเชื่อมต่อ](#กำลังไฟและการเชื่อมต่อ)
- [โหลดควบคุม (Relay)](#โหลดควบคุม-relay)
- [ปุ่มสวิตช์ (Switch)](#ปุ่มสวิตช์-switch)
- [อินพุตแยกไฟ (Isolated Input)](#อินพุตแยกไฟ-isolated-input)
- [จอแสดงผล OLED](#จอแสดงผล-oled)
- [การสื่อสาร RS485](#การสื่อสาร-rs485)
- [การติดตั้งและ Setup](#การติดตั้งและ-setup)

---

## ข้อมูลทั่วไป

### Specifications
| รายการ | ข้อมูล |
|--------|--------|
| **Microcontroller** | ESP32 |
| **Board** | DevKit V2 |
| **Processor** | Dual-core Xtensa 32-bit CPU |
| **RAM** | 520 KB SRAM |
| **Flash** | 4 MB |
| **WiFi** | IEEE 802.11 b/g/n |
| **Bluetooth** | BLE (Bluetooth Low Energy) |

### LED สถานะ
- **GPIO2**: Status LED - ใช้สำหรับแสดงสถานะบอร์ด

---

## กำลังไฟและการเชื่อมต่อ

### ข้อกำหนดแรงดัน
| ส่วนประกอบ | แรงดัน | หมายเหตุ |
|-----------|--------|---------|
| ESP32 Core | 3.3V | ห้ามจ่าย 5V ห่อมกับ voltage regulator |
| OLED Display | 3.3V | ไม่รองรับ 5V |
| RS485 Module | 5V (ปกติ) | MAX13487 ทำงานที่ 5V |
| Relay Coil | 5V | ขึ้นอยู่กับ relay module |
| Isolated Input | 10-24V | TTL compatible |

⚠️ **สำคัญ**: ใช้ 5V power supply ที่มีความจุเพียงพอสำหรับ relay และ RS485 module

---

## โหลดควบคุม (Relay)

ทั้ง 3 relay ใช้ logic **Active Low** (มีสัญญาณต่ำเพื่อเปิด)

### Relay Pin Assignment

| Relay | GPIO | การควบคุม | ฟังก์ชัน |
|-------|------|---------|---------|
| **Relay 1** | GPIO4 | Active Low | ควบคุมอุปกรณ์ที่ 1 |
| **Relay 2** | GPIO16 | Active Low | ควบคุมอุปกรณ์ที่ 2 |
| **Relay 3** | GPIO17 | Active Low | ควบคุมอุปกรณ์ที่ 3 |

### วิธีใช้ใน Code

```cpp
// ตั้งค่า GPIO เป็น Output
pinMode(GPIO4, OUTPUT);
pinMode(GPIO16, OUTPUT);
pinMode(GPIO17, OUTPUT);

// เปิด Relay (Active Low = ให้สัญญาณต่ำ)
digitalWrite(GPIO4, LOW);   // เปิด Relay 1
delay(100);

// ปิด Relay (Active Low = ให้สัญญาณสูง)
digitalWrite(GPIO4, HIGH);  // ปิด Relay 1
```

---

## ปุ่มสวิตช์ (Switch)

ทั้ง 3 สวิตช์ใช้ logic **Active Low** พร้อม Pull-up Resistor (10kΩ)

### Switch Pin Assignment

| Switch | GPIO | Pull-up | การใช้งาน |
|--------|------|---------|---------|
| **SW1** | GPIO34 | 10kΩ | Local Switch 1 |
| **SW2** | GPIO35 | 10kΩ | Local Switch 2 |
| **SW3** | GPIO32 | 10kΩ | Local Switch 3 |

### วิธีใช้ใน Code

```cpp
// ตั้งค่า GPIO เป็น Input
pinMode(GPIO34, INPUT_PULLUP);
pinMode(GPIO35, INPUT_PULLUP);
pinMode(GPIO32, INPUT_PULLUP);

// อ่านค่าจากปุ่ม
if (digitalRead(GPIO34) == LOW) {
    Serial.println("SW1 pressed!");
    // เปิด/ปิด Relay 1
    digitalWrite(GPIO4, digitalRead(GPIO4) == LOW ? HIGH : LOW);
}
```

---

## อินพุตแยกไฟ (Isolated Input)

ใช้สำหรับอ่านสัญญาณ 10-24V TTL จากเซนเซอร์หรืออุปกรณ์ภายนอก

### Isolated Input Assignment

| Input | GPIO | Voltage Range | Logic |
|-------|------|---------------|-------|
| **ISO1** | GPIO33 | 10-24V TTL | Active Low |
| **ISO2** | GPIO27 | 10-24V TTL | Active Low |

### วิธีใช้ใน Code

```cpp
// ตั้งค่า GPIO เป็น Input
pinMode(GPIO33, INPUT);
pinMode(GPIO27, INPUT);

// อ่านสถานะอินพุต
if (digitalRead(GPIO33) == LOW) {
    Serial.println("ISO1 is active!");
}
```

---

## จอแสดงผล OLED

### ข้อมูลจอ
| รายการ | ข้อมูล |
|--------|--------|
| **ประเภท** | OLED SSD1306 |
| **ขนาด** | 0.96" หรือ 1.3" |
| **ความละเอียด** | 128x64 หรือ 128x32 px |
| **I2C Address** | 0x3C (บางรุ่น 0x3D) |
| **แรงดัน** | 3.3V |
| **โปรโตคอล** | I2C |

### การต่อเชื่อม

| OLED Pin | ESP32 GPIO | ลักษณะ |
|----------|-----------|--------|
| **VCC** | 3.3V | Power |
| **GND** | GND | Ground |
| **SDA** | GPIO21 | I2C Data |
| **SCL** | GPIO22 | I2C Clock |
| **RST** | GPIO5 | Reset (optional) |

### วิธีใช้ใน Code

```cpp
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     5

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
    // เริ่มต้น I2C
    Wire.begin(21, 22); // SDA=21, SCL=22
    
    // เริ่มต้น OLED
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        while(1);
    }
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Smart Device Ready!");
    display.display();
}
```

---

## การสื่อสาร RS485

ใช้สำหรับการสื่อสารตัวอักษรระยะไกล กับอุปกรณ์ Modbus या Protocol อื่น

### โมดูล RS485
- **IC**: MAX13487 (Auto Direction Control)
- **ฟังก์ชัน**: แปลง UART เป็น RS485 half-duplex

### การต่อเชื่อม

| ESP32 (UART0) | MAX13487 | หมายถึง |
|---------------|----------|--------|
| TX0 (GPIO1) | DI | Data Input |
| RX0 (GPIO3) | RO | Receive Output |
| GND | GND | Ground |
| 5V | VCC | Power |
| GPIO (RTS) | DE/RE | Direction Control |

### สัญญาณ RS485 Bus
- **A** : บัส A (ข้อมูลบวก)
- **B** : บัส B (ข้อมูลลบ)

### วิธีใช้ใน Code

```cpp
// UART0 - Serial (TX=GPIO1, RX=GPIO3)
#define RS485_TX 1
#define RS485_RX 3
#define RS485_DE 17  // Direction control GPIO

void setup() {
    Serial.begin(9600);  // UART0 สำหรับ RS485
    pinMode(RS485_DE, OUTPUT);
}

// ส่งข้อมูล
void sendRS485(const char* data) {
    digitalWrite(RS485_DE, HIGH);  // Transmit mode
    Serial.print(data);
    delay(10);
    digitalWrite(RS485_DE, LOW);   // Receive mode
}

// อ่านข้อมูล
void readRS485() {
    if (Serial.available()) {
        char buff = Serial.read();
        Serial.print(buff);
    }
}
```

---

## สวิตช์ RS232/RS485

บนบอร์ดมีสวิตช์ SPDT เพื่อเลือกระหว่าง:
- **RS232 Mode**: ใช้ Serial Port แบบดั้งเดิม
- **RS485 Mode**: ใช้ MAX13487 สำหรับระยะไกล

⚠️ **หมายเหตุ**: ต้องตั้งค่าฮาร์ดแวร์ให้ถูกต้องตามความต้องการ

---

## การติดตั้งและ Setup

### ข้อกำหนดสำคัญ
- ✅ PlatformIO IDE
- ✅ ESP32 Board Support
- ✅ USB Cable Type-C สำหรับโปรแกรมและ Serial Monitor

### ขั้นตอนการติดตั้ง

1. **คัดลอก Project**
   ```bash
   git clone <repo-url>
   cd mySmartDevice
   ```

2. **ติดตั้ง Dependencies**
   ```bash
   platformio lib install
   ```

3. **ตั้งค่า platformio.ini**
   ```ini
   [env:esp32dev]
   platform = espressif32
   board = esp32doit-devkit-v1
   framework = arduino
   monitor_speed = 115200
   ```

4. **Build และ Upload**
   ```bash
   platformio run --target upload
   ```

5. **ทดสอบ Serial Monitor**
   ```bash
   platformio device monitor
   ```

---

## Pin Summary

### GPIO Mapping Reference

| GPIO | ฟังก์ชัน | ประเภท |
|------|---------|--------|
| GPIO1 | UART0 TX (RS485) | Serial |
| GPIO2 | Status LED | Output |
| GPIO3 | UART0 RX (RS485) | Serial |
| GPIO4 | Relay 1 | Output |
| GPIO5 | OLED RST (optional) | Output |
| GPIO16 | Relay 2 | Output |
| GPIO17 | Relay 3 | Output |
| GPIO21 | I2C SDA (OLED) | I2C |
| GPIO22 | I2C SCL (OLED) | I2C |
| GPIO27 | Isolated Input 2 | Input |
| GPIO32 | Switch 3 | Input |
| GPIO33 | Isolated Input 1 | Input |
| GPIO34 | Switch 1 | Input |
| GPIO35 | Switch 2 | Input |

---

## ข้อควรระวัง ⚠️

1. **แรงดันไฟ**: ไม่ให้จ่าย 5V ตรงไปยัง GPIO โดยตรง ต้องใช้ voltage regulator
2. **Relay Coil**: การเปิด/ปิด relay ต้องใช้ pulldown resistor เพื่อป้องกันการสูญเสียพลังงาน
3. **I2C Address**: ตรวจสอบ I2C Address ของ OLED (0x3C หรือ 0x3D)
4. **RS485 Termination**: ถ้ากำลังใช้บัส RS485 ยาวต้องเพิ่ม 120Ω termination resistor
5. **Serial Port**: ตรวจสอบให้แน่ใจว่า Serial port ถูกต้องเมื่อ Upload code

---

## ข้อมูลเพิ่มเติม

- [ESP32 Official Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [SSD1306 OLED Library](https://github.com/adafruit/Adafruit_SSD1306)
- [MAX13487 Datasheet](https://datasheets.maximintegrated.com/en/ds/MAX13487.pdf)

---

**ปรับปรุงครั้งล่าสุด**: March 2, 2026

