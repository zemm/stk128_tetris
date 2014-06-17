/*****************************************************
This program was produced by the
CodeWizardAVR V1.25.2 Standard
Automatic Program Generator
© Copyright 1998-2006 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com
 
Project : STK128_ZemmTrix
Version : v0.2
Date    : 18.12.2006
Author  : zemm
Company : Jyvaskylan ammattikorkeakoulu
Comments: 
Testailenpa omaa versiotani... tuskinpa tästä mitään tulee
 
SW0 käynnistää pelin alku- ja lopputilasta
 
Näppäimet:
 SW0 = tiputa palikka laudan pohjalle
 SW1 = käännä palikkaa vastapäivään
 SW2 = liikuta palikkaa oikealle
 SW3 = liikuta palikkaa vasemmalle
 SW4 = käännä palikkaa myötäpäivään
 SW6 = palaa pause-tilasta
 SW7 = mene pause-tilaan (keskeyttää pelin)
 
 
Chip type           : ATmega128
Program type        : Application
Clock frequency     : 4,000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 1024
*****************************************************/
 
#include <mega128.h>
 
// Alphanumeric LCD Module functions
#asm
   .equ __lcd_port=0x15 ;PORTC
#endasm
#include <lcd.h>
#include <delay.h>
 
#define RXB8 1
#define TXB8 0
#define UPE 2
#define OVR 3
#define FE 4
#define UDRE 5
#define RXC 7
 
#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<OVR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)
 
// USART0 Receiver buffer
#define RX_BUFFER_SIZE0 40
char rx_buffer0[RX_BUFFER_SIZE0];
 
#if RX_BUFFER_SIZE0<256
unsigned char rx_wr_index0,rx_rd_index0,rx_counter0;
#else
unsigned int rx_wr_index0,rx_rd_index0,rx_counter0;
#endif
 
// This flag is set on USART0 Receiver buffer overflow
bit rx_buffer_overflow0;
 
// USART0 Receiver interrupt service routine
interrupt [USART0_RXC] void usart0_rx_isr(void) {
  char status,data;
  status=UCSR0A;
  data=UDR0;
  if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0) {
    rx_buffer0[rx_wr_index0]=data;
    if (++rx_wr_index0 == RX_BUFFER_SIZE0) rx_wr_index0=0;
      if (++rx_counter0 == RX_BUFFER_SIZE0) {
        rx_counter0=0;
        rx_buffer_overflow0=1;
      };
  };
}
 
#ifndef _DEBUG_TERMINAL_IO_
// Get a character from the USART0 Receiver buffer
#define _ALTERNATE_GETCHAR_
#pragma used+
char getchar(void) {
  char data;
  while (rx_counter0==0);
  data=rx_buffer0[rx_rd_index0];
  if (++rx_rd_index0 == RX_BUFFER_SIZE0) rx_rd_index0=0;
  #asm("cli")
  --rx_counter0;
  #asm("sei")
  return data;
}
#pragma used-
#endif
 
// USART0 Transmitter buffer
#define TX_BUFFER_SIZE0 40
char tx_buffer0[TX_BUFFER_SIZE0];
 
#if TX_BUFFER_SIZE0<256
unsigned char tx_wr_index0,tx_rd_index0,tx_counter0;
#else
unsigned int tx_wr_index0,tx_rd_index0,tx_counter0;
#endif
 
// USART0 Transmitter interrupt service routine
interrupt [USART0_TXC] void usart0_tx_isr(void) {
  if (tx_counter0) {
    --tx_counter0;
    UDR0=tx_buffer0[tx_rd_index0];
    if (++tx_rd_index0 == TX_BUFFER_SIZE0) tx_rd_index0=0;
  };
}
 
#ifndef _DEBUG_TERMINAL_IO_
// Write a character to the USART0 Transmitter buffer
#define _ALTERNATE_PUTCHAR_
#pragma used+
void putchar(char c) {
  while (tx_counter0 == TX_BUFFER_SIZE0);
  #asm("cli")
  if (tx_counter0 || ((UCSR0A & DATA_REGISTER_EMPTY)==0)) {
    tx_buffer0[tx_wr_index0]=c;
    if (++tx_wr_index0 == TX_BUFFER_SIZE0) tx_wr_index0=0;
    ++tx_counter0;
  }
  else
    UDR0=c;
  #asm("sei")
}
#pragma used-
#endif
 
// USART1 Receiver buffer
#define RX_BUFFER_SIZE1 40
char rx_buffer1[RX_BUFFER_SIZE1];
 
#if RX_BUFFER_SIZE1<256
unsigned char rx_wr_index1,rx_rd_index1,rx_counter1;
#else
unsigned int rx_wr_index1,rx_rd_index1,rx_counter1;
#endif
 
// This flag is set on USART1 Receiver buffer overflow
bit rx_buffer_overflow1;
 
// USART1 Receiver interrupt service routine
interrupt [USART1_RXC] void usart1_rx_isr(void) {
  char status,data;
  status=UCSR1A;
  data=UDR1;
  if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0) {
    rx_buffer1[rx_wr_index1]=data;
    if (++rx_wr_index1 == RX_BUFFER_SIZE1) rx_wr_index1=0;
      if (++rx_counter1 == RX_BUFFER_SIZE1) {
        rx_counter1=0;
        rx_buffer_overflow1=1;
      };
  };
}
 
