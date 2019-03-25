# MPLAB IDE generated this makefile for use with GNU make.
# Project: ClockProject.mcp
# Date: Mon Mar 25 16:52:45 2019

AS = MPASMWIN.exe
CC = mcc18.exe
LD = mplink.exe
AR = mplib.exe
RM = rm

ClockProject.cof : main.o oled.o OledGraphics.o
	$(LD) /p18F46J50 /l"..\..\MPLAB C18\lib" "rm18f46j50_g.lkr" "main.o" "oled.o" "OledGraphics.o" "C:\Users\Leon\Documents\Embedded Cores\Microchip\Obj\BMA150.o" "C:\Users\Leon\Documents\Embedded Cores\Microchip\Obj\mtouch.o" "C:\Users\Leon\Documents\Embedded Cores\Microchip\Obj\soft_start.o" /u_CRUNTIME /u_DEBUG /z__MPLAB_BUILD=1 /z__MPLAB_DEBUG=1 /z__MPLAB_DEBUGGER_SKDE=1 /z__ICD2RAM=1 /m"ClockProject.map" /w /o"ClockProject.cof"

main.o : main.c ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/stdio.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/stdlib.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/string.h ../../Microchip/mTouch/mtouch.h ../../Microchip/BMA150\ driver/BMA150.h oled.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/math.h OledGraphics.h main.c ../../Microchip/Include/GenericTypeDefs.h ../../Microchip/Include/Compiler.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/p18cxxx.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/p18f46j50.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/stdarg.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/stddef.h ../../Microchip/Include/HardwareProfile.h ../../Microchip/Include/HardwareProfile\ -\ PIC18F\ Starter\ Kit.h ../../Microchip/Soft\ Start/soft_start.h
	$(CC) -p=18F46J50 /i"..\..\Microchip\Soft Start" -I"..\..\Microchip\BMA150 driver" -I"..\..\Microchip\OLED driver" -I"..\..\Microchip\mTouch" -I"..\..\Microchip\Include" "main.c" -fo="main.o" -D__DEBUG -D__MPLAB_DEBUGGER_SKDE=1 -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-

oled.o : ../../Microchip/OLED\ driver/oled.c ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/stdio.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/stdlib.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/string.h ../../Microchip/OLED\ driver/oled.h ../../Microchip/OLED\ driver/oled.c ../../Microchip/Include/GenericTypeDefs.h ../../Microchip/Include/Compiler.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/p18cxxx.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/p18f46j50.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/stdarg.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/stddef.h
	$(CC) -p=18F46J50 /i"..\..\Microchip\Soft Start" -I"..\..\Microchip\BMA150 driver" -I"..\..\Microchip\OLED driver" -I"..\..\Microchip\mTouch" -I"..\..\Microchip\Include" "C:\Users\Leon\Documents\Embedded Cores\Microchip\OLED driver\oled.c" -fo="oled.o" -D__DEBUG -D__MPLAB_DEBUGGER_SKDE=1 -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-

OledGraphics.o : OledGraphics.c ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/stdio.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/stdlib.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/string.h OledGraphics.h oled.h OledGraphics.c ../../Microchip/Include/Compiler.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/p18cxxx.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/p18f46j50.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/stdarg.h ../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.47/h/stddef.h ../../Microchip/Include/GenericTypeDefs.h
	$(CC) -p=18F46J50 /i"..\..\Microchip\Soft Start" -I"..\..\Microchip\BMA150 driver" -I"..\..\Microchip\OLED driver" -I"..\..\Microchip\mTouch" -I"..\..\Microchip\Include" "OledGraphics.c" -fo="OledGraphics.o" -D__DEBUG -D__MPLAB_DEBUGGER_SKDE=1 -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-

clean : 
	$(RM) "main.o" "oled.o" "OledGraphics.o" "ClockProject.cof" "ClockProject.hex" "ClockProject.map"

