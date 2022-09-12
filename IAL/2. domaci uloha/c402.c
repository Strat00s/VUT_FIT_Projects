
/* c402.c: ********************************************************************}
{* Téma: Nerekurzivní implementace operací nad BVS
**                                     Implementace: Petr Přikryl, prosinec 1994
**                                           Úpravy: Petr Přikryl, listopad 1997
**                                                     Petr Přikryl, květen 1998
**			  	                        Převod do jazyka C: Martin Tuček, srpen 2005
**                                         Úpravy: Bohuslav Křena, listopad 2009
**                                                 Karel Masařík, říjen 2013
**                                                 Radek Hranický 2014-2018
**
** S využitím dynamického přidělování paměti, implementujte NEREKURZIVNĚ
** následující operace nad binárním vyhledávacím stromem (předpona BT znamená
** Binary Tree a je u identifikátorů uvedena kvůli možné kolizi s ostatními
** příklady):
**
**     BTInit .......... inicializace stromu
**     BTInsert ........ nerekurzivní vložení nového uzlu do stromu
**     BTPreorder ...... nerekurzivní průchod typu pre-order
**     BTInorder ....... nerekurzivní průchod typu in-order
**     BTPostorder ..... nerekurzivní průchod typu post-order
**     BTDisposeTree ... zruš všechny uzly stromu
**
** U všech funkcí, které využívají některý z průchodů stromem, implementujte
** pomocnou funkci pro nalezení nejlevějšího uzlu v podstromu.
**
** Přesné definice typů naleznete v souboru c402.h. Uzel stromu je typu tBTNode,
** ukazatel na něj je typu tBTNodePtr. Jeden uzel obsahuje položku int Cont,
** která současně slouží jako užitečný obsah i jako vyhledávací klíč
** a ukazatele na levý a pravý podstrom (LPtr a RPtr).
**
** Příklad slouží zejména k procvičení nerekurzivních zápisů algoritmů
** nad stromy. Než začnete tento příklad řešit, prostudujte si důkladně
** principy převodu rekurzivních algoritmů na nerekurzivní. Programování
** je především inženýrská disciplína, kde opětné objevování Ameriky nemá
** místo. Pokud se Vám zdá, že by něco šlo zapsat optimálněji, promyslete
** si všechny detaily Vašeho řešení. Povšimněte si typického umístění akcí
** pro různé typy průchodů. Zamyslete se nad modifikací řešených algoritmů
** například pro výpočet počtu uzlů stromu, počtu listů stromu, výšky stromu
** nebo pro vytvoření zrcadlového obrazu stromu (pouze popřehazování ukazatelů
** bez vytváření nových uzlů a rušení starých).
**
** Při průchodech stromem použijte ke zpracování uzlu funkci BTWorkOut().
** Pro zjednodušení práce máte předem připraveny zásobníky pro hodnoty typu
** bool a tBTNodePtr. Pomocnou funkci BTWorkOut ani funkce pro práci
** s pomocnými zásobníky neupravujte
** Pozor! Je třeba správně rozlišovat, kdy použít dereferenční operátor *
** (typicky při modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem
** (např. při vyhledávání). V tomto příkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, použijeme dereferenci.
**/

#include "c402.h"
int solved;

void BTWorkOut (tBTNodePtr Ptr)		{
/*   ---------
** Pomocná funkce, kterou budete volat při průchodech stromem pro zpracování
** uzlu určeného ukazatelem Ptr. Tuto funkci neupravujte.
**/

	if (Ptr==NULL)
    printf("Chyba: Funkce BTWorkOut byla volána s NULL argumentem!\n");
  else
    printf("Výpis hodnoty daného uzlu> %d\n",Ptr->Cont);
}

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu tBTNodePtr. Tyto funkce neupravujte.
**/

void SInitP (tStackP *S)
/*   ------
** Inicializace zásobníku.
**/
{
	S->top = 0;
}

void SPushP (tStackP *S, tBTNodePtr ptr)
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
{
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
  if (S->top==MAXSTACK)
    printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
  else {
		S->top++;
		S->a[S->top]=ptr;
	}
}

tBTNodePtr STopPopP (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
		return(NULL);
	}
	else {
		return (S->a[S->top--]);
	}
}