// Get a character from the USART1 Receiver buffer
#pragma used+
char getchar1(void) {
  char data;
  while (rx_counter1==0);
  data=rx_buffer1[rx_rd_index1];
  if (++rx_rd_index1 == RX_BUFFER_SIZE1) rx_rd_index1=0;
  #asm("cli")
  --rx_counter1;
  #asm("sei")
  return data;
}
#pragma used-
 
typedef unsigned char byte;
 
// Function used to define user characters
void define_char(byte flash *pc,byte char_code) {
  byte i,a;
  a=(char_code<<3) | 0x40;
  for (i=0; i<8; i++) lcd_write_byte(a++,*pc++);
}
 
// USART1 Transmitter buffer
#define TX_BUFFER_SIZE1 40
char tx_buffer1[TX_BUFFER_SIZE1];
 
#if TX_BUFFER_SIZE1<256
unsigned char tx_wr_index1,tx_rd_index1,tx_counter1;
#else
unsigned int tx_wr_index1,tx_rd_index1,tx_counter1;
#endif
 
// USART1 Transmitter interrupt service routine
interrupt [USART1_TXC] void usart1_tx_isr(void) {
  if (tx_counter1) {
    --tx_counter1;
    UDR1=tx_buffer1[tx_rd_index1];
    if (++tx_rd_index1 == TX_BUFFER_SIZE1) tx_rd_index1=0;
  };
}
 
// Write a character to the USART1 Transmitter buffer
#pragma used+
void putchar1(char c) {
while (tx_counter1 == TX_BUFFER_SIZE1);
  #asm("cli")
  if (tx_counter1 || ((UCSR1A & DATA_REGISTER_EMPTY)==0)) {
    tx_buffer1[tx_wr_index1]=c;
    if (++tx_wr_index1 == TX_BUFFER_SIZE1)
      tx_wr_index1=0;
    ++tx_counter1;
  }
  else
    UDR1=c;
  #asm("sei")
}
#pragma used-
 
// Standard Input/Output functions
#include <stdio.h>
 
#include <stdlib.h>
 
//------------------------------------------ GLOBAALIT -----------------------------------------
 
#define LEVEYS 6
#define KORKEUS 16
#define KIERROSVIIVE 50
#define NAPPAIMISTOVIIVE 150
 
/**
 * Tilakoneen tilat, pelin vuo
 */
enum tilat {
  TILA_ALOITUS = 0,
  TILA_PELI = 1,
  TILA_PELIPAATTYNYT = 2,
  TILA_PAUSSI = 3
};
 
/**
 * Puskuri
 * Puskuri on KORKEUS x LEVEYS -kokoinen kenttä.
 * Puskuri-taulukon yksi char vastaa yhtä x-riviä, bitit vastaavat yhtä kentän solua.
 */
unsigned char puskuri[KORKEUS] = { 0 }; // näyttöpuskuri bitteinä
/**
 * Kenttä sisältää pelikentän bitteinä biteissä 1..6
 * Bitit 0 ja 7 ovat aina 1 reunan törmäystarkistuksen vuoksi
 */
unsigned char kentta[KORKEUS] = { 0 };
 
// Tetris-palikka
signed char palikkaY = -1;  // palikan yläreunan sijainti, 0-rivi on kentän yläreuna
unsigned char palikkaX = 4; // palikan pivotin etäisyys oikeasta reunasta + 1
byte rotaatio = 0;          // nykyisen palikan rotaatio
byte rotaatioMax = 3;       // nykyisen palikan mahdolliset rotaatiot
byte muoto = 0;             // nykyisen palikan perusmuoto
 
byte palikka[5] = { 0 };    // palikan rivit 0 - 3
byte palikkaTmp[5] = { 0 }; // käytetään apuna palikan rotaatiossa
 
// apumuuttujia
byte x = 0; // loop-muuttuja x
byte y = 0; // loop-muuttuja y
 
// Viive palikan tiputtamiselle - pelin vaikeutuessa tätä pienennetään
signed int tiputusViive = 1000;
signed int alkuperViive = 1000;
 
// Tilakoneen tila
char tila = 0;
 
unsigned int palikkaLaskuri = 0;  // montako palikkaa kentällä on ollut
unsigned int riviLaskuri = 0;     // montako riviä on räjähtänyt
unsigned int pisteet = 0;         // pisteesi
byte level = 1;     // level, eli mitä korkeampi, sen nopeammin palikat tippuvat - mutta saat enemmän pisteitä
byte levelMax = 10; // maksimi-level
 
int tmpViive = 0; // apu-viive-muuttuja
 
/**
 * Piirtomerkit
 *
 * 0  1  2  3  4  5  6  7
 *
 * -  #  -  #  -  #  -  #
 * -  -  #  #  -  -  #  #
 * -  -  -  -  #  #  #  #
 *
 */
