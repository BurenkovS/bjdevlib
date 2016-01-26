# bjdevlib
BJ Devices TB series MIDI controller  open source library (bjdevlib) provide functionality for custom controller firmware creation.
BJ Devices TB series MIDI controllers based on ATmega64 microcontroller. Library was created in Atmel Studio. 
Currently there are two versions of controllers - TB-12 (12 footbuttons) and TB-5(5 buttons).
All modification builds on same hardware, only buttons connection is differ, so bjdevlib works for all devices.

Library used third-party code for LCD display. You can use provided code, or add your own LCD library to your project and use it.

How to start:
1. Download this library. You can save it in any convenient location on the disk

2. Create new Atmel Studio project (GCC C Executable Project, ATmega64 device)

3. Go to project properties -> AVR/GNU C Compiler -> Symbols,
add symbol TB_12_DEVICE or TB_5_DEVICE(depends of your model), add F_CPU=8000000UL symbol for all configurations

4. Go to AVR/GNU C Compiler -> Symbols, add include path for "bjdevib/tbseries/include" folder for all configuratoin
4a. If you want to use same LCD library, as in example, add "lcdlib" to include path for all configurations

5. In project explorer create folder "bjdevilib", add existing items - all files from "bjdevib/tbseries/src" folder
5a. If you want to use same LCD library, also add "lcd_lib.c" from "lcdlib" folder

6. Copy code from examples to your main file, compile it to make sure that everything is configured correctly. 
You can also upload compiled hex file to hardware, check BJ devices TB-series manual to update firmware (chapter "Firmware Update") http://bjdevices.com/manuals/manualtb12_en.pdf 

7. Learn example to understand how to work with library

8. Write your own best of the world firmware and update your TB-12 or TB-5 device!!

9. Find bugs in library and report us. Improve library and send pull request! It is strongly welcome!


