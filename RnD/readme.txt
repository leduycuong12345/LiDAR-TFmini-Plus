-Step 1: Cài stm32-stlink , connect-device + upgrade driver device stlink-v2
-Step 2: Cài stm32cubemx
-Step 3: Mở stm32cubemx:
+ File → New Project hoặc New Project trên màn hình chính
+ Trong tab MCU/MPU Selector, nhập mã chip, ví dụ [STM32F103C8T6], rồi chọn đúng kết quả.
+ Trong dự an này chỉ cần PA9(display) , PA10(read)
	Trong mục Connectivity, chọn USART1, sau đó chọn Asynchronous.
 		Với STM32F103C8T6, cấu hình mặc định của USART1 là:
			- PA9: USART1_TX
			- PA10: USART1_RX
+ Chọn [generate_code]
-Step 4: Nếu muốn nạp code vscode 
+ cấu trúc project phải có file CMakeLists.txt hay Makefile =>Nếu chưa có 2 file trong project , open-project  stm32cubemx -> tab [Project Manager] -> sub-tab [Project] -> Toolchian/IDE: Cmake -> chạy[generate_code]
+ Open vscode -> open folder stm32-project -> ctrl+shift+p -> "CMake:Configure" -> Success ->Done
=> Fail -> maybe missing arm-none-eabi-gcc path in environement 
-> exe command in cmd "where.exe arm-none-eabi-gcc" -> missing ?
-> fix by download ( https://drive.google.com/file/d/1BCL5-3o_A2u6E8eyZtC-UX5A3EXDn9aM/view?usp=sharing )
-> extract and copy the  arm-none-eabi-gcc/path to systems_environment
-> back to vscode execute "CMake:Configure"  ->"Debug" ->Success -> exe "CMake:Build" -> Success then done ( Nếu k có lỗi thì sẽ generate file /debug/project_name.elf )
- Step 4: Cài STM32CubeProgrammer_win64
+ Open project -> Lúc này ta thấy device "Connected" nếu ok
- Step 5: Nạp code vscode   file debug/*.elf. vào mạch stm32 bằng stm32cubeprogrammer
+ Tìm file *.elf -> Download
- Step 6: Nếu nạp code lỗi làm disconnect phải reset board-> ngắt nguồn board
-> chuyển boot0-HIGH -> kết nối vào lại 
-> STm32CubeProgrammer frequency: 100 , mode under reset, reset-mode:hardware-reset.
-> Nếu thấy device lại  thì Full-chip-erase -> start-automatic-mode
-> Ngắt nguồn board trả boot0-low -> kết nối lại là connect bth
- Step 7: Final
+STM32CubeMX :
	Configure GPIO, clocks, and peripherals.
	Generate the initial project code.
	It is not the main IDE for writing and compiling your application.
	At this stage need :
		PA10, and you only need PA9 if you
		PA10	USART1_RX	TFmini Plus TX (Required!! to receive distance measurements from the 									LiDAR )
		PA9	USART1_TX	TFmini Plus RX (Optional!! transmit data back to a PC or another device. )
+VScode : de lap trinh ham tinh van toc
	Put your .c, .cpp, and .h files into your project.
	Build the firmware to generate an .elf file.
	Debug your code using ST-Link.
+STM32CubeProgrammer:
	Connect to your Blue Pill using ST-Link.
	Select the compiled .elf, .hex, or .bin file.
	Click Download to program the microcontroller.
	You cannot directly flash .cpp or .h source files.