/*
flash byte char0[8] = { 0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000 };
flash byte char1[8] = { 0b1111111,0b1111111,0b1111111,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000 };
flash byte char2[8] = { 0b0000000,0b0000000,0b0000000,0b1111111,0b1111111,0b1111111,0b0000000,0b0000000 };
flash byte char3[8] = { 0b1111111,0b1111111,0b1111111,0b1111111,0b1111111,0b1111111,0b0000000,0b0000000 };
flash byte char4[8] = { 0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b1111111,0b1111111 };
flash byte char5[8] = { 0b1111111,0b1111111,0b1111111,0b0000000,0b0000000,0b0000000,0b1111111,0b1111111 };
flash byte char6[8] = { 0b0000000,0b0000000,0b0000000,0b1111111,0b1111111,0b1111111,0b1111111,0b1111111 };
flash byte char7[8] = { 0b1111111,0b1111111,0b1111111,0b1111111,0b1111111,0b1111111,0b1111111,0b1111111 };
*/
flash byte char0[8] = { 0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000 };
flash byte char1[8] = { 0b1111111,0b1111111,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000 };
flash byte char2[8] = { 0b0000000,0b0000000,0b0000000,0b1111111,0b1111111,0b0000000,0b0000000,0b0000000 };
flash byte char3[8] = { 0b1111111,0b1111111,0b0000000,0b1111111,0b1111111,0b0000000,0b0000000,0b0000000 };
flash byte char4[8] = { 0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b0000000,0b1111111,0b1111111 };
flash byte char5[8] = { 0b1111111,0b1111111,0b0000000,0b0000000,0b0000000,0b0000000,0b1111111,0b1111111 };
flash byte char6[8] = { 0b0000000,0b0000000,0b0000000,0b1111111,0b1111111,0b0000000,0b1111111,0b1111111 };
flash byte char7[8] = { 0b1111111,0b1111111,0b0000000,0b1111111,0b1111111,0b0000000,0b1111111,0b1111111 };
 
// palikoiden muodot, 4 bittiä per rivi
flash byte muotoI = 0b00001111;
flash byte muotoO = 0b01100110;
flash byte muotoT = 0b01001110;
flash byte muotoS = 0b01101100;
flash byte muotoZ = 0b11000110;
flash byte muotoJ = 0b10001110;
flash byte muotoL = 0b00101110;
 
