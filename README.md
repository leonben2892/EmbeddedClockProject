# PIC18F Controller Clock

Designing & implementing a clock which includes the following:
- Choosing display mode
  - Analog
  - Digital
- Choosing interval mode
  - 12H
  - 24H
- Setting clock to AM / PM if clock is set to 12H interval
- Setting clock time
- Setting clock date
- Setting alarm time & toggle ON / OFF
- Choose analog clock design
<div style="text-align:center"><img src ="https://user-images.githubusercontent.com/12692788/55685556-618eed00-5960-11e9-9fbb-70fce2722375.jpg" height=400/></div>

## Clock Overview
### First Screen
![FirstScreen](https://user-images.githubusercontent.com/12692788/55684831-79fb0980-5958-11e9-9c17-c8e83cb9d220.jpg)

**Description:** The first screen when booting up the clock.

**Navigation**
* Enter menu - Black button

### Main Menu
![MainMenu](https://user-images.githubusercontent.com/12692788/55684874-073e5e00-5959-11e9-8c41-48cbdde82f12.jpg)

**Description:** Navigating through the various options of the clock.

**Navigation**
* Upper Option - UP Key
* Lower Option - DOWN key
* Selecting Option - Black button
* Return - L button

### Display Mode
![DisplayMode](https://user-images.githubusercontent.com/12692788/55684917-60a68d00-5959-11e9-80f5-d1d8b41cb50b.jpg)

**Description:** Choosing the display mode of the clock.

**Navigation**
* Upper Option - UP Key
* Lower Option - DOWN key
* Selecting Option / Return - Black button

### Interval Menu
![IntervalMenu](https://user-images.githubusercontent.com/12692788/55684939-964b7600-5959-11e9-8ae6-82cca09c9aae.jpg)

**Description:** If clock is set to digital mode then the user can choose between 12H / 24H. Error message will appear when clock is set to analog mode:

![ClockIsSetToAnalog](https://user-images.githubusercontent.com/12692788/55715146-cf88f200-59fc-11e9-961b-2961b8461502.jpg)

**Navigation**
* Upper Option - UP Key
* Lower Option - DOWN key
* Selecting Option / Return - Black button

### AM / PM Menu
![AmPmMenu](https://user-images.githubusercontent.com/12692788/55684959-c1ce6080-5959-11e9-9f9f-d13ff0142365.jpg)

**Description:** If clock is set to 12H interval then the user can choose between AM / PM. Error message will appear when clock is set to 24H interval:

![ClockIsNot12H](https://user-images.githubusercontent.com/12692788/55684996-1d98e980-595a-11e9-81f5-550ecda1bcde.jpg)

**Navigation**
* Upper Option - UP Key
* Lower Option - DOWN key
* Selecting Option / Return - Black button

### Set Time Menu
![SetTime](https://user-images.githubusercontent.com/12692788/55685007-40c39900-595a-11e9-9510-e6c2804e88bd.jpg)

**Description:** Setting the clock time. When the clock time is set, a small digital display will appear in the bottom right corner throughout menu navigation. 

| | ||
| ------------- | ------------- | ------------- |
| ![MainMenuSmallClock](https://user-images.githubusercontent.com/12692788/55685489-9e0e1900-595f-11e9-86ee-58d62bbf1e59.jpg) | ![SmallClock1](https://user-images.githubusercontent.com/12692788/55715422-5e960a00-59fd-11e9-9d29-bc1a020144e5.jpg) | ![SmallClock2](https://user-images.githubusercontent.com/12692788/55715482-7ec5c900-59fd-11e9-8926-678429298015.jpg)  |

**Navigation**
* Changing numbers - UP /DOWN keys
* Switch between Hour/Minute/Second - L / R keys
* Confirm Time / Return - Black button

### Set Date Menu
![SetDate](https://user-images.githubusercontent.com/12692788/55685043-ad3e9800-595a-11e9-9853-31bb84b447dd.jpg)

**Description:** Setting the clock date.

**Navigation**
* Changing numbers - UP /DOWN keys
* Switch between Day/Month - L / R keys
* Confirm Date / Return - Black button

### Alarm Menu
![AlarmMenu](https://user-images.githubusercontent.com/12692788/55685073-e6770800-595a-11e9-81f8-b4b4328a6571.jpg)

**Description:** Setting the clock alarm. When the alarm is set and activated, an icon will appear on the top left corner.

1. Set Alarm Time

![AlarmTime](https://user-images.githubusercontent.com/12692788/55685093-132b1f80-595b-11e9-84cf-2f60d4b19313.jpg)

**Set Alarm Time Navigation:** 
* Changing numbers - UP /DOWN keys
* Switch between Hour/Minute - L / R keys
* Confirm alarm time / Return - Black button

2. Toggle alarm ON / OFF

![ToggleAlarm](https://user-images.githubusercontent.com/12692788/55685115-3eae0a00-595b-11e9-80ba-67e0274d9c5a.jpg)

**Toggle Alarm Navigation:** 
* Upper Option - UP Key
* Lower Option - DOWN key
* Selecting Option / Return - Black button

**Navigation**
* Upper Option - UP Key
* Lower Option - DOWN key
* Selecting Option - Black button
* Return - L Key

### Analog Design
![AnalogDesign](https://user-images.githubusercontent.com/12692788/55685175-cb58c800-595b-11e9-9b3b-487bf4da3733.jpg)

**Description:** Choosing between different options of analog clock design.

**Navigation**
* Upper Option - UP Key
* Lower Option - DOWN key
* Selecting Option / Return - Black button

### Digital Time View

| 12H Interval w/ Alarm OFF  | 12H Interval w/ Alarm ON | 24H Interval w/ Alarm ON |
| ------------- | ------------- | -------------|
| ![DigitalTime12Ham](https://user-images.githubusercontent.com/12692788/55685209-1377ea80-595c-11e9-9027-e94ac535b0fc.jpg) | ![DigitalTime12Hpm](https://user-images.githubusercontent.com/12692788/55685233-55089580-595c-11e9-8ebb-24c2e846883d.jpg)  | ![DigitalTime24H](https://user-images.githubusercontent.com/12692788/55685309-3c4caf80-595d-11e9-8371-93dedfbfe6ed.jpg)|

**Description:** Displaying time in digital mode represented by 12H /24H interval.

**Navigation**
* Opening Main Menu - Black button

### Analog Time View

| Design v1  | Design v2 | Design v3 |
| ------------- | ------------- | -------------|
| ![Design1](https://user-images.githubusercontent.com/12692788/55685357-bc731500-595d-11e9-913e-067f347f872e.jpg) | ![Design2](https://user-images.githubusercontent.com/12692788/55685358-c85ed700-595d-11e9-8278-fbb7cdc0f1c4.jpg)  | ![DesignV3](https://user-images.githubusercontent.com/12692788/55715383-4b833a00-59fd-11e9-8cb7-38f03168fe7b.jpg)|

**Description:** Displaying time in analog mode.

**Navigation**
* Opening Main Menu - Black button

### Triggered Alarm
|   | |
| ------------- | ------------- |
| ![AlarmTriggered1](https://user-images.githubusercontent.com/12692788/55685396-563ac200-595e-11e9-9f36-74ce1db1fb75.jpg) | ![AlarmTriggered2](https://user-images.githubusercontent.com/12692788/55685399-5e92fd00-595e-11e9-9ef0-6a31776eacab.jpg)


**Description:** When the alarm is triggered, the screen will flicker for 20 seconds or until user will press the black button.

**Navigation**
* Stop Alarm - Black button
* Opening Main Menu - Black button (After the alarm is canceled)


## Course Information
**Course Name:** Introduction to Embedded Systems

**Head Lecturer & Tutor** 
* Dr. Amit Resh
