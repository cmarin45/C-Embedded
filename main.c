#include "driverlib.h"
#include "typedef.h"
#include "msp430f5438a.h"

#define LED1_ON 0x01                    // HI P1.0
#define LED2_ON 0x02                    // HI P1.1
#define LED_OFF 0xFC                    // LO - P1.0+P1.1
#define BUTTONS 0x00                    // PIN2 - input
#define S1 0x40                         // P2.6
#define S2 0x80                         // P2.7
#define SELECT 0x08                     // P2.3
#define SW 0xC8                         // P2.3,6,7 - Input-Pullup
#define WD_DISABLE 0x5A80               // WDTCTL = WDTPW+WDTHOLD
#define WD_ENABLE 0x5A08                // Enable WD
#define TIMER_UP 0x0010                 // MC Up
#define SELREF_REFO 0X0021              // REFOCLK for UCSCTL3 + 2 div
#define SELM_MCLK 0X0244                // DOCCLKDIV for SELM and SELS and REFO for SELA UCSCTL4
#define DIVM_MCLK 0X0010                // Divider - 2 for SMCLK
#define FLLD_N 0x2200                   // FLLD - 4 / FLLN - 512
const char hello[] = {"\r\nNew Line\r\n"};
unsigned int i;
int value = 0;

void clock()
{
  int x = 100;
  P1DIR = BIT0+BIT1;
  P11DIR |= 0x07;                       // Output P11.0,P11.1,P11.2
  P11SEL |= 0x07;                       // Peripheral Output
  UCSCTL3 |= SELREF_REFO;               // REFOCLK
  UCSCTL4 |= SELM_MCLK;                 // MCLK - DCOCLK
  UCSCTL5 |= DIVM_MCLK;                 // DIVIDER - 2
  __bic_SR_register(SCG0);              // Disable the FLL control loop
  UCSCTL0 = 0x0000;                     // lowest bit counter
  UCSCTL1 = DCORSEL_5;                  // DCORSEL_5
  UCSCTL2 = FLLD_N;                     // Set MCLK to 8.333 Mhz and SMCLK to 4.167 Mhz and ACLK to 32.94 Khz
  __bic_SR_register(SCG0);              // Enable the FLL control loop
  __delay_cycles(350000);        
  do                                    // Wait for DCO to stabilize
   {
    UCSCTL7 &= ~DCOFFG;
   }while (UCSCTL7 & DCOFFG);           // Test oscillator fault flag
  SFRIFG1 &= ~OFIFG;                    // Clear OFIFG fault flag
  while(x>=0)
  {
   x--; 
   for(int z=0;z<30000;z++);
   P1OUT ^= LED1_ON;
  }
  P1OUT &= LED_OFF;
}

void timer()
{ 
  int j = 40;
  P4DIR = 0x01;
  P4SEL = 0x01;
  TB0CCTL0 = 0x0940;                    // sync,capture,toogle/reset
  TB0CCR0 = 4000;                       // TB0 counts 0.1213sec A and 0.0768s SM
  TB0CTL = 0x0114;                      // ALK,up
  while (j>=0)
  {
   if(TB0CTL & TBIFG)                   // if timer B0 finished counting
   {
    P1OUT ^= LED1_ON;   
    TB0CTL &= ~TBIFG;                   // count again  
    j--; 
   }                          
  }
  P1OUT &= LED_OFF;
}                               

void uart_read(const char *text)
{
  while (*text != 0)                    // While a char is TXed       
  {
    while (!(UCA1IFG&UCTXIFG));         // While the TX buffer is empty
    UCA1TXBUF = *text++;                // TX the characters
  }
}

int uart_write()
{
  int chr = 0;
  while(1)
  {
   while (UCA1IFG&UCRXIFG)              // While the flag is up
   {
    chr = UCA1RXBUF;
    if (chr == '\r')                    // If Enter is pressed
    {
     uart_read("\n\r");                 // Go to the begining of the line
     return 0;                          // Exit function
    }
    else 
     UCA1TXBUF = chr;                   // Else echo typed char
   }
  }
}