// ----------------------------------- FUNCTION DECLARATIONS ----------------------------------- 
 
 
void asetaPikseli(char *taulukko, const byte x, const byte y, const byte arvo);
byte haePikseli(char* taulukko, const byte x, const byte y);
void piirraPuskuriLcd(void);
void rakennaPuskuri(void);
void alustaPeli(void);
void alustaPalikka(void);
byte siirraPalikkaaVasemmalle(void);
byte siirraPalikkaaOikealle(void);
byte siirraPalikkaaAlas(void);
byte siirraPalikkaaYlos(void);
byte testaaPalikkaaKenttaan(char *testiPalikka, signed char testiY);
byte kaannaPalikkaaMP(void);
byte kaannaPalikkaaVP(void);
void poltaPalikkaKenttaan(void);
char rakennaRotatoituPalikka(char *taulukko, const char offsetX, const char rot);
void poistaTaydetRivit(void);
 
 
// ----------------------------------- MAIN -----------------------------------
 
 
void main(void) {
  //signed char oma='/' ; //oma lähetettävä merkki
 
  // alaspäin-laskuri palikan tiputtamisen ja näppäimistökuuntelun ajastukselle
  signed int tiputusViiveLaskuri = 0;
  signed int nappaimistoViiveLaskuri = NAPPAIMISTOVIIVE;
 
  char aliTila = 0;
  signed char tmpSig = 0; // käytetään sielä jossain
  signed char tmpSjg = 0;
 
  // Käytetään ritariässässä
  bit lediSuunta = 0;
  char ledi = 0;
  int lediViive = 0;
  char nayttorivi1[8] = { 0 };
  char nayttorivi2[8] = { 0 };
  nayttorivi1[8] = '\0'; // tarvitaanko tämmöstä? emt oon ihan huono c:ssä =)
  nayttorivi2[8] = '\0';
 
  //bit nappaintaPainettu = 0;
 
 
  // Input/Output Ports initialization
  // Port A initialization
  // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
  // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
  PORTA=0x00;
  DDRA=0x00;
 
  // Port B initialization
  // Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
  // State7=0 State6=1 State5=0 State4=1 State3=0 State2=1 State1=0 State0=1 
  PORTB=0x55;
  DDRB=0xFF;
 
  // Port C initialization
  // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
  // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
  PORTC=0x00;
  DDRC=0x00;
 
  // Port D initialization
  // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
  // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
  PORTD=0x00;
  DDRD=0x00;
 
  // Port E initialization
  // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
  // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
  PORTE=0x00;
  DDRE=0x00;
 
  // Port F initialization
  // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
  // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
  PORTF=0x00;
  DDRF=0x00;
 
  // Port G initialization
  // Func4=In Func3=In Func2=In Func1=In Func0=In 
  // State4=T State3=T State2=T State1=T State0=T 
  PORTG=0x00;
  DDRG=0x00;
 
  // Timer/Counter 0 initialization
  // Clock source: System Clock
  // Clock value: Timer 0 Stopped
  // Mode: Normal top=FFh
  // OC0 output: Disconnected
  ASSR=0x00;
  TCCR0=0x00;
  TCNT0=0x00;
  OCR0=0x00;
 
  // Timer/Counter 1 initialization
  // Clock source: System Clock
  // Clock value: Timer 1 Stopped
  // Mode: Normal top=FFFFh
  // OC1A output: Discon.
  // OC1B output: Discon.
  // OC1C output: Discon.
  // Noise Canceler: Off
  // Input Capture on Falling Edge
  TCCR1A=0x00;
  TCCR1B=0x00;
  TCNT1H=0x00;
  TCNT1L=0x00;
  ICR1H=0x00;
  ICR1L=0x00;
  OCR1AH=0x00;
  OCR1AL=0x00;
  OCR1BH=0x00;
  OCR1BL=0x00;
  OCR1CH=0x00;
  OCR1CL=0x00;
 
  // Timer/Counter 2 initialization
  // Clock source: System Clock
  // Clock value: Timer 2 Stopped
  // Mode: Normal top=FFh
  // OC2 output: Disconnected
  TCCR2=0x00;
  TCNT2=0x00;
  OCR2=0x00;
 
  // Timer/Counter 3 initialization
  // Clock source: System Clock
  // Clock value: Timer 3 Stopped
  // Mode: Normal top=FFFFh
  // Noise Canceler: Off
  // Input Capture on Falling Edge
  // OC3A output: Discon.
  // OC3B output: Discon.
  // OC3C output: Discon.
  TCCR3A=0x00;
  TCCR3B=0x00;
  TCNT3H=0x00;
  TCNT3L=0x00;
  ICR3H=0x00;
  ICR3L=0x00;
  OCR3AH=0x00;
  OCR3AL=0x00;
  OCR3BH=0x00;
  OCR3BL=0x00;
  OCR3CH=0x00;
  OCR3CL=0x00;
 
  // External Interrupt(s) initialization
  // INT0: Off
  // INT1: Off
  // INT2: Off
  // INT3: Off
  // INT4: Off
  // INT5: Off
  // INT6: Off
  // INT7: Off
  EICRA=0x00;
  EICRB=0x00;
  EIMSK=0x00;
 
  // Timer(s)/Counter(s) Interrupt(s) initialization
  TIMSK=0x00;
  ETIMSK=0x00;
 
  // USART0 initialization
  // Communication Parameters: 8 Data, 1 Stop, No Parity
  // USART0 Receiver: On
  // USART0 Transmitter: On
  // USART0 Mode: Asynchronous
  // USART0 Baud rate: 9600
  UCSR0A=0x00;
  UCSR0B=0xD8;
  UCSR0C=0x06;
  UBRR0H=0x00;
  UBRR0L=0x19;
 
  // USART1 initialization
  // Communication Parameters: 8 Data, 1 Stop, No Parity
  // USART1 Receiver: On
  // USART1 Transmitter: On
  // USART1 Mode: Asynchronous
  // USART1 Baud rate: 9600
  UCSR1A=0x00;
  UCSR1B=0xD8;
  UCSR1C=0x06;
  UBRR1H=0x00;
  UBRR1L=0x19;
 
  // Analog Comparator initialization
  // Analog Comparator: Off
  // Analog Comparator Input Capture by Timer/Counter 1: Off
  ACSR=0x80;
  SFIOR=0x00;
 
  // LCD module initialization
  lcd_init(16);
 
  // Asetetaan omat piirtomerkit
  define_char(char0, 0);
  define_char(char1, 1);
  define_char(char2, 2);
  define_char(char3, 3);
  define_char(char4, 4);
  define_char(char5, 5);
  define_char(char6, 6);
  define_char(char7, 7);
 
  // Global enable interrupts
  #asm("sei")
 
  lcd_clear();
  putsf("ZemmTrix v1");
  //delay_ms(500);           
  PORTB=0xFF;
 
  // ------------------------------------- LOOPPI -------------------------------------
 
  //alustaKentta(); // alusta uusi tyhjä kenttä
  alustaPalikka(); // alusta uusi satunnainen palikka
  tiputusViiveLaskuri = tiputusViive;
  tila = TILA_ALOITUS;
 
  while(1) {
    // luetaan mahdollinen sarjaliitännän yli tullut merkki
    //oma = 0; if (rx_counter0 != 0) { oma = getchar(); }
 
    switch(tila) {
 
      // ============================== ALKUTILA ==============================
 
      case TILA_ALOITUS:
        //delay_ms(1000);
        aliTila = 1;
        tmpViive = 0;
        while(PINA.0 != 0) {
          delay_ms(20);
          lediViive += 20;
          tmpViive += 20;
          if(tmpViive > 1000) {
            aliTila = (aliTila) ? 0 : 1;
            tmpViive = 0;
          }
          // lcd help
          if(aliTila) {
            lcd_clear();
            lcd_gotoxy(0,0);
            lcd_putsf("* ZemmTrix  v1 *underkround.net ");
          } else {
            lcd_clear();
            lcd_gotoxy(0,0);
            lcd_putsf("Paina klunssia 0  palataksesi   ");
          }
          // Ritariässä
          if(lediViive > 100) { 
            lediViive = 0;
            if(lediSuunta == 0) {
              ++ledi;
              if(ledi == 7)
                lediSuunta = 1;
            }
            else {
              --ledi;
              if(ledi == 0)
                lediSuunta = 0;
            }
            PORTB = 1 << ledi;
          }
        }
        //Nollataan ledit
        PORTB = 0xFF;
 
        // Le' efekti
        for(palikkaY=-7; palikkaY<KORKEUS+7; palikkaY++) {
          for(x=0; x<6; x++) {
            tmpSig = palikkaY-(x/2);
            if(tmpSig >= 0)
              asetaPikseli(puskuri, x, (byte)tmpSig, 1);
            --tmpSig;
            --tmpSig;
            if(tmpSig >= 0)
              asetaPikseli(puskuri, x, (byte)tmpSig, 0);
          }
          piirraPuskuriLcd();
          delay_ms(50);
        }
 
        // alusta uusi peli
        alustaPeli();
        tmpViive = 0;
        aliTila = 0;
        tila = TILA_PELI;
        //delay_ms(500); // poistetaan vahinkotiputus alusta kun nappi 0 on pohjassa..
        break;
 
      // ============================== PELITILA ==============================
 
      case TILA_PAUSSI:
        lediSuunta = 1;
        aliTila = 1;
        while(aliTila) {
          // Le' efekti
          for(tmpSjg=-7; tmpSjg<KORKEUS+7; tmpSjg++) {
            for(x=0; x<6; x++) {
              if(lediSuunta) tmpSig = tmpSjg-(x/2);
              else tmpSig = (KORKEUS)-tmpSjg - (x/2);
 
              if(tmpSig >= 0 && tmpSig < KORKEUS)
                asetaPikseli(puskuri, x, (byte)tmpSig, 1);
 
              if(lediSuunta) { --tmpSig; --tmpSig; }
              else { ++tmpSig; ++tmpSig; }
 
              if(tmpSig >= 0 && tmpSig < KORKEUS)
                asetaPikseli(puskuri, x, (byte)tmpSig, 0);
 
              PORTB = 1 << (tmpSig / 2);
            }
            piirraPuskuriLcd();
            delay_ms(50);
            if(PINA.6 == 0)
              aliTila = 0;
          }
          lediSuunta = (lediSuunta) ? 0 : 1;
        }
        PORTB = 0xFF;
        tila = TILA_PELI;
        break;
 
      // ============================== PELITILA ==============================
 
      case TILA_PELI:
 
        // ----- begin näppäimistön kuuntelu
 
        // vähennä näppäimistöviivettä, jos se on asetettu
        // näppäimistöviive estää saman napinpainalluksen rekisteröitymisen
        // nopean päivityksen vuoksi useana painalluksena
        if(nappaimistoViiveLaskuri <= 0) {
          if(PINA.7 == 0) {
            PINA.7 = 1;
            tila = TILA_PAUSSI;
          }
          if(PINA.5 == 0) {
            siirraPalikkaaAlas();
          }
          // 4 - Kääntää palikkaa myötäpäivään
          if(PINA.4 == 0) {
            kaannaPalikkaaMP();
          }
          // 3 - siirtää palikkaa vasemmalle
          if(PINA.3 == 0) {
            siirraPalikkaaVasemmalle();
          }
          // 2 - Siirtää palikkaa oikealle
          if(PINA.2 == 0) {
            siirraPalikkaaOikealle();
          }
          // 1 - Kääntää palikkaa vastapäivään
          if(PINA.1 == 0) {
            kaannaPalikkaaVP();
          }
          // 0 - Tiputtaa palikan alas
          if(PINA.0 == 0) {
            while(siirraPalikkaaAlas());
          }
          nappaimistoViiveLaskuri = NAPPAIMISTOVIIVE;
        }
        else {
          nappaimistoViiveLaskuri -= KIERROSVIIVE;
        }
        // ----- end näppäimistön kuuntelu
 
        if(tmpViive > 0) {
          if(tmpViive <= 100)      PORTB = 0xFF; // off
          else if(tmpViive <= 250) PORTB = 0xDB; // on
          else if(tmpViive <= 400) PORTB = 0xFF; // off
          else if(tmpViive <= 550) PORTB = 0xAA; // on
          else if(tmpViive <= 700) PORTB = 0xFF; // off
          else if(tmpViive <= 850) PORTB = 0x92; // on
          else if(tmpViive <=1000) PORTB = 0xFF; // off
          else if(tmpViive <=1150) PORTB = 0x00; // on
          tmpViive -= KIERROSVIIVE;
        }
 
        // vähennä tiputuslaskuria
        tiputusViiveLaskuri -= KIERROSVIIVE;
        // jos laskuri on nollassa, tiputa palikkaa
        if(tiputusViiveLaskuri <= 0) {
          if(siirraPalikkaaAlas() == 0) {
            // palikka ei voi mennä enää alaspäin, poltetaan palikka lautaan ja alustetaan uusi
            poltaPalikkaKenttaan();
            poistaTaydetRivit();
            alustaPalikka();
            putsf("\r[uusi palikka kentalle]\r");
          }
          tiputusViiveLaskuri = tiputusViive; // "nollaa" viivelaskuri
          putsf("\r[timeout]\r");
        }
 
        // rakenna puskuri ja piirrä sen sisältö lcd-näytölle
        rakennaPuskuri();
        piirraPuskuriLcd();
        delay_ms(KIERROSVIIVE);
        break;
 
      // ============================== GAME OVER ==============================
 
      case TILA_PELIPAATTYNYT:
        aliTila = 1;
        tmpViive = 0;
        while(PINA.0 != 0) {
          // Ritariässä
          delay_ms(20);
          lediViive += 20;
          tmpViive += 20;
          if(tmpViive > 1000) {
            aliTila = (aliTila) ? 0 : 1;
            tmpViive = 0;
          }
          // lcd nfo
          if(aliTila) {
            lcd_clear();
            itoa(pisteet, nayttorivi1);
            nayttorivi1[7] = 'p';
            itoa(level, nayttorivi2);
            nayttorivi2[7] = 'l';
            lcd_puts(nayttorivi1);
            lcd_gotoxy(0,0);
            lcd_puts(nayttorivi1);
            lcd_gotoxy(0,1);
            lcd_puts(nayttorivi2);
          } else {
            lcd_clear();
            lcd_gotoxy(0,0);
            lcd_putsf(" by: zemm@iio6s underkround.net ");
          }
 
          if(lediViive > 100) { 
            lediViive = 0;
            if(lediSuunta == 0) {
              ++ledi;
              if(ledi == 7)
                lediSuunta = 1;
            }
            else {
              --ledi;
              if(ledi == 0)
                lediSuunta = 0;
            }
            PORTB = 1 << ledi;
          }
        }
        //Nollataan ledit
        PORTB = 0xFF;
        // alusta uusi peli
        alustaPeli();
        tmpViive = 0;
        aliTila = 0;
        tila = TILA_PELI;
        delay_ms(500); // poistetaan vahinkotiputus alusta kun nappi 0 on pohjassa..
      break;
 
    }; // end switch(tila)
 
  }; // end while(1)
 
} // end main
 
 
// ----------------------------------- FUNCTION -----------------------------------
 
 
/**
 * Aseta pikselille arvo (0 tai 1)
 */
