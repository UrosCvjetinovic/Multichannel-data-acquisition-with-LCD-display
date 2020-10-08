/**
 * @project   1. Project IRS (2019/2020 school year)
 * @brief
 *  Program that enables acquisition of two channels from ADC12 with a press of a button.
 *  The acquisition can be disabled with the same button. Enabled changeable frequency (0.5Hz-2Hz)
 *  Acquisition starts with freq. of 0.5Hz, and can be changed to {0.5,0.75,1,1.25,1.5,1.75,2} HZ.
 *  Channels that are being acquired  are 14 and 15
 *
 * @date 2020
 * @author Uros Cvjetinovic (cu160093d@student.etf.bg.ac.rs)
 *
 */

#include <msp430.h> 
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

/**
 * @brief Timer period for debauncing using ganssle method
 * */
#define TIMER_PERIOD   (63) /* ~2ms (1.95ms) */
/**
 * @brief Delay of 1 ms for LCD
 * */
#define DELAY_1MS   (1005)
/**
 * @brief Delay 5 ms for LCD
 * */
#define DELAY_5MS   (1005*5)
/**
 * @brief Delay 15 ms for LCD
 * */
#define DELAY_15MS  (1005*15)
/**
 * @brief Register Select bit for lcd
 * */
#define LCD_REGISTER_SELECT      BIT2
/**
 * @brief Enable bit for lcd
 * */
#define LCD_ENABLE_BIT           BIT3
/**
 * @brief DATA = it is data it isn't instruction
 * */
#define DATA 1
/**
 * @brief INSTR = it is instruction it isn't data
 * */
#define INSTR 2
/**
 * @brief INIT = it is initialization it isn't working
 * */
#define INIT 3
/**
 * @brief NOT_INIT = it is working it isn't initialization
 * */
#define NOT_INIT 4
/**
 * @brief Sets lcd to work with 8bit interface
 * */
#define LCD_8BIT_INTERFACE       0x03
/**
 * @brief Sets lcd to work with 4bit interface
 * */
#define LCD_4BIT_INTERFACE       0x02
/**
 * @brief Sets lcd display style (font and 2 lines)
 * */
#define LCD_DISPLAY_STYLE        0x2C
/**
 * @brief Enables lcd display
 * */
#define LCD_DISPLAY_ON           0x0E
/**
 * @brief Disables lcd display
 * */
#define LCD_DISPLAY_OFF          0x08
/**
 * @brief Clears lcd display
 * */
#define LCD_DISPLAY_CLEAR        0x01
/**
 * @brief Lcd entry mode command
 * */
#define LCD_ENTRY_MODE           0x06
/**
 * @brief Lcd command to write in new line
 * */
#define LCD_NEW_LINE             0xC0

/**
 * @brief Periods used for acquisition
 * */
const int periods[] = {
          32767,    // 0.5  Hz
          28671,    // 0.75 Hz
          24575,    // 1    Hz
          20479,    // 1.25 Hz
          16383,    // 1.5  Hz
          12287,    // 1.75 Hz
          8191      // 2    Hz
};
/**
 * @brief Frequencies used for acquisition
 * */
const float freq[] = {
           50,     // 0.5  Hz
           75,     // 0.75 Hz
          100,     // 1    Hz
          125,     // 1.25 Hz
          150,     // 1.5  Hz
          175,     // 1.75 Hz
          200      // 2    Hz
};
/**
 * @brief Variable for displaying conversion speed
 * */
volatile uint8_t conv_speed = 0;

/**
 * @brief Print data on the (P8OUT) LCD
 */
void P8OUT_data(uint8_t data){
    P8OUT = data;
    __delay_cycles(DELAY_1MS);
    P8OUT &= ~LCD_ENABLE_BIT;    // clear E bit
    __delay_cycles(DELAY_1MS);
}
/**
 * @brief Prints whole number (higher and lower nibble)
 */
void put_on_LCD(uint8_t number){
    uint8_t h = number & 0xF0;
    uint8_t l = (number & 0x0F) << 4;
    h|= LCD_REGISTER_SELECT | LCD_ENABLE_BIT;
    l |= LCD_REGISTER_SELECT | LCD_ENABLE_BIT;
    P8OUT_data(h);
    P8OUT_data(l);
}

/**
 * @brief basic int "command" for the LCD
 *  and the int "work" with the uint8_t "data"
 *  command can be DATA or INSTR
 *  work can be INIT or NOT_INIT
 * */
void lcd_command(uint8_t number, int command, int work) {
    uint8_t h = number & 0xF0;         // high nibble
    uint8_t l = (number & 0x0F) << 4;  // low nibble
    if (command == INSTR) {
        h &= ~LCD_REGISTER_SELECT;  // clear RS bit for h
        h |= LCD_ENABLE_BIT;        // set E bit for h
        l &= ~LCD_REGISTER_SELECT;  // clear RS bit for l
        l |= LCD_ENABLE_BIT;        // set E bit for l
    }
    if (work == INIT){
        P8OUT_data(l);
    }
    else if (work == NOT_INIT) {
        P8OUT_data(h);
        P8OUT_data(l);
    }
}