int uart_writeInfinite()
{
  int chr = 0;
  while(1)
  {
   while (UCA1IFG&UCRXIFG)              // While the flag is up
   {
    chr = UCA1RXBUF;
    if (chr == '\r')                    // If Enter is pressed
     uart_read("\n\r");                 // Go to the begining of the line
    else if (chr == 9)                  // if TAB is pressed exit
    { 
     return 0;
    }
    else 
     UCA1TXBUF = chr;                   // Else echo typed char
     P1OUT ^= LED2_ON;
   }
  }
}

void buttons()
{ 
  P2OUT = SW;                           // Buttons input
  P2REN = SW;                           // Enable pullup
  P2IE = SW;                            // Enable interrupt for P2.3,6,7
  P2IES = SW;                           // P2IFG flag is set with a high-to-low transition.
  P2IFG = 0x00;                         // No ISRs are pending
  uart_read("\n\rAti intrat in functia butoane, click switch pentru a iesi.\n\r");
  do                             
  { 
    while((S1&P2IN) == 0)               // Button S1 Pushed = True (P2.6)
    {
      P1OUT |= LED1_ON;
    }
    P1OUT &= LED_OFF;
    while((S2&P2IN) == 0)               // Button S2 Pushed = True (P2.7)
    {
      P1OUT |= LED2_ON;
    }
    P1OUT &= LED_OFF;
  } 
  while((SELECT&P2IN) != 0);            // While SW2 Select is not pushed 
}

void i2cUart()
{
  unsigned int data = 0x55;
  P5SEL = 0x10;                         // USCI_B1 SCL P5.4
  P5DIR = 0x10;
  P3SEL = 0x80;                         // SDA P3.7
  P3DIR = 0x80;
  UCB1CTL1 = 0x93;                      // UCSWRST + SMCLK + START + transmitter
  UCB1CTL0 = 0x0F;                      // Sync + I2C + Master
  UCB1I2CSA = 0x48;                     // Slave address
  UCB1BR0 = 0x28;                       // /40 ~100khz
  UCB1BR1 = 0x00;
  UCB1CTL1 &= 0x92;                     // Clear reset
  UCB1TXBUF = data;             
  P1OUT ^= LED2_ON;
  __delay_cycles(3000000);              // 3 sec delay
  P1OUT &= LED_OFF;
  P5SEL = 0xC0;                         // Enable UART
  P5DIR = 0x40;       
}

void spiUart()
{ 
  unsigned int data = 0x55;
  P5SEL = 0x30;                         // USCI_B1 TXD/RXD P5.4,5
  P5DIR = 0x30;                         // Clock P5.5
  P3DIR = 0xC0;                         // 3.6,7
  P3SEL = 0xC0;                         
  UCB1CTL1 |= 0x81;                     // Software reset enable + SMCLK
  UCB1CTL0 |= 0xA9;                     // Sync + Master + MSB + UCCKPH
  UCB1BR0 = 0x02;                       // /2
  UCB1BR1 = 0x00;
  UCB1CTL1 &= 0x80;
  UCB1TXBUF = data;             
  P1OUT ^= LED2_ON;
  __delay_cycles(3000000);              // 3 sec delay
  P1OUT &= LED_OFF;
  P5SEL = 0xC0;                         // Enable UART
  P5DIR = 0x40;                        
}

void usciUart()
{
  P5SEL = 0xC0;                         // USCI_A1 TXD/RXD P5.6,7 + Slave transmit enable
  P5DIR = 0x40;                         // P5.6 output
  UCA1CTL1 |= 0x41;                     // USCI logic held in reset state + CLK = ACLK
  UCA1BR0 |= 0x03;                      // 32khz - 9600 baud
  UCA1BR1 |= 0x00;
  UCA1MCTL = UCBRS_3+UCBRF_0;           // Modulation UCBRSx=3, UCBRFx=0
  UCA1CTL1 &= 0x40;                     // USCI state released for operation
  UCA1IE = 0x01;                        // Enable Interrupt
  uart_read("Introduceti prenumele: ");
  uart_write();
  uart_read("Introduceti numele: ");
  uart_write();
  uart_read("Descriere(TAB pentru a iesi):\r\n");
  uart_writeInfinite();
  P1OUT &= LED_OFF;
}