void asetaPikseli(char *taulukko, const byte x, const byte y, const byte arvo) {
  if(x >= LEVEYS || y >= KORKEUS)
    return; // out of range
  taulukko[y] = (arvo != 0) ? taulukko[y] | (1 << x) : taulukko[y] ^ (taulukko[y] & (1 << x));
}
 
/**
 * Hae pikselin arvo (0 tai 1)
 */
byte haePikseli(char* taulukko, const byte x, const byte y) {
  if(x >= LEVEYS || y >= KORKEUS)
    return 0;
  return taulukko[y] & (1 << x);
}
 
/**
 * Piirrä puskuri lcd-näytölle
 */
void piirraPuskuriLcd() {
  // first line
  lcd_gotoxy(0,0);
  for(y=0; y<KORKEUS; y++) {
    x = (puskuri[y] & 0b111);
    lcd_putchar(x);
  }
  // second line
  lcd_gotoxy(0,1);
  for(y=0; y<KORKEUS; y++) {
    x = (puskuri[y] & 0b111000) >> 3;
    lcd_putchar(x);
  }
}
 
/**
 * Päivittää puskuri-taulun muodostaen sen palikasta ja kentästä yhdistettynä.
 */
void rakennaPuskuri() {
  // tyhjennä puskuri
  for(y=0; y<KORKEUS; y++)
    puskuri[y] = 0;
  // aseta palikka puskuriin
  for(y=0; y<5; y++)
    if((y+palikkaY) >= 0 && (y+palikkaY) < KORKEUS)
      puskuri[y+palikkaY] |= (palikka[y] >> 1);
  // kopioi kenttä puskuriin
  for(y=0; y<KORKEUS; y++)
    puskuri[y] |= ((0b01111110 & kentta[y]) >> 1); // jätetään pois kentän 0 ja 8 bitit, jotka ovat aina 1
}
 
