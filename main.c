/********************************************************************
 FileName:     main.c
 Dependencies: See INCLUDES section
 Processor:   PIC18 or PIC24 USB Microcontrollers
 Hardware:    The code is natively intended to be used on the following
        hardware platforms: PICDEM™ FS USB Demo Board, 
        PIC18F87J50 FS USB Plug-In Module, or
        Explorer 16 + PIC24 USB PIM.  The firmware may be
        modified for use on other USB platforms by editing the
        HardwareProfile.h file.
 Complier:    Microchip C18 (for PIC18) or C30 (for PIC24)
 Company:   Microchip Technology, Inc.

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the “Company”) for its PIC® Microcontroller is intended and
 supplied to you, the Company’s customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

********************************************************************
 File Description:

 Change History:
  Rev   Date         Description
  1.0   11/19/2004   Initial release
  2.1   02/26/2007   Updated for simplicity and to use common
                     coding style
********************************************************************/


//	========================	INCLUDES	========================
#ifdef _VISUAL
#include "VisualSpecials.h"
#endif // VISUAL

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "HardwareProfile.h"

#include "mtouch.h"

#include "BMA150.h"

#include "oled.h"

#include "soft_start.h"

#include "OledGraphics.h"


//	========================	CONFIGURATION	========================

#if defined(PIC18F46J50_PIM)
   //Watchdog Timer Enable bit:
     #pragma config WDTEN = OFF          //WDT disabled (control is placed on SWDTEN bit)
   //PLL Prescaler Selection bits:
     #pragma config PLLDIV = 3           //Divide by 3 (12 MHz oscillator input)
   //Stack Overflow/Underflow Reset Enable bit:
     #pragma config STVREN = ON            //Reset on stack overflow/underflow enabled
   //Extended Instruction Set Enable bit:
     #pragma config XINST = OFF          //Instruction set extension and Indexed Addressing mode disabled (Legacy mode)
   //CPU System Clock Postscaler:
     #pragma config CPUDIV = OSC1        //No CPU system clock divide
   //Code Protection bit:
     #pragma config CP0 = OFF            //Program memory is not code-protected
   //Oscillator Selection bits:
     #pragma config OSC = ECPLL          //HS oscillator, PLL enabled, HSPLL used by USB
   //Secondary Clock Source T1OSCEN Enforcement:
     #pragma config T1DIG = ON           //Secondary Oscillator clock source may be selected
   //Low-Power Timer1 Oscillator Enable bit:
     #pragma config LPT1OSC = OFF        //Timer1 oscillator configured for higher power operation
   //Fail-Safe Clock Monitor Enable bit:
     #pragma config FCMEN = OFF           //Fail-Safe Clock Monitor disabled
   //Two-Speed Start-up (Internal/External Oscillator Switchover) Control bit:
     #pragma config IESO = OFF           //Two-Speed Start-up disabled
   //Watchdog Timer Postscaler Select bits:
     #pragma config WDTPS = 32768        //1:32768
   //DSWDT Reference Clock Select bit:
     #pragma config DSWDTOSC = INTOSCREF //DSWDT uses INTOSC/INTRC as reference clock
   //RTCC Reference Clock Select bit:
     #pragma config RTCOSC = T1OSCREF    //RTCC uses T1OSC/T1CKI as reference clock
   //Deep Sleep BOR Enable bit:
     #pragma config DSBOREN = OFF        //Zero-Power BOR disabled in Deep Sleep (does not affect operation in non-Deep Sleep modes)
   //Deep Sleep Watchdog Timer Enable bit:
     #pragma config DSWDTEN = OFF        //Disabled
   //Deep Sleep Watchdog Timer Postscale Select bits:
     #pragma config DSWDTPS = 8192       //1:8,192 (8.5 seconds)
   //IOLOCK One-Way Set Enable bit:
     #pragma config IOL1WAY = OFF        //The IOLOCK bit (PPSCON<0>) can be set and cleared as needed
   //MSSP address mask:
     #pragma config MSSP7B_EN = MSK7     //7 Bit address masking
   //Write Protect Program Flash Pages:
     #pragma config WPFP = PAGE_1        //Write Protect Program Flash Page 0
   //Write Protection End Page (valid when WPDIS = 0):
     #pragma config WPEND = PAGE_0       //Write/Erase protect Flash Memory pages starting at page 0 and ending with page WPFP[5:0]
   //Write/Erase Protect Last Page In User Flash bit:
     #pragma config WPCFG = OFF          //Write/Erase Protection of last page Disabled
   //Write Protect Disable bit:
     #pragma config WPDIS = OFF          //WPFP[5:0], WPEND, and WPCFG bits ignored
  
#else
    #error No hardware board defined, see "HardwareProfile.h" and __FILE__
#endif



//	========================	Global VARIABLES	========================
#pragma udata
//You can define Global Data Elements here

//Selected option variables
int mainMenuSelectedOption = 1, displayModeSelectedOption = 3, displayIntervaSelectedOption = 3, amPmSelectedOption =  3, setTimeSelectedOption = 43, setDateSelectedOption = 48;

//Time
int tmpHour = 0, tmpMinute = 0, tmpSecond = 0, hour, minute, second;

//Big Digits
char digitPos[3][2] = {{0,20},{45,63},{90,110}};

//Date
int month = 1, day = 1;

