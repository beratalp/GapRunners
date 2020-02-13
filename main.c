/*
 * File:   main.c
 * Author: beratalp
 *
 * Created on June 2, 2019, 5:41 PM
 */


// PIC16F88 Configuration Bit Settings
// 'C' source line config statements
// CONFIG1
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTRC oscillator; port I/O function on both RA6/OSC2/CLKO pin and RA7/OSC1/CLKI pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB0      // CCP1 Pin Selection bit (CCP1 function on RB0)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CONFIG2
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode enabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "uart.h"

#define _XTAL_FREQ 4000000
void writeData(char c);
float ultrasonicBack();
float distanceBack();
float ultrasonicFront();
float distanceFront();
float calculateDistance(float d);
void changeMode(char m);
float average(float *arr, short l);

char mode = '0';
short gap = 0;

void main(void) {
    OSCCON = 0b01101000;
    TRISB1 = 0;
    TRISB3 = 1;
    TRISA1 = 0;
    TRISA0 = 1;
    ANSEL = 0;
    T1CON = 0x10;
    INTCON = 0b10010000;
    UART_Init(9600);
    TRISA4 = 0;
    RA4 = 1;
    changeMode('0');
    __delay_ms(5000);
    distanceFront();
    RA4 = 0;
    __delay_ms(200);
    distanceBack();
    RA4 = 1;
    __delay_ms(200);
    RA4 = 0;
    changeMode('1');
    while(distanceFront() < 20);
    changeMode('0');
    RA4 = 1;
    __delay_ms(500);
    RA4 = 0;
    changeMode('5');
    __delay_ms(1000);
    changeMode('2');
    __delay_ms(2500);
    changeMode('4');
//    while(distanceBack() > 15);
//    changeMode('3');
//    gap++;
//    while(distanceFront() < 15);
//    changeMode('2');
//    __delay_ms(1000);
//    changeMode('1');
//    while(distanceBack() > 15);
//    changeMode('3');
//    gap++;
    __delay_ms(10000);
    while(1){
        changeMode('0');
    }
}

void writeData(char c){
    UART_Write(c);
}


float ultrasonicBack(){
    TMR1H = 0;
    TMR1L = 0;
    RA1 = 1;
    __delay_us(10);
    RA1 = 0;
    while(!RA0);
    T1CONbits.TMR1ON = 1;
    while(RA0);
    T1CONbits.TMR1ON = 0;
    
    float distance = (TMR1L | (TMR1H<<8));
    return calculateDistance(distance);
}

float ultrasonicFront(){
    TMR1H = 0;
    TMR1L = 0;
    RB1 = 1;
    __delay_us(10);
    RB1 = 0;
    while(!RB3);
    T1CONbits.TMR1ON = 1;
    while(RB3);
    T1CONbits.TMR1ON = 0;
    
    float distance = (TMR1L | (TMR1H<<8));
    return calculateDistance(distance);
}

float calculateDistance(float d){
    d = d / 58.82;
    d = d + 1;
    return d;
}

void changeMode(char m){
    mode = m;
    writeData(mode);
} 

void __interrupt() isr(){
    if(INTCONbits.RBIF == 1){
        if(gap == 1){
            changeMode('1');
        }
        else{
            changeMode('0');
        }
    }
}

float distanceBack(){
    float arr[10];
    for(int i = 0; i < 10; i++){
        arr[i] = ultrasonicBack();
    }
    return average(arr, 10);
}

float distanceFront(){
    float arr[10];
    for(int i = 0; i < 10; i++){
        arr[i] = ultrasonicFront();
    }
    return average(arr, 10);
}
float average(float *arr, short l){
    float sum = 0;
    for(int i = 0; i < l; i++){
        sum = sum + arr[i];
    }
    return sum / l;
}