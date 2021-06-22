# High/Low Game on PIC18

## Description

This *HiLo* project utilizes the PIC18f45k22 microcontroller from Microchip. The purpose behind this project is to create a High-Low game where the user enters sequentially through the mean of a phone keypad 3 digits. After the 3 digits have been successfully entered and the key-button enter is pressed, the program routine will compare this number against a randomly generated number by the start of each match. If the user presses the back button on the key-pad the current digit being entered is erased and the user can then enter another value instead. After the win situation happens and the number is guessed, the system restarts and the game starts again.


## Requirements / Environment
- MPLAB X IDE
- Proteus Design Suite


## Algorithm flow
The basic concept behind the algorithm of this design is to create at first, several global variables needed to hold triggering signals to track game start, end, comparison results. The program starts off by setting up the system and displaying to the user the beginning messages (“PLAY”, “HILO”) followed by a cleared screen and 3 dashed digits where the user can input values. The user can then begin to take input from the key-pad into the microcontroller. This is basically a loop that awaits a game-over signal to stop and let the game system start again. If the user inputs a back value, the digit is erased, if the user inputs an enter button press the 3 digits are checked against the save hidden number only if 3 digits have been entered successfully before else the user can continue to enter numbers until the 3-digits limit is reached. The algorithm actively checks every digit entered against the corresponding hidden number digits which makes the process faster and smoother for the end result. in order to translate the entered key-codes extracted from the phone key-pad. I replaced the usage of an external encoder by my own encoder through C code by using a key-code table and obtaining for each key-press the corresponding index for that key value to later on extract the SS-code value from another array to send it over to the Screen display by implementing a refreshing function that keeps running in the background and keeps on displaying each digit in the 4-digit screen.


## Hardware layout

![hardware](https://user-images.githubusercontent.com/86275885/122958333-21599080-d359-11eb-8d5d-a9fb77a3a7c9.png)


## Conclusion
As I conclude this project, we can see that it has been an excellent demonstration of the high capabilities that the PIC18 microcontroller has to offer. Combined with the strength that C coding can have, I was able to implement using proteus for hardware simulation a fully working HiLo game which holds good efficiency, speed and nice user interface.

