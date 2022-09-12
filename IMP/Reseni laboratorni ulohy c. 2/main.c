/******************************************************************************/
/*                                                                            */
/*  Laboratorni uloha c. 2 z predmetu IMP                                     */
/*                                                                            */
/*  Aplikace GPIO pro obsluhu tlacitek s vyuzitim preruseni                   */
/*                                                                            */
/*  Reseni vytvoril(a) a odevzdava: (jmeno, login)                            */
/*                                                                            */
/******************************************************************************/

#include "MKL05Z4.h"

/* -------------------------------------------------------------------------- *
 * (Makro)definice pinu tlacitek, bzucaku a RGB LEDky                         *
 * -------------------------------------------------------------------------- */
#define BUTTON_UP_MASK 0x08
#define BUTTON_CENT_MASK 0x10
#define BUTTON_LEFT_MASK 0x20
#define BUTTON_DOWN_MASK 0x40
#define BUTTON_RIGHT_MASK 0x80
#define PIEZZO_MASK 0x2000
#define LED_R_MASK (1 << 9)
#define LED_G_MASK (1 << 10)
#define LED_B_MASK (1 << 11)

/* -------------------------------------------------------------------------- *
 * Funkce realizujici aktivni cekani                                          *
 * -------------------------------------------------------------------------- */
void delay(uint64_t bound) {
	for (uint64_t i=0; i < bound; i++) { __NOP(); }
}

void delayMs(uint64_t bound) {
	bound = bound * 16 * 100;
	for (uint64_t i=0; i < bound; i++) { __NOP(); }
}

/* -------------------------------------------------------------------------- *
 * Funkce realizujici kratke pipnuti bzucaku                                  *
 * -------------------------------------------------------------------------- */
void beep(void) {
    for (uint32_t q=0; q<200; q++) {
        GPIOB_PDOR |=  PIEZZO_MASK; delay(200);
        GPIOB_PDOR &= ~PIEZZO_MASK; delay(200);
    }
}

void beepMs(uint64_t len) {
    for (uint32_t q=0; q<len; q++) {
        GPIOB_PDOR |=  PIEZZO_MASK; delay(200);
        GPIOB_PDOR &= ~PIEZZO_MASK; delay(200);
    }
}

/* -------------------------------------------------------------------------- *
 * Funkce realizujici kratke bliknuti LED_R                                   *
 * -------------------------------------------------------------------------- */
void flash(int color, int len) {
	switch (color) {
		case 0: GPIOB_PDOR |= LED_R_MASK; delayMs(len);	GPIOB_PDOR &= ~LED_R_MASK; break;
		case 1: GPIOB_PDOR |= LED_G_MASK; delayMs(len); GPIOB_PDOR &= ~LED_G_MASK; break;
		case 2: GPIOB_PDOR |= LED_B_MASK; delayMs(len); GPIOB_PDOR &= ~LED_B_MASK; break;
	}
}

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! *
 * Doplnte nasledujici obsluhu preruseni pro alespon dve ruzna tlacitka tak,  *
 * aby stisk jednoho tlacitka bliknul LEDkou - funkce flash() a stisk jineho  *
 * pipnul piezo-bzucakem - funkce beep(). Zajistete korektni osetreni zakmitu *
 * tlacitek, aby jeden stisk spolehlive generoval pouze jednu prislusnou akci.*
 * Predpokladejte, ze neni stisknuto vice tlacitek soucasne.                  *
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
void PORTB_IRQHandler(void) {
    /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
    /* 1. Vyckejte pomoci funkce delay() za ucelem moznosti filtrace zakmitu.
    /* 2. Doprogramujte vetveni obsluhy preruseni pro vybrana dve tlacitka:
     * - kazdou vetev zahajte testem, zda preruseni bylo vyvolano stiskem daneho
     *   tlacitka (vizte registr ISFR portu B, prip. bit ISF v reg. PCRn portu B),
     * - pokud ano, overte si, zda je v reg. PDIR na prislusne pozici opravdu
     *   ocekavana log. hodnota stisknuteho tlacitka a nejedna se tedy o zakmit,
     * - dale provedte odezvu na stisk tlacitka (volani funkci flash() a beep())
     * - a na konci vynulujte priznak preruseni vyvolaneho stiskem tlacitka
     *   (vizte registry Portu B: PCRn (priznak ISF), pripadne reg. ISFR).
     *
     *   !!!!!!!!! Detaily si sami dohledejte v dokumentaci k MCU. !!!!!!!!!  */

	delay(50000);
	//check for proper button
	int i = 0;
	if (PORTB->ISFR & BUTTON_LEFT_MASK) {
		while (!(GPIOB->PDIR & BUTTON_LEFT_MASK)) {
			flash(i++, 250);
			if (i >= 3)
				i = 0;
		}
		PORTB->PCR[5] = PORTB->PCR[5] | PORT_PCR_ISF(1);
	}
	if (PORTB->ISFR & BUTTON_RIGHT_MASK) {
		int sos[9] = {100, 100, 100, 200, 200, 200, 100, 100, 100};
		while (!(GPIOB->PDIR & BUTTON_RIGHT_MASK)) {
			beepMs(sos[i++] * 3);
			delayMs(100);
			if (i >= 9)
				i = 0;
		}
		PORTB->PCR[7] = PORTB->PCR[7] | PORT_PCR_ISF(1);
	}
}

