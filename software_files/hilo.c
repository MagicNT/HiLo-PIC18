//#//###############################//#//
//#//       Tony Nasr:  HiLo        //#//
//#//###############################//#//

#include <p18cxxx.h>
#include <BCDlib.h>
#include <delays.h>

#define ClearRBIF()    { WREG = PORTB; Nop(); INTCONbits.RBIF = 0; }
#define Transition()   { display(Empty, Empty, Empty, Empty); Delay10KTCYx(15); }
#define Reset_screen() { display(Empty, Back, Back, Back); }
#define Buzzer PORTAbits.RA0

enum {Back = 9, Enter = 11, P = 12, L, A, Y, H, I, O, Notify, Empty}; // Assign numbers to each character
unsigned char digits[4] = {Empty, Empty, Empty, Empty}, tries_BCD[3], hiddenNumber[3],
              tries, digit_count, current_index, gameOver, result,
              i, dsp, qStates, hexKey; 
char rawKey;

// Corresponding Phone-KeyPad 8-bit KeyPress Values
char keyCodes[] = { 0b00110111, 0b01010111, 0b01100111,    //  1   2   3
                    0b00111011, 0b01011011, 0b01101011,    //  4   5   6 
                    0b00111101, 0b01011101, 0b01101101,    //  7   8   9
                    0b00111110, 0b01011110, 0b01101110 };  //  *   0   #
// SSD Code Table
char ssCodes[] = { 0x60, 0xDA, 0xF2,        //   1     2     3
                   0x66, 0xB6, 0xBE,        //   4     5     6
                   0xE0, 0xFE, 0xF6,        //   7     8     9
                   0x02, 0xFC, 0x00,        //  Back   0   Enter
                   0xCE, 0x1C, 0xEE, 0x76,  //   P     L     A     Y
                   0x6E, 0x08, 0x3A, 0x10,  //   H     i     o     _
                   0x00 };                  // Empty

//############################################################################

void setup(void);                      // System initialization
unsigned char get_key(void);           // Get user key-code           
void compare(void);                    // Compare each digit sequentially as user enters
void check(void);                      // Check if Win-High-low situation
void hi_or_lo(unsigned char);          // High-or-Low situation
void display(char, char, char, char);  // Display on Screen

//############################################################################

void main(void) { // Main program Routine
    while(1) {
        setup();
        Transition(); display(P, L, A, Y); Delay10KTCYx(100); // Display "PLAY"
        Transition(); display(H, I, L, O); Delay10KTCYx(100); // Display "HILO"
        Transition(); Reset_screen();                         // Clean Screen
        tries = digit_count = 0; current_index = 1; gameOver = result = 0; // Initialize variables
        do {
            hexKey = get_key();
            switch (hexKey) {                               // Obtain & check user input
                case Back:                                  // If back key pressed
                    digits[current_index] = Back;           // Display "-" on segment
                    if (current_index > 1) current_index--; // Decrement digit index
                    digit_count--;                          // Decrement digit count
                    break;  
                case Enter:                                 // If Enter key pressed
                    if (digit_count > 3) check();           // If all 3 digits entered
                    break;      
                default:
                    if (digit_count <= 3) {                 // If key is different than back & enter keys
                        digits[current_index++] = hexKey;   // Display appropriate key value
                        TRISB = 0x70;                       // Reset key value
                        digit_count++;                      // Increment digit count
                        if (result == 1 || result == 0) compare(); // compare current digit only if not H-or-L
                    }
                    break;  
            }
        } while (!gameOver); // Keep looping until win situation then start Game again
    }
}

//############################################################################

void setup(void) {
    ANSELAbits.ANSA0 = 0; TRISAbits.TRISA0 = 0;    // RA0 (Buzzer pin) Digital & Output     
    i = dsp = 0; qStates = 0b00001000; Buzzer = 0; // initialize variables, Buzzer turn-off   
    ANSELB = 0x00; TRISB = 0x70;                   // PORTB Digital & Cols are input & Rows are outs
    ANSELD = 0X00; TRISD = 0x00;                   // PORTD Digital Output
    ANSELC = 0x00; TRISC = 0x00;                   // PORTC Digital Output 
    PORTB = 0x00;                                  // connect rows to GND
    INTCON2bits.RBPU = 0;                          // enable internal pull-ups 
    ClearRBIF();                                   // just in case it was set      
    T0CON = 0b11010101;             
    TMR0L = 256 - 125;              
    INTCONbits.GIE = 1;                            // global mask  
    INTCONbits.RBIE = 1;                           // local mask    
    INTCONbits.TMR0IE = 1;                         // enable real-time interrupt  
    Bin2Bcd(TMR0L, (char *)hiddenNumber);          // Obtain random value from Timer0
}

unsigned char get_key(void) {
    Delay10KTCYx(2);                               // Wait for small while
    while ((~PORTB & 0x70) == 0);                  // Wait for user input
    rawKey = PORTB;                                // Get user input
    TRISB = 0x7F; TRISB = 0x0F;                    // Switch rows and columns
    rawKey |= PORTB;                               // Obtain key code
    for (i = 0; rawKey != keyCodes[i]; i++);       // Figure out index for table
    TRISB = 0x70;                                  // Reset keyboard
    ClearRBIF();                                   // Clear PORTB
    return --i;                                    // Return key index for table 
}

#pragma code ISR = 0x0008  // Interrupt place at address 0x0008
#pragma interrupt ISR      // ISR is an interrupt

void ISR(void) {
    INTCONbits.TMR0IF = 0;                           // Acknowledge interrupt
    TMR0L = 256 - 125;                               // 
    PORTD = 0x00;                                    // Remove shadowing 
    PORTC = qStates;                                 // Select 1 digit
    PORTD = ssCodes[digits[dsp++]];                  // Send data to SS display
    qStates >>= 1;                                   // Move on to next digit
    if (dsp == 4) { dsp = 0; qStates = 0b00001000; } // Restart from 1st digit
}

//############################################################################

void display(char a, char b, char c, char d) {
    digits[0] = a; digits[1] = b; digits[2] = c; digits[3] = d; // Display on each Segment
}

void compare(void) {
    if (digits[current_index-1] == hiddenNumber[current_index-1]) result = 1;      // Digit's Equal
    else if (digits[current_index-1] > hiddenNumber[current_index-1]) result = 2;  // Digit's High
    else result = 3;                                                               // Digit's Low
}

void check(void) {
    switch (result) {
        case 1:
            Bin2Bcd(tries,tries_BCD);  // Split trials count to separate digits
            for (i=0; i<3; i++) // Find corresponding SScode for the trials number
                if (tries_BCD[i] == 0)
                    tries_BCD[i] = 10;
                else tries_BCD[i] = tries_BCD[i] + 1;
            display(Notify, tries_BCD[0], tries_BCD[1], tries_BCD[2]); // Display number of trials
            for (i = 0; i < 6; i++) {  // Repeat 6 times
                Delay10KTCYx(50);      // 0.5s delay
                TRISD = ~TRISD;        // Blink display
                Buzzer = ~Buzzer;      // Make Buzzer sound
            }  
            gameOver = 1;              // End Game
            break;
        case 2: hi_or_lo(H); break;    // Digit entered is Low
        case 3: hi_or_lo(L); break;    // Digit entered is High  
    }
}

void hi_or_lo(unsigned char v) {
    tries++;                                // Increment Number of trials 
    digits[0] = v;                          // Display corresponding H or L
    Delay10KTCYx(150); Reset_screen();      // keep H or L displayed for a while
}

//############################################################################