// ---------------------------------------------------------------------------
 
/**
 * Alusta pelilauta tyhjäksi, reunoihin laitetaan 1 bitti jonka avulla
 * tarkistetaan meneekö palikka reunasta yli
 */
void alustaPeli() {
  pisteet = 0;
  riviLaskuri = 0;
  palikkaLaskuri = 0;
  for(y=0; y<KORKEUS; y++)
    kentta[y] = 0b10000001;
}
 
 
/**
 * Alusta uusi satunnainen palikka
 */
void alustaPalikka() {
  unsigned int uusiMuoto;
  byte levelTmp;
  pisteet += level; // jokaisesta palikasta laudalla tulee levelin verran pisteitä
  palikkaLaskuri++;
  levelTmp = (int)(palikkaLaskuri / 20) + 1;
  if(level <= levelMax && level < levelTmp) {
    level = levelTmp;
    tiputusViive -= 80; // alkuperViive - (level*80);
  }
  // luo uusi palikka
  uusiMuoto = rand() % 6;
  palikkaX = 4;
  palikkaY = -2;
  rotaatio = 0;
  switch(uusiMuoto) {
    case 0: muoto = muotoI; rotaatioMax = 1; break;
    case 1: muoto = muotoO; rotaatioMax = 0; break;
    case 2: muoto = muotoT; rotaatioMax = 3; break;
    case 3: muoto = muotoS; rotaatioMax = 1; break;
    case 4: muoto = muotoZ; rotaatioMax = 1; break;
    case 5: muoto = muotoJ; rotaatioMax = 3; break;
    case 6: muoto = muotoT; rotaatioMax = 3; break;
    default: muoto = muotoT; rotaatioMax = 3; break;
  }
  rakennaRotatoituPalikka(palikka, palikkaX, rotaatio);
  if(testaaPalikkaaKenttaan(palikka, palikkaY) != 1) {
    tila = TILA_PELIPAATTYNYT; // jos uusi palikka ei mahdu laudalle, peli on ohi
  }
}
 
 
/**
 * Kääntää palikkaa myötäpäivään, mikäli mahdollista
 */
