/*
 * File:   test.c
 * Author: fathe
 *
 * Created on April 13, 2022, 9:13 AM
 */



// PIC18F4620 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config OSC = XT         // Oscillator Selection bits (XT oscillator)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = ON         // Watchdog Timer Enable bit (WDT enabled)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)
#define _XTAL_FREQ   4000000UL  
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>

#include "my_ser.h"
#include "lcd_x8.h"
#include "my_adc.h"
float SP;
char F[64];
char J[64];

/*void EXT_Int0_isr(void) {
    INTCONbits.INT0IF = 0;
    sprintf(F, "RE SET");

    sprintf(J, "|");
    delay_ms(2000);
    
}

void __interrupt(high_priority)highIsr(void) {
    if (INTCONbits.INT0IF)EXT_Int0_isr(); 
}*/
void initPorts(void){
    ADCON1 = 0x00;
    ADCON0 = 0;
    ADCON1 = 0b00001100; // Because We need AN2/AN0, Then we need 3 analog inputs
    LATA = LATB = LATC = LATD = LATE = 0;
    TRISA = 0xFF;
    TRISB = 0xFF;
    TRISD = TRISE = 0;
    TRISC = 0b10000000;//0x80
}

void display(void) {

    //////////////////////////////////////////////////////
    lcd_gotoxy(1, 1);
    sprintf(F, "Temp: %5.1fC", SP);
    lcd_puts(F);
    
    lcd_gotoxy(1, 2);
    sprintf(J, "FATHI");
    lcd_puts(J);  
    
}

void delay_ms(unsigned int n)
{
    int i;
    for (i=0; i < n; i++){
         __delaywdt_ms(1) ; 
    }
}

void main(void) {
    INTCON = 0;
    initPorts();
    setupSerial();
    init_adc_no_lib();
    
    lcd_init();
    lcd_send_byte(0, 1);
    lcd_putc('\f');
    display();
    //delay_ms(1000*20);
    send_string_no_lib("");
    send_string_no_lib("");
    lcd_putc('\f');
 char Buffer[64];
    while(1){ 
        CLRWDT();
        SP = (read_adc_voltage(0)*100.0);
        
        
        if(SP > 28){
        PORTEbits.RE0 = 1;
        }
        else PORTEbits.RE0 = 0;
       
        sprintf(Buffer, "T:%5.1fC\r\n", SP);
        send_string_no_lib(Buffer);
        if(SP > 28){
            sprintf(Buffer, "WARNING!!\r\n");
        }
        else sprintf(Buffer, "GOOD\r\n");
        send_string_no_lib(Buffer);
        sprintf(Buffer, "========================================\r\n");
        send_string_no_lib(Buffer);
        display();
        delay_ms(2000);
        
    }
    
    
    
    
    return; 
}
