/*  Project #2
    Phone book searcher
    Lukas Bastyr, xbasty00
    23.10.2019

    What it is supposed to do:
    Search your list of contacts and hopefully return all matching ones
    Input is just like on T9 keyboard. Numbers and +.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_LN 102 // Lazy lenght checking

// A lot of variables   wordmatch, numbermatch, if it run at leas once
bool wordm, numm, atleastone = false;
unsigned int name_pos, arg_pos, match_cnt = 0;   // they can't reset during the cycle

/*---(Word matching)---*/
int word_match (char *name, char *argument, char *inum){
    
    wordm = false;

    // stay on current char and
    for (name_pos = name_pos; name_pos <= strlen(name); name_pos++){
        // scroll through argument
        for (arg_pos = 0; arg_pos < strlen(argument); arg_pos++){

            // untill they match
            // if so, go to next char in name and next argument array on the list
            if (tolower(name[name_pos]) == argument[arg_pos]){
                name_pos++;    // continue in the name
                match_cnt++;    // match count
                // if nubmer of matches = argv[1] lenght -> match
                if (match_cnt == strlen(inum)){
                    name_pos = arg_pos = match_cnt = 0;  // reset all values to 0 to start over
                    wordm = true;
                    return 0;   // exit
                }
                return 0;   // exit in order to change arg array
            }

            // if at the end of the name, exit with nothing
            else if (name_pos >= strlen(name)){
                name_pos = arg_pos = match_cnt = 0;
                return -1;
            }
        }
    }
    return 0;
}

/*---(Number matching, comments todo)---*/
int num_match (char *inum, char *number){
    
    unsigned int inum_pos, numb_pos, inum_pos_alt = 0;
    numm = false;
    
    // Keep char of inum and
    for (inum_pos = 0; inum_pos < strlen(inum); inum_pos++){
        inum_pos_alt = inum_pos;
        // check it against number
        for (numb_pos = 0; numb_pos < strlen(number); numb_pos++){

            // if number has + -> treat it as a zero 0
            if (number[numb_pos] == '+'){
                if (inum[inum_pos_alt] == '0'){
                    inum_pos_alt++;
                }  
            }

            // with match, change position on inum, but contionue in number
            else if (inum[inum_pos_alt] == number[numb_pos]){
                inum_pos_alt++;
            }
        }

        // number of matches == lenght of input -> match
        if (inum_pos_alt == strlen(inum)){
            numm = true;
            break;
        }

        //  when on end of the number, return with no match
        else if (numb_pos == strlen(number)){
            return -1;
        }
    }
    return 0;
}

// Asigns arrays to correct numbers
int sequencer (char *inum, char *name, int pos){

    // Atoi segfaults when I want to change x position to int
    char ch[1]; ch[0] = inum[pos];

    // Switch for T9 arrays for current number
    switch (atoi(ch)){
        case 1:  return(word_match(name, "1", inum));    break;
        case 2:  return(word_match(name, "2abc", inum));   break;
        case 3:  return(word_match(name, "3def", inum));  break;
        case 4:  return(word_match(name, "4ghi", inum));   break;
        case 5:  return(word_match(name, "5jkl", inum));    break;
        case 6:  return(word_match(name, "6mno", inum));   break;
        case 7:  return(word_match(name, "7pqrs", inum));  break;
        case 8:  return(word_match(name, "8tuv", inum)); break;
        case 9:  return(word_match(name, "9wxyz", inum));   break;
        default: return(word_match(name, "0+", inum));    break;
    }
}

// Decides how many arguments we got.
int decider (int argc, char *name, char *number, char *inum){

    // check for number of arguments
    if (argc == 2){

        // algorithm start
        for (unsigned int i = 0; i < strlen(inum); i++){

            // check if input input is a number
            if (isdigit(inum[i]) == 0){
                fprintf(stderr, "Give me a correct argument!\n");
                return -1;
            }

            // Call sequencer to assign argv[1][i] to a correct T9 array
            // If we get to the end of a name without a match, 
            // break and continue with a new name.
            if (sequencer(inum, name, i) == -1){
                break;
            }
        }

        // Calling number matching
        num_match(inum, number);
        // If name or number or both got matched
        if (numm == true || wordm == true){
            atleastone = true;
            printf("%s, %s\n", name, number);
        }
    }       

    // If only one argument, print all names
    else if (argc == 1){
        atleastone = true;
        printf("%s, %s\n", name, number);
    }

    // Otherwise flip the table
    else {
        fprintf(stderr, "Give me just one argument!\n");
        return -1;
    }
    return 0;
}
 
int main (int argc, char *argv[]){

    // Setting name and number arrays to save guess what
    char name[MAX_LN], number[MAX_LN];

    // LOOps
    while (1){

        // Read whole line and remove EOL. Kinda counting that the name chart starts with name
        fgets(name, MAX_LN, stdin); 
        fgets(number, MAX_LN, stdin);

        name[strcspn(name, "\n")] = 0;              // this also makes arrays 1 shorter
        number[strcspn(number, "\n")] = 0;

        // Lazy checking for string lenght.
        if (strlen(name) > 100 || strlen(number) > 100){
            memset(name, 0, sizeof name); memset(number, 0, sizeof number);
            fprintf(stderr, "A really long name/number, so no more search for you!\n");
            return 1;
        }

        // check for the end of file
        if (feof(stdin) != 0){
            if (atleastone == false){
                fprintf(stderr, "Not found.\n");
            }
            return 1;
        }

        // call decider and exit if we got a wrong argument
        if (decider (argc, name, number, argv[1]) == -1){
            return 1;
        }
    }
    return 0;
}