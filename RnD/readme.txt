-Step 1: Cài stm32-stlink , connect-device + upgrade driver device stlink-v2
-Step 2: Cài stm32cubemx
-Step 3: Mở stm32cubemx:
+ File → New Project hoặc New Project trên màn hình chính
+ Trong tab MCU/MPU Selector, nhập mã chip, ví dụ STM32F103C8T6, rồi chọn đúng kết quả.
+ Chọn [generate_code]
-Step 4: Nếu muốn nạp code vscode 
+ cấu trúc project phải có file CMakeLists.txt hay Makefile =>Nếu chưa có 2 file trong project , open-project -> tab [Project Manager] -> sub-tab [Project] -> Toolchian/IDE: Cmake -> chạy[generate_code]
+ Open vscode -> open folder stm32-project -> ctrl+shift+p -> "CMake:Configure" -> Success ->Done
=> Fail -> maybe missing arm-none-eabi-gcc path in environement 
-> exe command in cmd "where.exe arm-none-eabi-gcc" -> missing ?
-> fix by download ( https://drive.google.com/file/d/1BCL5-3o_A2u6E8eyZtC-UX5A3EXDn9aM/view?usp=sharing )
-> extract and copy the  arm-none-eabi-gcc/path to systems_environment
-> back to vscode execute "CMake:Configure" againt ->Success -> exe "CMake:Build" -> Success then done.
- Step 4: Cài STM32CubeProgrammer_win64