//Booleans
BOOL IsClockSet = FALSE, IsMenuOpen = FALSE, IsDigitalDisplay = TRUE, Is12H = TRUE, IsAM = TRUE;

//	========================	PRIVATE PROTOTYPES	========================
static void InitializeSystem(void);
static void ProcessIO(void);
static void UserInit(void);
static void YourHighPriorityISRCode();
static void YourLowPriorityISRCode();
void AddMinute(void);
void AddHour(void);
void AddDay(void);
void AddMonth(void);
BOOL CheckButtonPressed(void);

//	========================	VECTOR REMAPPING	========================
#if defined(__18CXX)
  //On PIC18 devices, addresses 0x00, 0x08, and 0x18 are used for
  //the reset, high priority interrupt, and low priority interrupt
  //vectors.  However, the current Microchip USB bootloader 
  //examples are intended to occupy addresses 0x00-0x7FF or
  //0x00-0xFFF depending on which bootloader is used.  Therefore,
  //the bootloader code remaps these vectors to new locations
  //as indicated below.  This remapping is only necessary if you
  //wish to program the hex file generated from this project with
  //the USB bootloader.  If no bootloader is used, edit the
  //usb_config.h file and comment out the following defines:
  //#define PROGRAMMABLE_WITH_SD_BOOTLOADER
  
  #if defined(PROGRAMMABLE_WITH_SD_BOOTLOADER)
    #define REMAPPED_RESET_VECTOR_ADDRESS     0xA000
    #define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS  0xA008
    #define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS 0xA018
  #else 
    #define REMAPPED_RESET_VECTOR_ADDRESS     0x00
    #define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS  0x08
    #define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS 0x18
  #endif
  
  #if defined(PROGRAMMABLE_WITH_SD_BOOTLOADER)
  extern void _startup (void);        // See c018i.c in your C18 compiler dir
  #pragma code REMAPPED_RESET_VECTOR = REMAPPED_RESET_VECTOR_ADDRESS
  void _reset (void)
  {
      _asm goto _startup _endasm
  }
  #endif
  #pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS
  void Remapped_High_ISR (void)
  {
       _asm goto YourHighPriorityISRCode _endasm
  }
  #pragma code REMAPPED_LOW_INTERRUPT_VECTOR = REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS
  void Remapped_Low_ISR (void)
  {
       _asm goto YourLowPriorityISRCode _endasm
  }
  
  #if defined(PROGRAMMABLE_WITH_SD_BOOTLOADER)
  //Note: If this project is built while one of the bootloaders has
  //been defined, but then the output hex file is not programmed with
  //the bootloader, addresses 0x08 and 0x18 would end up programmed with 0xFFFF.
  //As a result, if an actual interrupt was enabled and occured, the PC would jump
  //to 0x08 (or 0x18) and would begin executing "0xFFFF" (unprogrammed space).  This
  //executes as nop instructions, but the PC would eventually reach the REMAPPED_RESET_VECTOR_ADDRESS
  //(0x1000 or 0x800, depending upon bootloader), and would execute the "goto _startup".  This
  //would effective reset the application.
  
  //To fix this situation, we should always deliberately place a 
  //"goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS" at address 0x08, and a
  //"goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS" at address 0x18.  When the output
  //hex file of this project is programmed with the bootloader, these sections do not
  //get bootloaded (as they overlap the bootloader space).  If the output hex file is not
  //programmed using the bootloader, then the below goto instructions do get programmed,
  //and the hex file still works like normal.  The below section is only required to fix this
  //scenario.
  #pragma code HIGH_INTERRUPT_VECTOR = 0x08
  void High_ISR (void)
  {
       _asm goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS _endasm
  }
  #pragma code LOW_INTERRUPT_VECTOR = 0x18
  void Low_ISR (void)
  {
       _asm goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS _endasm
  }
  #endif  //end of "#if defined(||defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER))"

  #pragma code
  
//	========================	Application Interrupt Service Routines	========================
  //These are your actual interrupt handling routines.
  #pragma interrupt YourHighPriorityISRCode
  void YourHighPriorityISRCode()
  {
    //Check which interrupt flag caused the interrupt.
    //Service the interrupt
    //Clear the interrupt flag
    //Etc.
	if(INTCONbits.TMR0IF)
	{
		TMR0H = 0x48 ;				//Write 0xCE51 to 16 bit Timer0
		TMR0L = 0xE3 ;
		
		second++;
		if(second > 59)
		{
			second = 0;
			AddMinute();
		}	
		INTCONbits.TMR0IF = 0;
	}	
  
  } //This return will be a "retfie fast", since this is in a #pragma interrupt section 
  #pragma interruptlow YourLowPriorityISRCode
  void YourLowPriorityISRCode()
  {
    //Check which interrupt flag caused the interrupt.
    //Service the interrupt
    //Clear the interrupt flag
    //Etc.
  
  } //This return will be a "retfie", since this is in a #pragma interruptlow section 
#endif




//	========================	Board Initialization Code	========================
#pragma code
#define ROM_STRING rom unsigned char*

/******************************************************************************
 * Function:        void UserInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine should take care of all of the application code
 *                  initialization that is required.
 *
 * Note:            
 *
 *****************************************************************************/
