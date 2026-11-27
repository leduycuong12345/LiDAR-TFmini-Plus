/*
 * Arduino nhận dữ liệu từ STM32 (USART3) và hiển thị lên LCD 16x2
 *
 * Giao thức: STM32 gửi mỗi gói là 1 dòng kết thúc bằng '\n'
 *   - "Nhiet do: 25.3\n"        -> hiển thị ở dòng 1, dòng 2 xóa trắng
 *   - "T=25.3C|H=60%\n"          -> phần trước '|' ở dòng 1, phần sau ở dòng 2
 *
 * Nối dây (Arduino Uno/Nano):
 *   STM32 PB10 (USART3_TX)  -> Arduino D10 (RX của SoftwareSerial)
 *   STM32 PB11 (USART3_RX)  -> Arduino D11 (chỉ cần nếu Arduino gửi ngược lại)
 *   STM32 GND               -> Arduino GND   (BẮT BUỘC chung GND)
 *
 * LCD:
 *   - Mặc định dùng module I2C (PCF8574): SDA -> A4, SCL -> A5
 *   - Nếu dùng LCD song song 4-bit: bỏ comment #define LCD_PARALLEL
 */

// #define LCD_PARALLEL      // bỏ comment nếu dùng LCD không có module I2C
// #define USE_MEGA          // bỏ comment nếu dùng Arduino Mega (dùng Serial1, chân 19 = RX1)

#define UART_BAUD  115200     // phải TRÙNG với baud của USART3 bên STM32
#define LCD_COLS   16
#define LCD_ROWS   2
#define BUF_SIZE   64

// ---------- LCD ----------
#ifdef LCD_PARALLEL
  #include <LiquidCrystal.h>
  // RS, E, D4, D5, D6, D7
  LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
#else
  #include <Wire.h>
  #include <LiquidCrystal_I2C.h>   // thư viện "LiquidCrystal I2C" (Frank de Brabander)
  LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS);  // nếu không hiện, thử 0x3F
#endif

// ---------- UART ----------
#ifdef USE_MEGA
  #define stmSerial Serial1
#else
  #include <SoftwareSerial.h>
  SoftwareSerial stmSerial(10, 11);  // RX, TX
#endif

char rxBuf[BUF_SIZE];
uint8_t rxIdx = 0;
unsigned long lastRxTime = 0;

// In 1 dòng lên LCD, tự đệm khoảng trắng để xóa ký tự cũ
void printLine(uint8_t row, const char *text) {
  lcd.setCursor(0, row);
  uint8_t i = 0;
  for (; i < LCD_COLS && text[i] != '\0'; i++) {
    lcd.print(text[i]);
  }
  for (; i < LCD_COLS; i++) {
    lcd.print(' ');
  }
}

// Xử lý 1 gói hoàn chỉnh
void handlePacket(char *data) {
  Serial.print(F("RX: "));
  Serial.println(data);              // debug ra Serial Monitor

  char *sep = strchr(data, '|');
  if (sep != NULL) {
    *sep = '\0';                     // tách thành 2 chuỗi
    printLine(0, data);
    printLine(1, sep + 1);
  } else if (strlen(data) > LCD_COLS) {
    printLine(0, data);              // chuỗi dài: tràn xuống dòng 2
    printLine(1, data + LCD_COLS);
  } else {
    printLine(0, data);
    printLine(1, "");
  }
}

void setup() {
  Serial.begin(UART_BAUD);                // debug qua USB
  stmSerial.begin(UART_BAUD);

#ifdef LCD_PARALLEL
  lcd.begin(LCD_COLS, LCD_ROWS);
#else
  lcd.init();
  lcd.backlight();
#endif

  printLine(0, "Cho du lieu...");
  printLine(1, "UART3 STM32");
  Serial.println(F("Arduino san sang nhan du lieu tu STM32"));
}

void loop() {
  while (stmSerial.available()) {
    char c = stmSerial.read();
    lastRxTime = millis();

    if (c == '\r') continue;         // bỏ qua CR (nếu STM32 gửi "\r\n")

    if (c == '\n') {
      rxBuf[rxIdx] = '\0';
      if (rxIdx > 0) handlePacket(rxBuf);
      rxIdx = 0;
    } else if (rxIdx < BUF_SIZE - 1) {
      rxBuf[rxIdx++] = c;
    } else {
      rxIdx = 0;                     // tràn buffer -> bỏ gói lỗi
    }
  }

  // Báo mất kết nối nếu 5 giây không nhận được gì
  if (lastRxTime != 0 && millis() - lastRxTime > 5000) {
    printLine(0, "Mat ket noi");
    printLine(1, "STM32...");
    lastRxTime = 0;
  }
}