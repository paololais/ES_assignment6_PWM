/*
 * File:   main.c
 * Author: paolo
 *
 * Created on 13 maggio 2025, 10.26
 */


#include "xc.h"
#include "pwm.h"
#include "timer.h"

int PWM_state = 0; // 0=off, 1=on

// Interrupt per INT1 (pulsante)
void __attribute__((__interrupt__, __auto_psv__)) _INT1Interrupt(){
  IFS1bits.INT1IF = 0; // Reset del flag di interrupt
  IEC0bits.T3IE = 1; // Abilita l'interrupt del Timer 3
  tmr_setup_period(TIMER3, 10); // Imposta il debounce del pulsante
}

// Interrupt per il Timer 3
void __attribute__((__interrupt__, __auto_psv__)) _T3Interrupt(){
  IFS0bits.T3IF = 0; // Reset del flag di interrupt di Timer 3
  IEC0bits.T3IE = 0; // Disabilita l'interrupt di Timer 3
  T3CONbits.TON = 0; // Ferma il Timer 3
  IEC1bits.INT1IE = 1; // Riabilita l'interrupt su INT2

  if (PORTEbits.RE8 == 1) {
      PWM_state = !PWM_state;
  }
}

int main(void) {
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
     
    // button RE8 configuration
    TRISEbits.TRISE8 = 1; // Configura RE8 come input
    RPINR0bits.INT1R = 0x58; // Mappatura INT1 su RE8
    INTCON2bits.GIE = 1; // Abilita gli interrupt globali
    IFS1bits.INT1IF = 0; // Reset flag interrupt INT1
    IEC1bits.INT1IE = 1; // Abilita interrupt INT1
    
    pwm_init();
    
    while(1){
        if(PWM_state == 1){
            pwm_move_forward();
        } else {
            pwm_stop();
        }
    }
}
