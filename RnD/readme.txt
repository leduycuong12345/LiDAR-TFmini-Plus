-Step 1: Cài stm32-stlink , connect-device + upgrade driver device stlink-v2
-Step 2: Cài stm32cubemx
-Step 3: Mở stm32cubemx:
+ File → New Project hoặc New Project trên màn hình chính
+ Trong tab MCU/MPU Selector, nhập mã chip, ví dụ STM32F103C8T6, rồi chọn đúng kết quả.
+ Chọn [generate_code]
-Step 4: Nếu muốn nạp code vscode thì cấu trúc project phải có file CMakeLists.txt hay Makefile
+ Nếu chưa có 2 file trong project , open-project -> tab [Project Manager] -> sub-tab [Project] 
-> Toolchian/IDE: Cmake -> chạy[generate_code]