/**
 * @brief Function that displays data
 */
void d2lcd(uint16_t data1, uint16_t data2){
    volatile int long voltage;
    volatile int speed;
    data1 %= 10000;
    data2 %= 10000;
    speed = freq[conv_speed];

    lcd_command(0x02, INSTR, NOT_INIT);    // LCD new line

    //## Displaying voltage1

    voltage = data1;
    voltage = (voltage * 300) / 4095 ;
    put_on_LCD(0x56);                           // V
    put_on_LCD(0x30 + 1);                       // 1
    put_on_LCD(0x3A);                           // :
    put_on_LCD(0x30 + (voltage / 100) % 10);    // higher digit
    put_on_LCD(0x2E);                           // dot
    put_on_LCD(0x30 + (voltage / 10) % 10);     // first decimal
    put_on_LCD(0x30 + voltage % 10);            // second decimal

    //## Displaying Speed of conversion (value of timers register)   for debuging

    put_on_LCD(0x20);                       // space
    put_on_LCD(0x20);                       // space
    put_on_LCD(0x66);                       // f
    put_on_LCD(0x3A);                       // :
    put_on_LCD(0x30 + speed / 100);         // whole part
    put_on_LCD(0x2E);                       // dot
    put_on_LCD(0x30 + (speed / 10) % 10);   // first decimal
    put_on_LCD(0x30 + speed % 10);          // second decimal

    lcd_command(LCD_NEW_LINE, INSTR, NOT_INIT);    // LCD new line

    //## Displaying voltage 2

    voltage = data2;
    voltage = (voltage * 300) / 4095 ;
    put_on_LCD(0x56);                           // V
    put_on_LCD(0x30 + 2);                       // 2
    put_on_LCD(0x3A);                           // :
    put_on_LCD(0x30 + (voltage / 100) % 10);    // whole part
    put_on_LCD(0x2E);                           // dot
    put_on_LCD(0x30 + (voltage / 10) % 10);     // first decimal
    put_on_LCD(0x30 + voltage % 10);            // second decimal

}
/**
 * @brief Initialization of the LCD
 * */
void init(){
    // power on
    __delay_cycles(DELAY_15MS);         // Wait 15 ms (Vcc > 15ms)
    lcd_command(LCD_8BIT_INTERFACE, INSTR, INIT); // 8-bit interface (rs = 0; r/~w=0; DB_7654 = 0011;)
    __delay_cycles(DELAY_5MS);          // Wait > 4.1ms
    uint16_t i = 0;
    uint8_t commands[9];
    commands[0] = LCD_8BIT_INTERFACE;   // 8-bit interface (rs = 0; r/~w=0; DB_7654 = 0011;)
    commands[1] = LCD_8BIT_INTERFACE;   // 8-bit interface (rs = 0; r/~w=0; DB_7654 = 0011;)
    commands[2] = LCD_4BIT_INTERFACE;   // 4-bit interface  (rs = 0; r/~w=0; DB_7654 = 0010;)
    commands[3] = LCD_DISPLAY_STYLE;    // 5x8 font and 2 lines (5bits horizontal and 8bits vertical)
    commands[4] = LCD_DISPLAY_OFF;      // display off
    commands[5] = LCD_DISPLAY_CLEAR;    // clear display
    commands[6] = LCD_ENTRY_MODE;       // entry mode
    commands[7] = LCD_DISPLAY_ON;       // display on
    commands[8] = LCD_ENTRY_MODE;       // entry mode
    for (i=0;i<9;++i)
        lcd_command(commands[i],INSTR,(i<3 ? INIT:NOT_INIT));
}
// Finished LCD

/**
 * @brief Variable for storing AD conversion of channel 1
 * */
volatile unsigned int ad_result1;
/**
 * @brief Variable for storing AD conversion of channel 2
 * */
volatile unsigned int ad_result2;
/**
 * @brief Flag for activating and deactivating AD conversion
 * */
volatile uint8_t active = 0;

/**
 * @brief GANSSLE debounce
 */
static volatile uint16_t state1 = 0; // For debouncing S1
static volatile uint16_t state2 = 0; // For debouncing S2
static volatile uint16_t state3 = 0; // For debouncing S3