void UserInit(void)
{

  /* Initialize the mTouch library */
  mTouchInit();

  /* Call the mTouch callibration function */
  mTouchCalibrate();

  /* Initialize the accelerometer */
  InitBma150(); 

  /* Initialize the oLED Display */
   ResetDevice();  
   FillDisplay(0x00);
}//end UserInit


/********************************************************************
 * Function:        static void InitializeSystem(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        InitializeSystem is a centralize initialization
 *                  routine. All required USB initialization routines
 *                  are called from here.
 *
 *                  User application initialization routine should
 *                  also be called from here.                  
 *
 * Note:            None
 *******************************************************************/
static void InitializeSystem(void)
{
	// Soft Start the APP_VDD
	while(!AppPowerReady())
		;

    #if defined(PIC18F46J50_PIM)
  //Configure all I/O pins to use digital input buffers
    ANCON0 = 0xFF;                  // Default all pins to digital
    ANCON1 = 0xFF;                  // Default all pins to digital
    #endif
       
    UserInit();

}//end InitializeSystem


//	========================	Application Code	========================

BOOL CheckButtonPressed(void)
{
    static char buttonPressed = FALSE;
    static unsigned long buttonPressCounter = 0;

    if(PORTBbits.RB0 == 0)
    {
        if(buttonPressCounter++ > 1)
        {
            buttonPressCounter = 0;
            buttonPressed = TRUE;
        }
    }
    else
    {
        if(buttonPressed == TRUE)
        {
            if(buttonPressCounter == 0)
            {
                buttonPressed = FALSE;
                return TRUE;
            }
            else
            {
                buttonPressCounter--;
            }
        }
    }

    return FALSE;
}

void AddMinute()
{
	minute++;
	if(minute > 59)
	{
		minute = 0;
		AddHour();
	}
		
}

void AddHour()
{
	hour++;
	if(Is12H)
	{
		if(hour > 12 && IsAM == TRUE)
		{
			IsAM = FALSE;
			hour = 1;
		}
		else if(hour > 12 && IsAM == FALSE)
		{
			IsAM = TRUE;
			hour = 1;
			AddDay();
		}	
	}
	else
	{
		if(hour > 23)
		{
			hour = 0;
			AddDay();
		}
	}
}

void AddDay()
{
	day++;
	if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
	{
		if(day > 31)
		{
			day = 1;
			AddMonth();
		}
	}
	else if(month == 4 || month == 6 || month == 9 || month == 11)
	{
		if(day > 30)
		{
			day = 1;
			AddMonth();
		}
	}
	else
	{
		if(day > 28)
		{
			day = 1;
			AddMonth();
		}
	}
}

void AddMonth()
{
	month++;
	if(month > 12)
		month = 1;
}

//Reads L button
int ReadsLButton()
{
	int btnL;
	btnL = mTouchReadButton(3);
	if (btnL < 600)
	{
		/* wait for the button to be released */
		while (btnL < 610)
		{
			btnL = mTouchReadButton(3);
		}
		return 1;
	}
	return 0;
}

//Reads R button
int ReadsRButton()
{
	int btnR;
	btnR = mTouchReadButton(0);
	if (btnR < 600)
	{
		/* wait for the button to be released */
		while (btnR < 610)
		{
			btnR = mTouchReadButton(0);
		}
		return 1;
	}
	return 0;
}

//Reads Potenziometer
int ReadPotenziometer()
{
	int x, a2d;		
	x = 2;
	ADCON0 = 0b00010011;
	while(x){
		x = x & ADCON0;
	}
	a2d =  ((int)ADRESH << 8) + ADRESL;
	return a2d;
}

//Funtion to read UP button press
int ReadUpButton()
{
	int btnUp;
	btnUp = mTouchReadButton(1);
	if ((btnUp < 800))
	{
		/* wait for the button to be released */
		while ((btnUp < 850))
		{
			btnUp = mTouchReadButton(1);
		}
		return 1;
	}
	return 0;
}

//Funtion to read DOWN button press
int ReadDownButton()
{
	int btnDown;
	btnDown = mTouchReadButton(2);
	if ((btnDown < 800))
	{
		/* wait for the button to be released */
		while ((btnDown < 850))
		{
			btnDown = mTouchReadButton(2);
		}
		return 1;
	}
	return 0;
}

void PrintMinutes()
{
	int lDigit, rDigit;
	lDigit = minute / 10;
	rDigit = minute % 10;
	switch(lDigit)
	{
		case 0: PrintBigDigit(0x20,digitPos[1][0]);break;
		case 1: PrintBigDigit(0x24,digitPos[1][0]);break;
		case 2: PrintBigDigit(0x28,digitPos[1][0]);break;
		case 3: PrintBigDigit(0x2C,digitPos[1][0]);break;
		case 4: PrintBigDigit(0x30,digitPos[1][0]);break;
		case 5: PrintBigDigit(0x34,digitPos[1][0]);break;
	}

	switch(rDigit)
	{
		case 0: PrintBigDigit(0x20,digitPos[1][1]);break;
		case 1: PrintBigDigit(0x24,digitPos[1][1]);break;
		case 2: PrintBigDigit(0x28,digitPos[1][1]);break;
		case 3: PrintBigDigit(0x2C,digitPos[1][1]);break;
		case 4: PrintBigDigit(0x30,digitPos[1][1]);break;
		case 5: PrintBigDigit(0x34,digitPos[1][1]);break;
		case 6: PrintBigDigit(0x38,digitPos[1][1]);break;
		case 7: PrintBigDigit(0x3C,digitPos[1][1]);break;
		case 8: PrintBigDigit(0x40,digitPos[1][1]);break;
		case 9: PrintBigDigit(0x44,digitPos[1][1]);break;
	}
}