void options()
{ 
  int o;
  uart_read("\n\r\nAlegeti o optiune pentru a continua:\r\n");
  uart_read("1:Timer\n\r2:UART\n\r3:Buttons\n\r4:spi\n\r9:Exit Program\n\r");
  while(1)
  {      
   while (UCA1IFG&UCRXIFG)             
   {
    o = UCA1RXBUF;
    if (o == 49)
    {
     UCA1TXBUF = o;
     timer();
     options();
    }   
    else if (o == 50)
    {
     UCA1TXBUF = o;
     usciUart();
     options();
    }    
    else if (o == 51)
    { 
     UCA1TXBUF = o;
     buttons();
     options();
    }   
    else if (o == 52)
    { 
     UCA1TXBUF = o;
     i2cUart();
     //spiUart();
     options();
    }  
    else if (o == 57)
    {
     P1OUT &= LED_OFF;
     UCA1TXBUF = o;
     uart_read("\n\rApasa TAB pentru a reveni.\n\r");
     UCA1IE |= 0x01;                    // USCI Receive Interrupt Enable. UCRXIE = 1
     __bis_SR_register(LPM3_bits + GIE);// LPM3 + Interrupts enabled;}   
    }   
    else 
     uart_read("Optiunea aleasa nu este valida \r\n");
     options();
    }
   }
}

void main(void)
{
  WDTCTL = WD_DISABLE;                  // WD disabled 
  clock();
  timer();
  usciUart();
  buttons();
  options();
}

#pragma vector=PORT2_VECTOR             // Interrupt Service Routine
   __interrupt void PORT2(void)
  { 
    switch(P2IV)                        // Port 2 Interrupt Vector 
    { 
     case 0x0E:                         // P2.6 interrupt (S1)
       P1OUT ^= LED1_ON;                // Toggle LED1 when P2.6 is pushed
        break;
     case 0x10:                         // P2.7 interrupt (S2)
       P1OUT ^= LED2_ON;                // Toggle LED2 when P2.7 is pushed
       break;
     case 0x08:                         // P2.3 interrupt (SW-select)
       P1OUT ^= LED1_ON;                // Toggle both leds
       P1OUT ^= LED2_ON;
      break;
    }  
  }

#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
  switch(__even_in_range(UCA1IV,18))
  {
  case 0x00:
    break;                              // Vector 0 - no interrupt
  case 0x02:                            // Vector 2 - RXIFG
   while (!(UCA1IFG&UCTXIFG));          // While the TX buffer is empty
   value = UCA1RXBUF;   
   if (UCA1RXBUF == '\r')               // If ENTER is pressed, start
   { 
     i = 0;                             
     UCA1IE |= 0x02;                    // Enable UCTXIE 
   }
   if (value >= 97 && value <= 122)     // a – z = 97 to 122
     UCA1TXBUF = value - 32;            // echo capital letter
   else if (value >= 65 && value <= 90) // A – Z = 65 to 90
     UCA1TXBUF = value + 32;            // echo small letter
   else 
     UCA1TXBUF = value;                 // echo other chr
   if (value == 9)                      // if TAB is pressed go to
   { value = '\r';                      // options();
     UCA1TXBUF = value;
     if (P1OUT = 0x03)                  // If a led is on, turn it off
     P1OUT &= LED_OFF; 
     __bic_SR_register_on_exit (LPM3_bits);
     options();
   }
    break;
  case 0x04:                            // Vector 4 - TXIFG  
     UCA1TXBUF = hello[i++];            // TX next char 
     if (i == sizeof(hello))            // If end of string
     UCA1IE &= ~0x02;                   // Disable UCTXIE 
    break;                             
  default: 
    break;
  }
}

/*#pragma vector=TIMERB0_VECTOR           // TimerB0 Interrupt Service Routine
  __interrupt void TimerB0_ISR (void)
 { 
  switch(TA0IV) 
  { 
   case 2: 
   { 
    P1OUT ^= LED1_ON;
    TA0CCR1 += 40000; 
    break; 
   } 
  }
 }*/