bool SEmptyP (tStackP *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu bool. Tyto funkce neupravujte.
*/

void SInitB (tStackB *S) {
/*   ------
** Inicializace zásobníku.
**/

	S->top = 0;
}

void SPushB (tStackB *S,bool val) {
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
	if (S->top==MAXSTACK)
		printf("Chyba: Došlo k přetečení zásobníku pro boolean!\n");
	else {
		S->top++;
		S->a[S->top]=val;
	}
}

bool STopPopB (tStackB *S) {
/*   --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0) {
		printf("Chyba: Došlo k podtečení zásobníku pro boolean!\n");
		return(NULL);
	}
	else {
		return(S->a[S->top--]);
	}
}

bool SEmptyB (tStackB *S) {
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Následuje jádro domácí úlohy - funkce, které máte implementovat.
*/

void BTInit (tBTNodePtr *RootPtr)	{
/*   ------
** Provede inicializaci binárního vyhledávacího stromu.
**
** Inicializaci smí programátor volat pouze před prvním použitím binárního
** stromu, protože neuvolňuje uzly neprázdného stromu (a ani to dělat nemůže,
** protože před inicializací jsou hodnoty nedefinované, tedy libovolné).
** Ke zrušení binárního stromu slouží procedura BTDisposeTree.
**
** Všimněte si, že zde se poprvé v hlavičce objevuje typ ukazatel na ukazatel,
** proto je třeba při práci s RootPtr použít dereferenční operátor *.
**/
    //"same as c401"
    if ((*RootPtr) != NULL) {
        (*RootPtr)->Cont = 0;
        (*RootPtr)->LPtr = NULL;
        (*RootPtr)->RPtr = NULL;
    }
}

void BTInsert (tBTNodePtr *RootPtr, int Content) {
/*   --------
** Vloží do stromu nový uzel s hodnotou Content.
**
** Z pohledu vkládání chápejte vytvářený strom jako binární vyhledávací strom,
** kde uzly s hodnotou menší než má otec leží v levém podstromu a uzly větší
** leží vpravo. Pokud vkládaný uzel již existuje, neprovádí se nic (daná hodnota
** se ve stromu může vyskytnout nejvýše jednou). Pokud se vytváří nový uzel,
** vzniká vždy jako list stromu. Funkci implementujte nerekurzivně.
**/

    //"same as c401"
    //nothing "yet"
    if ((*RootPtr) == NULL) {

        //new root node
        (*RootPtr) = malloc(sizeof(struct tBTNode));
        if ((*RootPtr) == NULL) return;

        (*RootPtr)->Cont = Content;
        (*RootPtr)->LPtr = NULL;
        (*RootPtr)->RPtr = NULL;
        return;
    }

    tBTNodePtr curr_node = (*RootPtr);
    while (curr_node != NULL) {

        //same
        if (curr_node->Cont == Content) return;

        //smaller on the left
        else if (curr_node->Cont > Content) {

            //next left does not exist -> create one
            if (curr_node->LPtr == NULL) {
                tBTNodePtr new_node = malloc(sizeof(struct tBTNode));
                if (new_node == NULL) return;

                //save data
                new_node->Cont = Content;
                new_node->LPtr = NULL;
                new_node->RPtr = NULL;

                curr_node->LPtr = new_node; //save new node
                //printf("%d<-%d\n", curr_node->LPtr->Cont, curr_node->Cont);
            }
            else curr_node = curr_node->LPtr;   //iterate
        }

        //bigger on the right
        else if (curr_node->Cont < Content) {

            //next right does not exist -> create one
            if (curr_node->RPtr == NULL) {
                tBTNodePtr new_node = malloc(sizeof(struct tBTNode));
                if (new_node == NULL) return;

                //save data
                new_node->Cont = Content;
                new_node->LPtr = NULL;
                new_node->RPtr = NULL;

                curr_node->RPtr = new_node; //save new node
                //printf("%d->%d\n", curr_node->Cont, curr_node->RPtr->Cont);
            }
            else curr_node = curr_node->RPtr;   //iterate

        }
    }
}

/*                                  PREORDER                                  */

void Leftmost_Preorder (tBTNodePtr ptr, tStackP *Stack)	{
/*   -----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Preorder navštívené uzly zpracujeme voláním funkce BTWorkOut()
** a ukazatele na ně is uložíme do zásobníku.
**/

    tBTNodePtr tmp_node = ptr;

    //Go as far left as possible
    while(tmp_node != NULL) {
        BTWorkOut(tmp_node);        //has to be here to print curr node first
        SPushP(Stack, tmp_node);    //push
        tmp_node = tmp_node->LPtr;  //iterate
    }
}

void BTPreorder (tBTNodePtr RootPtr)	{
/*   ----------
** Průchod stromem typu preorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Preorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut() -> already in Leftmost_Preorder.
**/

    if (RootPtr != NULL) {

        //create new stack
        tStackP *tmp_stack = malloc(sizeof(tStackP));
        if (tmp_stack == NULL) return;

        SInitP(tmp_stack);  //init stack
        Leftmost_Preorder(RootPtr, tmp_stack);  //go to left

        //while not empty, iterate (from last item on stack) | from most left go to right and iterate
        while (!SEmptyP(tmp_stack)) {
            tBTNodePtr tmp_node = STopPopP(tmp_stack);
            if (tmp_node->RPtr != NULL) Leftmost_Preorder(tmp_node->RPtr, tmp_stack);   
        }

        free(tmp_stack);    //free once done
    }
}


/*                                  INORDER                                   */

void Leftmost_Inorder(tBTNodePtr ptr, tStackP *Stack)		{
/*   ----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Inorder ukládáme ukazatele na všechny navštívené uzly do
** zásobníku.
**/

    tBTNodePtr tmp_node = ptr;

    //Go as far left as possible
    while(tmp_node != NULL) {
        SPushP(Stack, tmp_node);    //push
        tmp_node = tmp_node->LPtr;  //iterate
    }
}

void BTInorder (tBTNodePtr RootPtr)	{
/*   ---------
** Průchod stromem typu inorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Inorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut().
**/

    if (RootPtr != NULL) {

        tBTNodePtr tmp_node = RootPtr;

        //create new stack
        tStackP *tmp_stack = malloc(sizeof(tStackP));
        if (tmp_stack == NULL) return;

        SInitP(tmp_stack);                      //init stack
        Leftmost_Inorder(tmp_node, tmp_stack);  //go to left

        //while stack is not empty
        while(!SEmptyP(tmp_stack)) {
            tmp_node = STopPopP(tmp_stack);                     //move on stack
            if (tmp_node->RPtr != NULL) {                       //if right node exist
                BTWorkOut(tmp_node);                            //wright current node
                Leftmost_Inorder(tmp_node->RPtr, tmp_stack);    //go to the left on the right node   
            }
            else BTWorkOut(tmp_node);       //otherwise whole node done
        }

        free(tmp_stack);
    }
}

/*                                 POSTORDER                                  */

void Leftmost_Postorder (tBTNodePtr ptr, tStackP *StackP, tStackB *StackB) {
/*           --------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Postorder ukládáme ukazatele na navštívené uzly do zásobníku
** a současně do zásobníku bool hodnot ukládáme informaci, zda byl uzel
** navštíven poprvé a že se tedy ještě nemá zpracovávat.
**/

    tBTNodePtr tmp_node = ptr;

    //Go as far left as possible
    while(tmp_node != NULL) {
        SPushP(StackP, tmp_node);   //push
        SPushB(StackB, FALSE);      //push to b
        tmp_node = tmp_node->LPtr;  //iterate
    }
}

void BTPostorder (tBTNodePtr RootPtr)	{
/*           -----------
** Průchod stromem typu postorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Postorder, zásobníku ukazatelů a zásobníku hotdnot typu bool.
** Zpracování jednoho uzlu stromu realizujte jako volání funkce BTWorkOut().
**/

    if (RootPtr != NULL) {

        tBTNodePtr tmp_node = RootPtr;
        //printf("%d\n", tmp_node->Cont);

        //create new stacks
        tStackP *tmp_stackP = malloc(sizeof(tStackP));
        if (tmp_stackP == NULL) return;

        tStackB *tmp_stackB = malloc(sizeof(tStackB));
        if (tmp_stackB == NULL) return;

        SInitP(tmp_stackP);                      //init stack
        SInitB(tmp_stackB);                      //init stack
        Leftmost_Postorder(tmp_node, tmp_stackP, tmp_stackB);  //go to left

        //while stack is not empty
        
        while(!SEmptyP(tmp_stackP)) {

            tmp_node = STopPopP(tmp_stackP);    //last on stack
            //printf("StackP: %d | StackB: %d | Cont: %d\n", tmp_stackP->top, tmp_stackB->top, tmp_node->Cont);
            //printf("StackB: %d\n", tmp_stackB->top);

            //if false
            if (!STopPopB(tmp_stackB)) {
                SPushB(tmp_stackB, TRUE);       //push it back as TRUE

                //if right node exist
                if (tmp_node->RPtr != NULL) {
                    SPushP(tmp_stackP, tmp_node);   //push current node to stack (again)
                    Leftmost_Postorder(tmp_node->RPtr, tmp_stackP, tmp_stackB);    //go to the left on the right node   
                }
            }
            else SPushB(tmp_stackB, TRUE);

            if (STopPopB(tmp_stackB)) BTWorkOut(tmp_node);  //if last on bool stack is true, we have already seen it
            else SPushB(tmp_stackB, FALSE);     //if not true, dont change it
        }

        free(tmp_stackP);
        free(tmp_stackB);
    }
}


void BTDisposeTree (tBTNodePtr *RootPtr)	{
/*   -------------
** Zruší všechny uzly stromu a korektně uvolní jimi zabranou paměť.
**
** Funkci implementujte nerekurzivně s využitím zásobníku ukazatelů.
**/
    if ((*RootPtr) != NULL) {

        tBTNodePtr tmp_node = (*RootPtr);   //save root

        tStackP *tmp_stack = malloc(sizeof(tStackP));   //create stack
        if (tmp_stack == NULL) return;

        SInitP(tmp_stack);  //init stack

        //go through tree untill stack is empty AND node is null (we went through everything - stack, and there is nowhere else to go - null)
        while (true) {
            if (SEmptyP(tmp_stack) && tmp_node == NULL) break;
            
            //go to left, if we can
            if (tmp_node != NULL) Leftmost_Inorder(tmp_node, tmp_stack);

            //get and delete latest node on stack (and if possible) move to right node
            tBTNodePtr del_node = STopPopP(tmp_stack);
            tmp_node = del_node->RPtr;
            free(del_node);
        }

        free(tmp_stack);    //free stack
        (*RootPtr) = NULL;  //NULL root
    }
}

/* konec c402.c */