void PrintSeconds()
{
	int lDigit, rDigit;
	lDigit = second / 10;
	rDigit = second % 10;
	switch(lDigit)
	{
		case 0: PrintBigDigit(0x20,digitPos[2][0]);break;
		case 1: PrintBigDigit(0x24,digitPos[2][0]);break;
		case 2: PrintBigDigit(0x28,digitPos[2][0]);break;
		case 3: PrintBigDigit(0x2C,digitPos[2][0]);break;
		case 4: PrintBigDigit(0x30,digitPos[2][0]);break;
		case 5: PrintBigDigit(0x34,digitPos[2][0]);break;
	}

	switch(rDigit)
	{
		case 0: PrintBigDigit(0x20,digitPos[2][1]);break;
		case 1: PrintBigDigit(0x24,digitPos[2][1]);break;
		case 2: PrintBigDigit(0x28,digitPos[2][1]);break;
		case 3: PrintBigDigit(0x2C,digitPos[2][1]);break;
		case 4: PrintBigDigit(0x30,digitPos[2][1]);break;
		case 5: PrintBigDigit(0x34,digitPos[2][1]);break;
		case 6: PrintBigDigit(0x38,digitPos[2][1]);break;
		case 7: PrintBigDigit(0x3C,digitPos[2][1]);break;
		case 8: PrintBigDigit(0x40,digitPos[2][1]);break;
		case 9: PrintBigDigit(0x44,digitPos[2][1]);break;
	}
}

void Print12HourClock()
{
	switch(hour)
	{
		case 1: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x24,digitPos[0][1]);break;
		case 2: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x28,digitPos[0][1]);break;
		case 3: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x2C,digitPos[0][1]);break;
		case 4: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x30,digitPos[0][1]);break;
		case 5: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x34,digitPos[0][1]);break;
		case 6: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x38,digitPos[0][1]);break;
		case 7: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x3C,digitPos[0][1]);break;
		case 8: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x40,digitPos[0][1]);break;
		case 9: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x44,digitPos[0][1]);break;
		case 10: PrintBigDigit(0x24,digitPos[0][0]);PrintBigDigit(0x20,digitPos[0][1]);break;
		case 11: PrintBigDigit(0x24,digitPos[0][0]);PrintBigDigit(0x24,digitPos[0][1]);break;
		case 12: PrintBigDigit(0x24,digitPos[0][0]);PrintBigDigit(0x28,digitPos[0][1]);break;
	}
	PrintMinutes();
	PrintSeconds();
	if(IsAM)
		oledPutROMString((ROM_STRING)"AM",7,110);
	else
		oledPutROMString((ROM_STRING)"PM",7,110);
}

void Print24HourClock()
{
	switch(hour)
	{
		case 1: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x24,digitPos[0][1]);break;
		case 2: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x28,digitPos[0][1]);break;
		case 3: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x2C,digitPos[0][1]);break;
		case 4: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x30,digitPos[0][1]);break;
		case 5: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x34,digitPos[0][1]);break;
		case 6: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x38,digitPos[0][1]);break;
		case 7: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x3C,digitPos[0][1]);break;
		case 8: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x40,digitPos[0][1]);break;
		case 9: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x44,digitPos[0][1]);break;
		case 10: PrintBigDigit(0x24,digitPos[0][0]);PrintBigDigit(0x20,digitPos[0][1]);break;
		case 11: PrintBigDigit(0x24,digitPos[0][0]);PrintBigDigit(0x24,digitPos[0][1]);break;
		case 12: PrintBigDigit(0x24,digitPos[0][0]);PrintBigDigit(0x28,digitPos[0][1]);break;
		case 13: PrintBigDigit(0x24,digitPos[0][0]);PrintBigDigit(0x2C,digitPos[0][1]);break;
		case 14: PrintBigDigit(0x24,digitPos[0][0]);PrintBigDigit(0x30,digitPos[0][1]);break;
		case 15: PrintBigDigit(0x24,digitPos[0][0]);PrintBigDigit(0x34,digitPos[0][1]);break;
		case 16: PrintBigDigit(0x24,digitPos[0][0]);PrintBigDigit(0x38,digitPos[0][1]);break;
		case 17: PrintBigDigit(0x24,digitPos[0][0]);PrintBigDigit(0x3C,digitPos[0][1]);break;
		case 18: PrintBigDigit(0x24,digitPos[0][0]);PrintBigDigit(0x40,digitPos[0][1]);break;
		case 19: PrintBigDigit(0x24,digitPos[0][0]);PrintBigDigit(0x44,digitPos[0][1]);break;
		case 20: PrintBigDigit(0x28,digitPos[0][0]);PrintBigDigit(0x20,digitPos[0][1]);break;
		case 21: PrintBigDigit(0x28,digitPos[0][0]);PrintBigDigit(0x24,digitPos[0][1]);break;
		case 22: PrintBigDigit(0x28,digitPos[0][0]);PrintBigDigit(0x28,digitPos[0][1]);break;
		case 23: PrintBigDigit(0x28,digitPos[0][0]);PrintBigDigit(0x2C,digitPos[0][1]);break;
		case 0: PrintBigDigit(0x20,digitPos[0][0]);PrintBigDigit(0x20,digitPos[0][1]);break;
	}
	PrintMinutes();
	PrintSeconds();
}
void DisplayDigitalClock()
{
	if(Is12H)
		Print12HourClock();
	else
		Print24HourClock();
}

