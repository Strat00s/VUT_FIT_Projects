%write items from row one by one with 'space' between them
getcols([]).
getcols([Cur|Rest]):-
    write(Cur),
    write(' '),
    getcols(Rest).

%after each row, write new line and repeat till the end
uloha19([]).
uloha19([Row|Rows]):-
	getcols(Row),
    nl,
  	uloha19(Rows).

/** <examples>
?- uloha19(SEZNAM)
*/

