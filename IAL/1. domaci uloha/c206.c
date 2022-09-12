
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2020
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu,
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem,
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    
    //init everything to null
    L->First = NULL;
    L->Act= NULL;
    L->Last = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/

    if (L->First == NULL && L->Last == NULL) return;    //empty list check

    //save second item
    tDLElemPtr next = L->First->rptr;

    //null everything but first
    L->Act = NULL;
    L->Last = NULL;

    //go thourght the list and free "new" first item
    while(next != NULL){
        free(L->First);
        L->First = next;
        next = L->First->rptr;
    }
    free(L->First);

    L->First = NULL;    //null first item
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	
    //malloc new item and check that it worked
    tDLElemPtr new = malloc(sizeof(struct tDLElem));
    if (new == NULL){
        DLError();
        return;
    }

    //assign values to new item
    new->data = val;
    new->rptr = L->First;
    new->lptr = NULL;

    //if there is no item, both last and first should point to new item
    if (L->First == NULL){
        L->Last = new;  //Last points to new
    }
    else {
        L->First->lptr = new;   //Firsts left points to new
    }

    L->First = new;     //first points to new
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	

    /*"Opposite" of DLInsertFirst*/
	
	//malloc new item and check that it worked
    tDLElemPtr new = malloc(sizeof(struct tDLElem));
    if (new == NULL){
        DLError();
        return;
    }

    //assign values to new item
    new->data = val;
    new->rptr = NULL;
    new->lptr = L->Last;

    //if there is no item, both last and first should point to new item
    if (L->Last == NULL){
        L->First = new;  //Last points to new
    }
    else {
        L->Last->rptr = new;   //Firsts left points to new
    }

    L->Last = new;     //first points to new
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	
    L->Act = L->First;
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	
	L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

    //check if empty
    if (L->First == NULL){
        DLError();
        return;
    }

    //return data from first
    *val = L->First->data;
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	
    //check if empty
    if (L->First == NULL){
        DLError();
        return;
    }

    //return data from last
    *val = L->Last->data;
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
	
	if (L->First == NULL) return;           //check if empty
    if (L->Act == L->First) L->Act = NULL;  //check if first is active

    //single item left
    if (L->First == L->Last){
        free(L->First);
        L->First = NULL;
        L->Last = NULL;
        return;
    }

    tDLElemPtr tmp = L->First->rptr;    //save second item | NULL<-First->want

    tmp->lptr = NULL;       //null pointer first<-tmp | NULL<-tmp->next
    L->First->rptr = NULL;  //null pointer first->tmp | NULL<-first->NULL
    free(L->First);         //free current first
    L->First = tmp;         //tmp is now our first
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L.
** Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se neděje.
**/ 
	
	if (L->First == NULL) return;           //check if empty
    if (L->Act == L->Last) L->Act = NULL;   //check if Last is active

    //single item left
    if (L->First == L->Last){
        free(L->First);
        L->First = NULL;
        L->Last = NULL;
        return;
    }

    tDLElemPtr tmp = L->Last->lptr;    //save second last item | want<-Last->NULL

    tmp->rptr = NULL;       //null pointer tmp->Last | prev<-tmp->NULL
    L->Last->lptr = NULL;   //null pointer last->tmp | NULL<-Last->NULL
    free(L->Last);          //free current Last
    L->Last = tmp;          //tmp is now our Last
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
	
    if (L->Act == NULL || L->Act == L->Last) return;    //check if not active or last is active

    tDLElemPtr delete = L->Act->rptr;   //save item that we want to delete

    L->Act->rptr = delete->rptr;        //Acts next now points to delete next | Act->delete->next >> Act->next
    //if deletes next is null
    if (delete->rptr == NULL){
        L->Last = L->Act;               //active becomes new last
    }
    else {
        delete->rptr->lptr = L->Act;    //otherwise "skip" delete | delete<-next >> Act<-next
    }

    free(delete);
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
	
    if (L->Act == NULL || L->Act == L->First) return;    //check if not active or last is active

    tDLElemPtr delete = L->Act->lptr;   //save item that we want to delete

    L->Act->lptr = delete->lptr;        //Acts prev now points to delete prev | prev<-delete<-Act >> prev<-Act
    //if deletes next is null
    if (delete->lptr == NULL){
        L->First = L->Act;              //active becomes new last
    }
    else {
        delete->lptr->rptr = L->Act;    //otherwise "skip" delete | prev->delete >> prev->Act
    }

    free(delete);
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

    if (L->Act == NULL) return; //if not active
	
    //alloc and check if success
    tDLElemPtr new = malloc(sizeof(struct tDLElem));
    if (new == NULL){
        DLError();
        return;
    }

    //save data to new
    new->data = val;
    new->lptr = L->Act;         // Act<-new
    new->rptr = L->Act->rptr;   // Act<-new->next

    //check if next is NULL
    if (new->rptr == NULL){
        L->Last = new;          //new becomes last
    }
    else {
        new->rptr->lptr = new;  // new<-next
    }

    L->Act->rptr = new;         // Act->new

    /*
     *  Act<-new->next
     *  Act->new<-next
     * 
     *  Act<->new<->next
     */
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	
    if (L->Act == NULL) return; //if not active
	
    //alloc and check if success
    tDLElemPtr new = malloc(sizeof(struct tDLElem));
    if (new == NULL){
        DLError();
        return;
    }

    //save data to new
    new->data = val;
    new->rptr = L->Act;         //new->Act
    new->lptr = L->Act->lptr;   //prev<-new->Act

    //check if prev is NULL
    if (new->lptr == NULL){
        L->First = new;         //new becomes first
    }
    else {
        new->lptr->rptr = new;  // prev->new
    }

    L->Act->lptr = new;         // new<-Act

    /*
     *  prev<-new->Act
     *  prev->new<-Act
     * 
     *  prev<->new<->Act
     */
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
	
    //if not active
    if (L->Act == NULL){
        DLError();
        return;
    }

    *val = L->Act->data;    //copy data form act to val
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
	
    if (L->Act == NULL) return; //if not active return

    L->Act->data = val; //save new value
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
	
	if (L->Act == NULL) return;

    L->Act = L->Act->rptr;
}

void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
	
	if (L->Act == NULL) return;

    L->Act = L->Act->lptr;
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	
	return L->Act == NULL ? 0 : 1;
}

/* Konec c206.c*/