void DisplayDate()
{
	char timeBuffer[10];
	sprintf(timeBuffer,"%02d/%02d",day,month);
	oledPutString(timeBuffer, 7, 2);
}

void DisplayAnalogClock()
{

}

void DisplayClock()
{
	if(IsDigitalDisplay)
	{
		DisplayDigitalClock();

		//Print :
		oledWriteChar1x(0x6F,0xB3 , 37);
		oledWriteChar1x(0x6F,0xB4 , 37);
		oledWriteChar1x(0x6F,0xB3 , 82);
		oledWriteChar1x(0x6F,0xB4 , 82);
		
		DisplayDate();
	}
		
	else
		DisplayAnalogClock();
}

void DisplaySmallClock()
{
	char timeBuffer[10];
	sprintf(timeBuffer,"%02d:%02d:%02d",hour,minute,second);
	if(Is12H)
	{
		oledPutString(timeBuffer, 7, 60);
		if(IsAM)
			oledPutROMString((ROM_STRING)"AM",7,110);
		else
			oledPutROMString((ROM_STRING)"PM",7,110);
	}
	else
	{
		oledPutString(timeBuffer, 7, 80);
	}
		
}

int DisplayModeMenu()
{
	while(1)
	{
		int tmpUpBtn, tmpDownBtn;
		oledPutROMString((ROM_STRING)"Display Mode",0,30);
		oledPutROMString((ROM_STRING)"Digital",3,30);
		oledPutROMString((ROM_STRING)"Analog",5,30);
	
		//Navigate the menu through UP & DOWN buttons
		mTouchCalibrate();
		tmpUpBtn = ReadUpButton();
		if(tmpUpBtn == 1)
		{
			FillDisplay(0x00); //Clear Screen
			if(displayModeSelectedOption == 5)
				displayModeSelectedOption = 3;
			else
				displayModeSelectedOption = 5;
		}
	
		tmpDownBtn = ReadDownButton();
		if(tmpDownBtn == 1)
		{
			FillDisplay(0x00); //Clear Screen
			if(displayModeSelectedOption == 3)
				displayModeSelectedOption = 5;
			else
				displayModeSelectedOption = 3;
		}
		
		//Indicator of selected option	
		oledPutROMString((ROM_STRING)"*", displayModeSelectedOption, 22);
		oledPutROMString((ROM_STRING)"*", displayModeSelectedOption, 75);
		
		//Confirm selection by pressing the black button
		if(CheckButtonPressed())
		{
			if(displayModeSelectedOption == 3)
				IsDigitalDisplay = TRUE;
			else
				IsDigitalDisplay = FALSE;
			return 1;
		}
		
		//If the clock is set - display small clock while navigating the menu
		if(IsClockSet)
			DisplaySmallClock();
	}	
}

void ConvertClock()
{
	if(Is12H)
	{
		if(IsAM)
		{
			if(hour == 12)
				hour = 0;
		}
		else
		{
			switch(hour)
			{
				case 1: hour = 13;break;
				case 2: hour = 14;break;
				case 3: hour = 15;break;
				case 4: hour = 16;break;
				case 5: hour = 17;break;
				case 6: hour = 18;break;
				case 7: hour = 19;break;
				case 8: hour = 20;break;
				case 9: hour = 21;break;
				case 10: hour = 22;break;
				case 11: hour = 23;break;
			}
		}
	}
	else
	{
		switch(hour)
		{
			case 0: IsAM = TRUE; hour = 12;break;
			case 1: IsAM = TRUE;break;
			case 2: IsAM = TRUE;break;
			case 3: IsAM = TRUE;break;
			case 4: IsAM = TRUE;break;
			case 5: IsAM = TRUE;break;
			case 6: IsAM = TRUE;break;
			case 7: IsAM = TRUE;break;
			case 8: IsAM = TRUE;break;
			case 9: IsAM = TRUE;break;
			case 10: IsAM = TRUE;break;
			case 11: IsAM = TRUE;break;
			case 12: IsAM = FALSE;break;
			case 13: IsAM = FALSE; hour = 1;break;
			case 14: IsAM = FALSE; hour = 2;break;
			case 15: IsAM = FALSE; hour = 3;break;
			case 16: IsAM = FALSE; hour = 4;break;
			case 17: IsAM = FALSE; hour = 5;break;
			case 18: IsAM = FALSE; hour = 6;break;
			case 19: IsAM = FALSE; hour = 7;break;
			case 20: IsAM = FALSE; hour = 8;break;
			case 21: IsAM = FALSE; hour = 9;break;
			case 22: IsAM = FALSE; hour = 10;break;
			case 23: IsAM = FALSE; hour = 11;break;
		}
	}
}

