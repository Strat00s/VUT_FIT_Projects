
/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2014
**                              Radek Hranický, 2014-2018
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

void htInit ( tHTable* ptrht ) {

    //go through and null every item
    for (int i = 0; i < HTSIZE; i++) (*ptrht)[i] = NULL;
}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není,
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {
    int tmp_hash = hashCode(key);               //get hash
    tHTItem *curr_item = (*ptrht)[tmp_hash];    //save possible item

    //while item with the hash exists
    while (curr_item != NULL) {
        if (strcmp(curr_item->key, key) == 0) { //compare keys
            return curr_item;
        }
        curr_item = curr_item->ptrnext; //go to next item
    }
    return NULL;
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {
    int tmp_hash = hashCode(key);               //get hash
    tHTItem *curr_item = htSearch(ptrht, key);  //try and search for item

    //update data
    if (curr_item != NULL){
        curr_item->data = data;
        return;
    }
    //alloc new item
    tHTItem *new_item = malloc(sizeof(tHTItem));
    if (new_item == NULL) return;

    //save data
    new_item->data = data;
    new_item->key = key;

    new_item->ptrnext = (*ptrht)[tmp_hash]; //move first item
    (*ptrht)[tmp_hash] = new_item;          //add new as first
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {
    tHTItem *tmp_item = htSearch(ptrht, key);   //search for item

    if (tmp_item == NULL) return NULL;

    return &tmp_item->data; //return data
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {
    int tmp_hash = hashCode(key);           //get hash
    tHTItem *tmp_item = (*ptrht)[tmp_hash]; //get first item
    tHTItem *old_tmp_item = NULL;           //old item (used for check) (if stays null, first item was found)
    tHTItem *del_item = NULL;               //item to delete (if stays null, no item was found)

    //if there is nothing to remove
    if (tmp_item == NULL) return;

    //while current item not null
    while (tmp_item != NULL) {
        //on same key
        if (tmp_item->key == key) {
            del_item = tmp_item;    //save which item to delete
            break;
        }

        old_tmp_item = tmp_item;        //save current (previous) item
        tmp_item = tmp_item->ptrnext;   //move to next item
    }

    if (del_item == NULL) return;   //no item was found

    //if old item was not used, first one is to be deleted
    if (old_tmp_item == NULL) {
        (*ptrht)[tmp_hash] = del_item->ptrnext;
        free(del_item);
        return;
    }

    old_tmp_item->ptrnext = del_item->ptrnext;   //"jump"
    free(del_item); //delete
}

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll ( tHTable* ptrht ) {
    //go through table
    for (int i = 0; i < HTSIZE; i++) {
        tHTItem *tmp_item = (*ptrht)[i];    //start at first "bucket"

        //go through bucket
        while (tmp_item != NULL){
            (*ptrht)[i] = tmp_item->ptrnext;    //"jump"
            free(tmp_item);                     //delete
            tmp_item = (*ptrht)[i];             //move "back" to start
        }
        (*ptrht)[i] = NULL; //null
    }
}