/**
 * @brief main
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    // initialize button
    P2DIR &= ~BIT4;             // P2.4 in      //for enabling or disabling conversion
    P2IFG &= ~BIT4;             // clear flag
    P2DIR &= ~BIT5;             // P2.5 in      //for increasing frequency of conversion
    P2IFG &= ~BIT5;             // clear flag
    P2DIR &= ~BIT6;             // P2.5 in      //for decreasing frequency of conversion
    P2IFG &= ~BIT6;             // clear flag

    // initialize led                  // set P4.6-P4.3 as out
    initLEDs();
    //P4.3      // Acquisition enabled/disabled
    //P4.4      // Debouncing
    //P4.5      // Acquiring
    //P4.6      // LP_mode3

    // initialize lcd
    P8DIR |= 0xFC;              // set P8.7-P8.2 as out

    // initialize ADC
    P7SEL |= BIT6 | BIT7;                   // set P7.6, P7.7 for ADC
    ADC12CTL0 = ADC12ON | ADC12SHT0_8 | ADC12MSC;        // turn on ADC
    ADC12CTL1 = ADC12SHP | ADC12CONSEQ_1;   // set SHS = 1 (TA0.0 used for SAMPCON) and Repeat-sequence-of-channels mode
    ADC12MCTL0 = ADC12INCH_14;              // select channel 14
    ADC12MCTL1 = ADC12INCH_15+ADC12EOS;     // select channel 15, end seq.
    ADC12CTL0 |= ADC12ENC;                  // disable conversion              // will be changed with a press of a button
    ADC12IE = ADC12IE1;                     // disable interrupt for ADC12IFG.14 (ADC12MEM1)

    // initialize Timer B
    TA1CCR0 = TIMER_PERIOD;
    TA1CCTL0 = CCIE;
    TA1CTL = TASSEL__ACLK | MC__UP;

    init(); // lcd
    __enable_interrupt();

    while (1)
    {
        P4OUT |= BIT6;// Debug:  LP_mode3 = active
        // enter LPM mode
        __low_power_mode_3();
        __no_operation();
    }
}

/**
 * @brief TIMERA1 Interrupt service routine
 *
 * Used for debauncing
 */
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) TA1CCR0ISR (void)
{
    P4OUT |= BIT4;// Debug:  Debouncing
    state1 = (state1 << 1) | 0xe000 | ((P2IN & BIT4) ? 1 : 0);
    state2 = (state2 << 1) | 0xe000 | ((P2IN & BIT5) ? 1 : 0);
    state3 = (state3 << 1) | 0xe000 | ((P2IN & BIT6) ? 1 : 0);
    if(state1 == 0xf000){
        if (active == 0)
        {
            P4OUT |= BIT3;// Debug:  Acquisition = active
            TA0CCTL0 =  CCIE;                        // CCR0 interrupt enabled
            TA0CCR0 = periods[conv_speed];
            TA0CTL = TASSEL_1 | MC_1 | TACLR;       // ACLK, up mode, clear TAR
            active = 1;
            P4OUT &= ~BIT6;// Debug:  LP_mode3 = ~active
        }else
        {
            P4OUT &= ~BIT3;// Debug:  Acquisition = ~active
            TA0CCTL0 &= ~ CCIE;
            TA0CTL &= ~MC_3;
            active = 0;
            P4OUT |= BIT6;// Debug:  LP_mode3 = active
            // leave LPM
            __low_power_mode_off_on_exit();         // Turning off ADC, entering lp_mode
        }
    }
    if(state2 == 0xf000){
        if(conv_speed < 6){
            conv_speed++;
            TA0CCR0 = periods[conv_speed];              // f_OUT = 0.5 Hz, f_ACLK = 32768 Hz => T_OUT = 2*32768.
                                                      // TOGGLE outmod => T_OUT = 2 * T_CCR0 => T_CCR0 = 32768 (TA0CCR0 = T_CCR0 - 1)
            d2lcd(ad_result1,ad_result2);                  // Displaying acquired data
        }
    }
    if(state3 == 0xf000){
        if(conv_speed > 0){
            conv_speed--;
            TA0CCR0 = periods[conv_speed];              // f_OUT = 0.5 Hz, f_ACLK = 32768 Hz => T_OUT = 2*32768.
                                                      // TOGGLE outmod => T_OUT = 2 * T_CCR0 => T_CCR0 = 32768 (TA0CCR0 = T_CCR0 - 1)
            d2lcd(ad_result1,ad_result2);                  // Displaying acquired data
        }
    }
    P4OUT &= ~BIT4;// Debug:  Debouncing
}
/*
 * @brief ADC Interrupt service routine
 *
 */
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12ISR (void)
{
    if (ADC12IV == ADC12IV_ADC12IFG1) // check if converssion finished
    {
        ad_result1 = ADC12MEM0;
        ad_result2 = ADC12MEM1;
        d2lcd(ad_result1,ad_result2);   // Display values on LCD
        P4OUT &= ~BIT5;// Debug:  Acquired
    }
}

/**
 * @brief TIMERA0 Interrupt service routine
 *
 * Starts ADC
 */
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) CCR0ISR (void)
{
    P4OUT |= BIT5;// Debug: Acquiring
    ADC12CTL0 |= ADC12SC;           // start a conversion
}