int DisplayIntervalMenu()
{
	while(1)
	{
		int tmpUpBtn, tmpDownBtn;
		oledPutROMString((ROM_STRING)"Interval Menu",0,30);
		oledPutROMString((ROM_STRING)"12H",3,50);
		oledPutROMString((ROM_STRING)"24H",5,50);
	
		//Navigate the menu through UP & DOWN buttons
		mTouchCalibrate();
		tmpUpBtn = ReadUpButton();
		if(tmpUpBtn == 1)
		{
			FillDisplay(0x00); //Clear Screen
			if(displayIntervaSelectedOption == 5)
				displayIntervaSelectedOption = 3;
			else
				displayIntervaSelectedOption = 5;
		}
	
		tmpDownBtn = ReadDownButton();
		if(tmpDownBtn == 1)
		{
			FillDisplay(0x00); //Clear Screen
			if(displayIntervaSelectedOption == 3)
				displayIntervaSelectedOption = 5;
			else
				displayIntervaSelectedOption = 3;
		}
		
		//Indicator of selected option	
		oledPutROMString((ROM_STRING)"*", displayIntervaSelectedOption, 42);
		oledPutROMString((ROM_STRING)"*", displayIntervaSelectedOption, 70);
		
		//Confirm selection by pressing the black button + Convert clock to 12H / 24H if needed
		if(CheckButtonPressed())
		{
			if(displayIntervaSelectedOption == 3 && Is12H != TRUE)
			{
				if(IsClockSet)
					ConvertClock();
				Is12H = TRUE;
			}			
			else if(displayIntervaSelectedOption == 5 && Is12H != FALSE)
			{
				if(IsClockSet)
					ConvertClock();
				Is12H = FALSE;
			}		
			return 1;
		}

		//If the clock is set - display small clock while navigating the menu
		if(IsClockSet)
			DisplaySmallClock();
	}	
}

int DisplayAmPmMenu()
{
	while(1)
	{
		if(Is12H)
		{
			int tmpUpBtn, tmpDownBtn;
			oledPutROMString((ROM_STRING)"AM/PM Menu",0,30);
			oledPutROMString((ROM_STRING)"AM",3,50);
			oledPutROMString((ROM_STRING)"PM",5,50);
		
			//Navigate the menu through UP & DOWN buttons
			mTouchCalibrate();
			tmpUpBtn = ReadUpButton();
			if(tmpUpBtn == 1)
			{
				FillDisplay(0x00); //Clear Screen
				if(amPmSelectedOption == 5)
					amPmSelectedOption = 3;
				else
					amPmSelectedOption = 5;
			}
		
			tmpDownBtn = ReadDownButton();
			if(tmpDownBtn == 1)
			{
				FillDisplay(0x00); //Clear Screen
				if(amPmSelectedOption == 3)
					amPmSelectedOption = 5;
				else
					amPmSelectedOption = 3;
			}
			
			//Indicator of selected option	
			oledPutROMString((ROM_STRING)"*", amPmSelectedOption, 42);
			oledPutROMString((ROM_STRING)"*", amPmSelectedOption, 64);
	
			//Confirm selection by pressing the black button
			if(CheckButtonPressed())
			{
				if(amPmSelectedOption == 3)
					IsAM = TRUE;
				else
					IsAM = FALSE;
				return 1;
			}
		}
		//Print error if clock is not set to 12H
		else
		{
			oledPutROMString((ROM_STRING)"Clock is not set ", 2, 0);
			oledPutROMString((ROM_STRING)"to 12H!", 3, 0);
			oledPutROMString((ROM_STRING)"Press button ", 5, 0);
			oledPutROMString((ROM_STRING)"to return.", 6, 0);
			//Press black button to return
			if(CheckButtonPressed())
				return 1;
		}

		//If the clock is set - display small clock while navigating the menu
		if(IsClockSet)
			DisplaySmallClock();
	}
}

void ChangeHour()
{
	int tmpUpBtn, tmpDownBtn;
	//Set hour through UP & DOWN buttons
	mTouchCalibrate();
	tmpUpBtn = ReadUpButton();
	if(tmpUpBtn == 1)
	{
		FillDisplay(0x00); //Clear Screen
		tmpHour++;
		if(Is12H)
		{			
			if(tmpHour > 12)
				tmpHour = 1;
		}
		else
		{
			if(tmpHour > 23)
				tmpHour = 0;
		}
	}

	tmpDownBtn = ReadDownButton();
	if(tmpDownBtn == 1)
	{
		FillDisplay(0x00); //Clear Screen
		tmpHour--;
		if(Is12H)
		{
			if(tmpHour < 1)
				tmpHour = 12;
		}
		else
		{
			if(tmpHour < 0)
				tmpHour = 23;
		}
	}	
}

void ChangeMinute()
{
	int tmpUpBtn, tmpDownBtn;
	//Set minute through UP & DOWN buttons
	mTouchCalibrate();
	tmpUpBtn = ReadUpButton();
	if(tmpUpBtn == 1)
	{
		FillDisplay(0x00); //Clear Screen
		tmpMinute++;
		if(tmpMinute > 59)	
			tmpMinute = 0;
	}

	tmpDownBtn = ReadDownButton();
	if(tmpDownBtn == 1)
	{
		FillDisplay(0x00); //Clear Screen
		tmpMinute--;
		if(tmpMinute < 0)
			tmpMinute = 59;
	}	
}