/* -------------------------------------------------------------------------------- *
 * Inicializace hardware                                                            *
 * -------------------------------------------------------------------------------- */
void init_hardware(void) {
	uint8_t i;

	MCG->C4 |= (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01)); /* Nastav hodinovy podsystem */
	SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
	SIM->COPC = SIM_COPC_COPT(0x00);						/* Vypni WatchDog */

    // Konfigurace portu B, kam jsou pripojena tlacitka a RGB BED
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; /* Aktivuj hodiny pro PORTB */
    // konfigurace pinu s tlacitky, porty jsou po resetu implicitne jako vstupy
	for(i=3; i<=7; i++) {
		PORTB->PCR[i] = ( PORT_PCR_ISF(0x01) /* Nuluj ISF (Interrupt Status Flag) */
				| PORT_PCR_IRQC(0x0A) /* Interrupt enable on failing edge */
				| PORT_PCR_MUX(0x01) /* Pin Mux Control to GPIO */
				| PORT_PCR_PE(0x01) /* Pull resistor enable... */
				| PORT_PCR_PS(0x01)); /* ...select Pull-Up */
    // Pozn.: Pull-Up rezistory je NUTNE zapnout, protoze jsou tlacitka pripojena
    // z portu primo na zem (stisk da na portu log. 0, tj. generuje SESTUPNOU hranu),
    // Pull-Up rezistor zajisti, ze nestisknute tlacitko da na portu STABILNI "1".
    // Stiskem tlacitka pak generujeme preruseni, ktere musi byt nakonfigurovano
    // prave na SESTUPNOU hranu na prislusnych vyvodech portu B. Vizte schema kitu.
	}
    // Port B, kde je RED LED a bzucak nastavime jako vystupni GPIO, abychom
    // mohli tyto komponenty programove ridit - vizte funkce beep(), flash()
	PORTB->PCR[9] |= PORT_PCR_MUX(0x01);  /* Pin Mux Control - GPIO (RED LED) */
	PORTB->PCR[10] |= PORT_PCR_MUX(0x01);  /* Pin Mux Control - GPIO (RED LED) */
	PORTB->PCR[11] |= PORT_PCR_MUX(0x01);  /* Pin Mux Control - GPIO (RED LED) */
	PORTB->PCR[13] |= PORT_PCR_MUX(0x01); /* Pin Mux Control - GPIO (BZZZZZZ) */
	GPIOB_PDDR |= PIEZZO_MASK | LED_R_MASK | LED_G_MASK | LED_B_MASK;

	NVIC_ClearPendingIRQ(PORTB_IRQn); /* Nuluj priznak preruseni od portu B */
	NVIC_EnableIRQ(PORTB_IRQn);       /* Povol preruseni od portu B */
}

/* -------------------------------------------------------------------------------- *
 * Hlavni funkce                                                                    *
 * -------------------------------------------------------------------------------- */
int main(void) {
    init_hardware();

    //flash();
    //beep();

    while (1); /* zamezeni navratu z main(), v nekonecne smycce se ceka na
                  generovani preruseni nakonfigurovaneho v init_hardware(),
                  ktere bude obslouzeno v podprogramu PORTB_IRQHandler().
                  Preruseni bude vyvolano stiskem tlacitka na kitu. */
}
