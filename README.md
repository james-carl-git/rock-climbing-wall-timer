# Rock Climbing Wall Timer
PCB for a timer on a rock climbing wall. Could be used for other timer functions

I built a rock wall for my niece and nephew. This repo is for the PCB I created that acts as a timer to measure how long it takes to climb the wall and determine who climbed it the fastest if 2 people are racing. 

Features:
- Simple PCB (2 layer, all through-hole) allows it to be easily hand soldered and you can use a CNC router to make it (this is what I did)
- Designed to be powered by batteries so has low power draw
- Buzzer/pizo speaker for an audible countdown before the timer starts (should be familiar if you have played those popular karting video games)
- 2x 7-segment LED arrays using 5mm LEDs, displays 0.0 to 9.9 seconds, and whole decimals after 10 seconds. 99s max time
- Inputs and outputs for an illuminated start button and 2 end buttons

I included CAD files for the 3d printed shroud I used separate the light from each segment.

Notes:
- LEDs in each digit are 3 LEDs wired in series and depending on what LEDs you use the voltage needed may be higher than the max voltage of the ATMega328P. As a result I have 2 separate inputs for power supply, one for the LEDs in the segmented display and one for the logic and the other LEDs/buzzer...etc.
- The current limiting resistors on the LEDs will be custom to what LEDs/supply voltage you choose.
- I used illuminated buttons for an arcade game as inputs

Known issues:

The TLC5940 LED driver I used has a high idle current(couple ma) which would prevent you from running this on batteries. When I was debugging this issue I was surprised to find that the TLC5940 works even with its VCC pin floating and doesn't seem to draw as much current, although the batteries did not last as long as I was expecting so this needs to be investigated further. I added a jumper on the PCB to allow you to easily connect/disconnect VCC on the TLC5940. Easy solution would be to just power this from a wall wart which is what I ended up doing.