void ChangeSecond()
{
	int tmpUpBtn, tmpDownBtn;
	//Set second through UP & DOWN buttons
	mTouchCalibrate();
	tmpUpBtn = ReadUpButton();
	if(tmpUpBtn == 1)
	{
		FillDisplay(0x00); //Clear Screen
		tmpSecond++;
		if(tmpSecond > 59)	
			tmpSecond = 0;
	}

	tmpDownBtn = ReadDownButton();
	if(tmpDownBtn == 1)
	{
		FillDisplay(0x00); //Clear Screen
		tmpSecond--;
		if(tmpSecond < 0)
			tmpSecond = 59;
	}
}

int DisplaySetTimeMenu()
{
	while(1)
	{
		char timeBuffer[10];
		int tmpLeftBtn, tmpRightBtn;
		oledPutROMString((ROM_STRING)"Set Time Menu",0,25);
		sprintf(timeBuffer,"%02d:%02d:%02d",tmpHour,tmpMinute,tmpSecond);
		oledPutString(timeBuffer, 4, 40);
		
		//Navigate the menu through LEFT & RIGHT buttons
		mTouchCalibrate();
		tmpRightBtn = ReadsRButton();
		if(tmpRightBtn == 1)
		{
			FillDisplay(0x00); //Clear Screen
			switch(setTimeSelectedOption)
			{
				case 43: setTimeSelectedOption = 61;break;
				case 61: setTimeSelectedOption = 79;break;
				case 79: setTimeSelectedOption = 43;break;
			}
		}

		tmpLeftBtn = ReadsLButton();
		if(tmpLeftBtn == 1)
		{
			FillDisplay(0x00); //Clear Screen
			switch(setTimeSelectedOption)
			{
				case 43: setTimeSelectedOption = 79;break;
				case 61: setTimeSelectedOption = 43;break;
				case 79: setTimeSelectedOption = 61;break;
			}
		}
	
		//Indicator of selected option	
		//43 - hour | 61 - minute | 79 - second
		oledPutROMString((ROM_STRING)"*", 3, setTimeSelectedOption);
		oledPutROMString((ROM_STRING)"*", 5, setTimeSelectedOption);

		//Change hour / minute / second
		switch(setTimeSelectedOption)
		{
			case 43: ChangeHour();break;
			case 61: ChangeMinute();break;
			case 79: ChangeSecond();break;
		}

		//Confirm time by pressing the black button
		if(CheckButtonPressed())
		{
			hour = tmpHour;
			minute = tmpMinute;
			second = tmpSecond;
			IsClockSet = TRUE;
			return 1;
		}

		//If the clock is set - display small clock while navigating the menu
		if(IsClockSet)
			DisplaySmallClock();
	}
}

void ChangeMonth()
{
	int tmpUpBtn, tmpDownBtn;
	//Set second through UP & DOWN buttons
	mTouchCalibrate();
	tmpUpBtn = ReadUpButton();
	if(tmpUpBtn == 1)
	{
		FillDisplay(0x00); //Clear Screen
		month++;
		if(month > 12)
			month = 1;
	}

	tmpDownBtn = ReadDownButton();
	if(tmpDownBtn == 1)
	{
		FillDisplay(0x00); //Clear Screen
		month--;
		if(month < 1)
			month = 12;
	}
}

void ChangeDay()
{
	int tmpUpBtn, tmpDownBtn;
	//Set second through UP & DOWN buttons
	mTouchCalibrate();
	tmpUpBtn = ReadUpButton();
	if(tmpUpBtn == 1)
	{
		FillDisplay(0x00); //Clear Screen
		day++;
		if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
		{
			if(day > 31)
				day = 1;
		}
		else if(month == 4 || month == 6 || month == 9 || month == 11)
		{
			if(day > 30)
				day = 1;
		}
		else
		{
			if(day > 28)
				day = 1;
		}
	}

	tmpDownBtn = ReadDownButton();
	if(tmpDownBtn == 1)
	{
		FillDisplay(0x00); //Clear Screen
		day--;
		if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
		{
			if(day < 1)
				day = 31;
		}
		else if(month == 4 || month == 6 || month == 9 || month == 11)
		{
			if(day < 1)
				day = 30;
		}
		else
		{
			if(day < 1)
				day = 28;
		}
	}
	
}

int DisplaySetDate()
{
	while(1)
	{	
		char timeBuffer[10];
		int tmpLeftBtn, tmpRightBtn;
		oledPutROMString((ROM_STRING)"Set Date Menu",0,25);
		sprintf(timeBuffer,"%02d/%02d",day,month);
		oledPutString(timeBuffer, 4, 45);

		//Navigate the menu through LEFT & RIGHT buttons
		mTouchCalibrate();
		tmpRightBtn = ReadsRButton();
		if(tmpRightBtn == 1)
		{
			FillDisplay(0x00); //Clear Screen
			switch(setDateSelectedOption)
			{
				case 48: setDateSelectedOption = 66;break;
				case 66: setDateSelectedOption = 48;break;
			}
		}

		tmpLeftBtn = ReadsLButton();
		if(tmpLeftBtn == 1)
		{
			FillDisplay(0x00); //Clear Screen
			switch(setDateSelectedOption)
			{
				case 48: setDateSelectedOption = 66;break;
				case 66: setDateSelectedOption = 48;break;
			}
		}
		
		//Indicator of selected option	
		//48 - day | 66 - month 
		oledPutROMString((ROM_STRING)"*", 3, setDateSelectedOption);
		oledPutROMString((ROM_STRING)"*", 5, setDateSelectedOption);

		//Chane month / day
		switch(setDateSelectedOption)
		{
			case 48: ChangeDay();break;
			case 66: ChangeMonth();break;
		}

		//Confirm date by pressing the black button
		if(CheckButtonPressed())
		{
			return 1;
		}

		//If the clock is set - display small clock while navigating the menu
		if(IsClockSet)
			DisplaySmallClock();
	}
}

