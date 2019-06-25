# stm32f429_vga_examples
Code examples of using STM32F429 for generating VGA signals (HAL and SPL).

Hardware: Core429I devboard + custom devboard with R-2R VGA DAC, USB, SDCard and camera connectors:
![Alt text](schematic/ModuleSchematic.png?raw=true "Image")


"ov7670_camera" - picture from OV7670 camera is displayed at the VGA display.  Written for SPL.  

"simple_vga_example" - display some simple patterns at the VGA display.  Written for SPL.  

"simple_vga_example_hal" - display some simple patterns at the VGA display.  Written for HAL.  

"sdcard_vga_example" - STM32F429 reads image from SD Card and show it at the VGA display.  Written for SPL.  

There are some photos of VGA display at "Photos" directory.