byte kaannaPalikkaaMP() {
  byte rotaatioTmp;
  if(rotaatio < rotaatioMax)
    rotaatioTmp = rotaatio + 1;
  else
    rotaatioTmp = 0;
 
  // testataan mahtuuko käännetty palikka laudalle
  rakennaRotatoituPalikka(palikkaTmp, palikkaX, rotaatioTmp);
  if(testaaPalikkaaKenttaan(palikkaTmp, palikkaY) == 0)
    return 0; // palikka ei mahdu käännetyssä muodossaan kentälle
 
  // käännetty palikka mahtuu laudalle, otetaan käännetty palikka käyttöön
  rotaatio = rotaatioTmp;
  for(y=0; y<5; y++)
    palikka[y] = palikkaTmp[y];
  return 1;
}
 
 
/**
 * Kääntää palikkaa vastapäivään
 */
byte kaannaPalikkaaVP() {
  byte rotaatioTmp;
  if(rotaatio > 0)
    rotaatioTmp = rotaatio - 1;
  else
    rotaatioTmp = rotaatioMax;
 
  // testataan mahtuuko käännetty palikka laudalle
  rakennaRotatoituPalikka(palikkaTmp, palikkaX, rotaatioTmp);
  if(testaaPalikkaaKenttaan(palikkaTmp, palikkaY) == 0)
    return 0; // palikka ei mahdu käännetyssä muodossaan kentälle
 
  // käännetty palikka mahtuu laudalle, otetaan käännetty palikka käyttöön
  rotaatio = rotaatioTmp;
  for(y=0; y<5; y++)
    palikka[y] = palikkaTmp[y];
  return 1;
}
 
 
/*
 * 0100
 * 1110
 *  ^
 *  |
 * OffsetX tarkoittaa palikan 2-bitin etäisyyttä oikeasta reunasta.
 *
 * Muodostettu rotaatio sijoitetaan annettuun taulukkoon.
 */
char rakennaRotatoituPalikka(char *taulukko, const char offsetX, const char rot) {
  signed char shiftX;
  shiftX = offsetX - 2;
  if(shiftX < 0)
    return 0; // ei voida shiftata negatiivistä määrää
  // käydään muoto-tavun bitit läpi, ja asetetaan ne taulukkoon oikeassa järjestyksessä
  // muoto 01001110 tarkoittaa:
  //  0100
  //  1110  jossa keskipiste on bitti 2
  switch(rot) {
    case 0: // perusrotaatio
      taulukko[0] = 0;
      taulukko[1] = (  ((muoto & 0b11110000) >> 4)  << shiftX);
      taulukko[2] = (  ( muoto & 0b00001111      )  << shiftX);
      taulukko[3] = 0;
      taulukko[4] = 0;
      break;
 
    case 1:
      taulukko[0] = 0;
      taulukko[1] = ( (    ( (muoto&0b00001000) >> 1 ) | ((muoto&0b10000000) >> 6 )    ) << shiftX);
      taulukko[2] = ( (    ( (muoto&0b00000100) >> 0 ) | ((muoto&0b01000000) >> 5 )    ) << shiftX);
      taulukko[3] = ( (    ( (muoto&0b00000010) << 1 ) | ((muoto&0b00100000) >> 4 )    ) << shiftX);
      taulukko[4] = ( (    ( (muoto&0b00000001) << 2 ) | ((muoto&0b00010000) >> 3 )    ) << shiftX);
      break;
 
    case 2:
      taulukko[0] = 0;
      taulukko[1] = 0;
      taulukko[2] = ( (    ( (muoto&0b00000001) << 4 ) | ((muoto&0b00000010) << 2 ) | ((muoto&0b00000100) << 0 ) | ((muoto&0b00001000) >> 2 )    ) << shiftX);
      taulukko[3] = ( (    ( (muoto&0b00010000) << 0 ) | ((muoto&0b00100000) >> 2 ) | ((muoto&0b01000000) >> 4 ) | ((muoto&0b10000000) >> 6 )    ) << shiftX);
      taulukko[4] = 0;
      break;
 
    case 3:
      taulukko[0] = ( (    ( (muoto&0b00010000) >> 1 ) | ((muoto&0b00000001) << 2 )    ) << shiftX);
      taulukko[1] = ( (    ( (muoto&0b00100000) >> 2 ) | ((muoto&0b00000010) << 1 )    ) << shiftX);
      taulukko[2] = ( (    ( (muoto&0b01000000) >> 3 ) | ((muoto&0b00000100) << 0 )    ) << shiftX);
      taulukko[3] = ( (    ( (muoto&0b10000000) >> 4 ) | ((muoto&0b00001000) >> 1 )    ) << shiftX);
      taulukko[4] = 0;
      break;
 
    default:
      return 0; // ei onnistunut (virheellinen rotaatio)
      break;
  }
  return 1; // onnistui
}
 
 
/**
 * Poistaa täydet rivit kentältä, päivittää rivilaskuria ja mahdollisesti
 * leveliä sekä viivettä.
 */