int DisplayAlarmMenu()
{
	oledPutROMString((ROM_STRING)"Alarm Menu",0,10);
}

void SelectedSubMenu()
{
	FillDisplay(0x00); //Clear Screen
	while(1)
	{
		int goBack;
		switch(mainMenuSelectedOption)
		{
			case 1: if(DisplayModeMenu() == 1)
					{
						FillDisplay(0x00); //Clear Screen
						return;
					}
					break;
			case 2: if(DisplayIntervalMenu() == 1)
					{
						FillDisplay(0x00); //Clear Screen
						return;
					}
					break;
			case 3: if(DisplayAmPmMenu() == 1)
					{
						FillDisplay(0x00); //Clear Screen
						return;
					}
					break;
			case 4: if(DisplaySetTimeMenu() == 1)
					{
						FillDisplay(0x00); //Clear Screen
						return;
					}
					break;
			case 5: if(DisplaySetDate() == 1)
					{
						FillDisplay(0x00); //Clear Screen
						return;
					}
					break;
			case 6: if(DisplayAlarmMenu() == 1)
					{
						FillDisplay(0x00); //Clear Screen
						return;
					}
					break;
		}

		goBack = ReadsLButton();
		if(goBack == 1)
		{
			FillDisplay(0x00); //Clear Screen
			return;
		}	
	}
}

void DisplayMenu()
{
	while(1)
	{
		int tmpUpBtn, tmpDownBtn, goBack;
		oledPutROMString((ROM_STRING)"Display Mode",1,6);
		oledPutROMString((ROM_STRING)"12H/24H Interval",2,6);
		oledPutROMString((ROM_STRING)"Set AM/PM",3,6);
		oledPutROMString((ROM_STRING)"Set Time",4,6);
		oledPutROMString((ROM_STRING)"Set Date",5,6);
		oledPutROMString((ROM_STRING)"Alarm",6,6);
		
		//Indicator of selected option	
		oledPutROMString((ROM_STRING)"*", mainMenuSelectedOption, 0);
		oledPutROMString((ROM_STRING)"*", mainMenuSelectedOption, 100);
		
		//Navigate the menu through UP & DOWN buttons
		mTouchCalibrate();
		tmpUpBtn = ReadUpButton();
		if(tmpUpBtn == 1)
		{
			FillDisplay(0x00); //Clear Screen
			mainMenuSelectedOption--;
			if(mainMenuSelectedOption < 1)
				mainMenuSelectedOption = 6;
		}
	
		tmpDownBtn = ReadDownButton();
		if(tmpDownBtn == 1)
		{
			FillDisplay(0x00); //Clear Screen
			mainMenuSelectedOption++;
			if(mainMenuSelectedOption > 6)
				mainMenuSelectedOption = 1;
		}
		
		//Check if any option is selected by pressing the black button
		if(CheckButtonPressed())
		{
			SelectedSubMenu();
		}
		
		//Check if the the L button is pressed in order to go back to previous menu
		goBack = ReadsLButton();
		if(goBack == 1)
		{
			IsMenuOpen = FALSE;
			mainMenuSelectedOption = 1;
			FillDisplay(0x00); //Clear Screen
			return;
		}	
		
		//If the clock is set - display small clock while navigating the menu
		if(IsClockSet)
			DisplaySmallClock();
	}
}






/********************************************************************
 * Function:        void main(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Main program entry point.
 *
 * Note:            None
 *******************************************************************/
void main(void)
{

    InitializeSystem();

	// Initialize Timer 0
	T0CON = 0x05;
	// Initialize Timer Interrupt
	RCONbits.IPEN = 1 ;			//Prio Enable
	INTCON2bits.TMR0IP = 1 ;	//Use Hi-Prio
	INTCON = 0xE0 ;				//Enable Timer Interrupt

	T0CON |= 0x80 ;				//Start the Timer

    while(1)							//Main is Usualy an Endless Loop
    {
    // Application-specific tasks.
    // Application related code may be added here

		if(IsClockSet)
		{
			if(IsMenuOpen == FALSE)
				DisplayClock();
			else
				DisplayMenu();
			if(CheckButtonPressed())
			{
				FillDisplay(0x00); //Clear Screen
				IsMenuOpen = TRUE;
			}
				
		}
			
		else
		{
			if(IsMenuOpen == FALSE)
			{
				oledPutROMString((ROM_STRING)"Clock is not set!",3,10);
				oledPutROMString((ROM_STRING)"Black button for menu",5,2);
				if(CheckButtonPressed())
				{
					IsMenuOpen = TRUE;
					FillDisplay(0x00); //Clear Screen
				}
			}
			else
				DisplayMenu();	
		}		
    }
}//end main


/** EOF main.c *************************************************/
//#endif
