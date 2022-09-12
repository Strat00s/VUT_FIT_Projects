/************************************************************************************/
/*                                                                                  */
/*  Laboratorni uloha c. 1 z predmetu IMP                                           */
/*                                                                                  */
/*  Komunikace pres UART                                                            */
/*                                                                                  */
/*  Reseni vytvoril a odevzdava: (Lukáš Baštýř, xbasty00)                           */
/*                                                                                  */
/************************************************************************************/


#include "MKL05Z4.h"

int n=0;                                // index
unsigned char c;                        // prijaty znak
unsigned char prompt[7] = "Login>";     // text vyzvy
unsigned char corrl[9] = "xbasty00";    // Vas login
unsigned char login[9];                 // misto pro prijaty retezec

/* Inicializace MCU - zakladni nastaveni hodin, vypnuti watchdogu */
void MCUInit(void)  {
    MCG->C4 |= ( MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01) );
    SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
    SIM->COPC = SIM_COPC_COPT(0x00);	// watchdog vypnut
}

/* Inicializace pinu pro vysilani a prijem pres UART - RX a TX */
void PinInit(void)  {
    SIM->SOPT2 |= SIM_SOPT2_UART0SRC(0x01);
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;     // Zapnout hodiny pro PORTA a PORTB
    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;

    PORTB->PCR[1] = ( 0 | PORT_PCR_MUX(0x02) );	// UART0_TX
    PORTB->PCR[2] = ( 0 | PORT_PCR_MUX(0x02) ); // UART0_RX

    PORTB->PCR[13] = ( 0|PORT_PCR_MUX(0x01) );  // Beeper (PTB13)
    PTB->PDDR = GPIO_PDDR_PDD( 0x2000 );		// "1" znamena, ze pin bude vystupni
}

/* Inicializace UART - nastaveni prenosove rychlosti 115200Bd, 8 bitu, bez parity */
void UART0Init(void) {
    UART0->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);

    UART0->BDH = 0x00;
    UART0->BDL = 0x1A;	// Baud rate 115 200 Bd, 1 stop bit
    UART0->C4 = 0x0F;	// Oversampling ratio 16, match address mode disabled

    UART0->C1 = 0x00;	// 8 data bitu, bez parity
    UART0->C3 = 0x00;
    UART0->MA1 = 0x00;	// no match address (mode disabled in C4)
    UART0->MA2 = 0x00;	// no match address (mode disabled in C4)
    UART0->S1 |= 0x1F;
    UART0->S2 |= 0xC0;

    UART0->C2 |= ( UART0_C2_TE_MASK | UART0_C2_RE_MASK );	// Zapnout vysilac i prijimac
}


/* vyslani jednoho znaku (ch) pres UART - funkce vycka, az je vysilaci buffer prazdny, pak posle */
void SendCh(char ch)  {
    while(!(UART0->S1 & UART0_S1_TDRE_MASK) && !(UART0->S1 & UART0_S1_TC_MASK) );
    UART0->D = ch;
}

/* prijeti jednoho znaku pres UART - funkce ceka na prichozi znak a ten vrati jako vysledek */
/* !!!!! FUNKCI DOPLNTE */
//TODO - fix this
char ReceiveCh(void) {
    while(!(UART0->S1 & UART0_S1_RDRF_MASK));
    return UART0->D;
}

/* vysilani retezce ukonceneho 0 */
void SendStr(char *s)  {

    int i = 0;
    while (s[i]!=0)
        {SendCh(s[i++]);}
}

/* funkce zpozdeni - funkce skonci po nastavenem case */
void delay(int bound) {

  int i;
  for(i=0; i<bound; i++);
}

/* Pipnuti pres bzucak na PTB13 - generuje 500 period obdelnikoveho signalu */
void beep(void) {
int q;
    for (q=0; q<500; q++) {
        PTB->PDOR = GPIO_PDOR_PDO(0x2000);
        delay(500);
        PTB->PDOR = GPIO_PDOR_PDO(0x0000);
        delay(500);
    }
}


/*----(My stuff)----*/
#define E5 659  //frequency
#define C5 523
#define G5 784
#define G4 392

long notes[]    =  {E5,    0,  E5,   0,  E5,   0,  C5,   0,  E5,   0,  G5,   0,  G4};
long duration[] =  {200,   0, 200,   0, 200,   0, 150,   0, 200,   0, 200,   0, 250};    //ms
long delays[]    = {100, 200, 100, 400, 100, 400, 100, 200, 100, 500, 100, 800, 100};    //ms

//
void delayUs(long micros) {
    static int cycles = 48 / 10;
    for (long i = 0; i < micros * cycles; i++);
}

//arduino tone function "implementation"
void playTone(long note, long duration) {
    long delays  = 1000000 / note / 2;      //calculate delay in microsecond
    long repeats = note * duration / 1000;  //calculate how many times we should repeat the function in a second

    //toggle buzzer
    for (long i = 0; i < repeats; i++) {
        PTB->PDOR = GPIO_PDOR_PDO(0x2000);
        delayUs(delays);
        PTB->PDOR = GPIO_PDOR_PDO(0x0000);
        delayUs(delays);
    }
}

void playSong() {
    int length = sizeof(notes) / sizeof(notes[0]);
    for (int i = 0; i < length; i++) {
        playTone(notes[i], duration[i]);
        delayUs(delays[i]);
    }
    playTone(0, 1);
}


/* Hlavni funkce */
int main(void) {

    MCUInit();		/* Zakladni inicializace vlastniho MCU - hodiny, watchdog */
    PinInit();		/* Inicializace vstupu a vystupu potrebnych pro tuto ulohu - piny RX a TX UART0 */
    UART0Init();	/* Inicializace modulu UART0 - nastaveni prenosove rychlosti, parametru prenosu */

    char fail_msg[] = " Failed to login. Please try again\n";

    //main loop
    while(1) {
        //read and print chars
    	SendStr(prompt);	// Vyslani vyzvy k zadani loginu
        for(n = 0; n < 8; n++) {
            c = ReceiveCh();    //receive character
            SendCh(c);          //Prijaty znak se hned vysle - echo linky
            login[n] = c;       //Postupne se uklada do pole
        }

        //compare received login with correct one and play song on same
        //option 1 (should work)
        if (!strcmp(login, corrl)) {
            playSong();     //play song
            while(1);    //end in loop
        }
        else {
            SendStr(fail_msg);  //send fail message and repeat
            SendCh('\r');
            SendCh('\n');
        }
    }
}