void poistaTaydetRivit() {
  byte poistetutRivit = 0;
  //for(y=KORKEUS-1; y>=0; y--) {
  for(y=0; y<KORKEUS; y++) {
    // jos rivi on täysi, poista se ja tiputa muita
    if((kentta[y] & 0b01111110) == 0b01111110) {
      ++poistetutRivit;
      // siirrä ylempiä rivejä alaspäin
      for(x=y; x>0; x--) {
        kentta[x] = kentta[x-1];
      }
      kentta[0] = 0b10000001; // nollaa ylin rivi
    }
  }
  if(poistetutRivit > 0) {
    riviLaskuri += poistetutRivit;
    switch(poistetutRivit) {
      case 1: pisteet += 10 * level; tmpViive = 250; break;
      case 2: pisteet += 30 * level; tmpViive = 550; break;
      case 3: pisteet += 60 * level; tmpViive = 850; break;
      case 4: pisteet += 100* level; tmpViive =1150; break;
      default: break;
    }
  }
}
 
 
// ---------------------------------------------------------------------------
 
 
/**
 * Siirrä palikkaa yhdellä vasemmalle, mikäli sallittua
 * @return 1, jos siirto onnistui, muuten 0
 */
byte siirraPalikkaaVasemmalle() {
  // luodaan testipalikka joka on yhden vasemmalle oikeasta palikasta
  for(y=0; y<5; y++)
    palikkaTmp[y] = palikka[y] << 1;
  // testataan sopiiko palikka
  if(testaaPalikkaaKenttaan(palikkaTmp, palikkaY) == 0)
    return 0; // palikka ei sopinut
  // palikka sopi, joten siirretään oikeaa palikkaa vasemmalle
  for(y=0; y<5; y++)
    palikka[y] = palikka[y] << 1;
  palikkaX++;
  return 1;
}
 
 
/**
 * Siirrä palikkaa yhdellä oikealle, mikäli sallittua
 * @return 1, jos siirto onnistui, muuten 0
 */
byte siirraPalikkaaOikealle() {
  // luodaan testipalikka joka on yhden oikealla oikeasta palikasta
  for(y=0; y<5; y++)
    palikkaTmp[y] = palikka[y] >> 1;
  // testataan sopiiko palikka
  if(testaaPalikkaaKenttaan(palikkaTmp, palikkaY) == 0)
    return 0; // palikka ei sopinut
  // palikka sopi, joten siirretään oikeaa palikkaa oikealle
  for(y=0; y<5; y++)
    palikka[y] = palikka[y] >> 1;
  palikkaX--;
  return 1;
}
 
 
/**
 * Siirrä palikkaa yhdellä alaspäin, mikäli sallittua
 * @return 1, jos siirto onnistui, muuten 0
 */
byte siirraPalikkaaAlas() {
  // palikka sopii, joten siirretään palikkaa yhdellä alaspäin
  palikkaY++;
  if(testaaPalikkaaKenttaan(palikka, palikkaY) == 1)
    return 1;
  palikkaY--; // palikka ei sopinut
  return 0;
}
 
 
/**
 * Siirrä palikkaa yhdellä ylöspäin. Tämä on aina sallittua, koska kentällä ei
 * ole kattoa. Tätä ei myöskään tarvita varsinaisessa pelissä, tämä on vain
 * testaukseen
 * @return 1, jos siirto onnistui, eli aina
 */
/*
byte siirraPalikkaaYlos() {
  palikkaY--;
  return 1;
}*/
 
 
/**
 * Testataa sopiiko annettu palikka annetulla korkeudella pelilautaan,
 * vai tuleeko päällekäisyys.
 */
byte testaaPalikkaaKenttaan(char *testiPalikka, signed char testiY) {
  // testataan osuuko palikka mihinkään kentässä
  signed char j;
  for(j=0; j<5; j++) {
    if(palikka[j] != 0) { // ei tarkisteta palikan tyhjiä rivejä
      if((j+testiY) >= KORKEUS)
        return 0;
      if((j+testiY) >= 0) {
        if((kentta[j+testiY] & testiPalikka[j]) != 0)
          return 0;
      }
    }
  }
  return 1;
}
 
 
/**
 * "Poltetaan" palikka pelilautaan, eli asetetaan palikan 1-bitit pelilaudalla
 * päälle.
 */
void poltaPalikkaKenttaan() {
  signed char j;
  y = 0;
  for(j=palikkaY; j<palikkaY+5; j++) {
    if((j >= 0) && (j < KORKEUS)) {
      kentta[j] |= palikka[y];
    }
    y++;
  }
}