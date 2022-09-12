library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

-- rozhrani Vigenerovy sifry
entity vigenere is
   port(
         CLK : in std_logic;
         RST : in std_logic;
         DATA : in std_logic_vector(7 downto 0);
         KEY : in std_logic_vector(7 downto 0);

         CODE : out std_logic_vector(7 downto 0)
    );
end vigenere;

-- V souboru fpga/sim/tb.vhd naleznete testbench, do ktereho si doplnte
-- znaky vaseho loginu (velkymi pismeny) a znaky klice dle vaseho prijmeni.

architecture behavioral of vigenere is

    -- Sem doplnte definice vnitrnich signalu, prip. typu, pro vase reseni,
    -- jejich nazvy doplnte tez pod nadpis Vigenere Inner Signals v souboru
    -- fpga/sim/isim.tcl. Nezasahujte do souboru, ktere nejsou explicitne
    -- v zadani urceny k modifikaci.
	 
	 signal offset: std_logic_vector(7 downto 0);	-- offset in number
	 signal add_up: std_logic_vector(7 downto 0);	-- letter up
	 signal add_down: std_logic_vector(7 downto 0);	-- letter down
	 
	 signal add_sub: std_logic_vector(1 downto 0) := "01"; -- start on sub (first CLK will flip it to add)
	 signal fsm_out: std_logic_vector(1 downto 0);	-- FSM output


begin

    -- Sem doplnte popis obvodu. Doporuceni: pouzivejte zakladni obvodove prvky
    -- (multiplexory, registry, dekodery,...), jejich funkce popisujte pomoci
    -- procesu VHDL a propojeni techto prvku, tj. komunikaci mezi procesy,
    -- realizujte pomoci vnitrnich signalu deklarovanych vyse.

    -- DODRZUJTE ZASADY PSANI SYNTETIZOVATELNEHO VHDL KODU OBVODOVYCH PRVKU,
    -- JEZ JSOU PROBIRANY ZEJMENA NA UVODNICH CVICENI INP A SHRNUTY NA WEBU:
    -- http://merlin.fit.vutbr.cz/FITkit/docs/navody/synth_templates.html.
	 
	------------(OFFSETS)------------
	offsetProc: process (DATA, KEY) is
	begin
		offset <= KEY - 64; -- calculate, how much the offset should be (-64 -> B = 66 - 64 = 2)
	end process;
	
	addUpProc: process (offset, DATA) is
		variable up_offset: std_logic_vector(7 downto 0);
	begin
		up_offset := offset;	-- save offset
		up_offset := DATA + up_offset;	-- add it to DATA
		if (up_offset > 90) then	-- if over ASCII Z
			up_offset := up_offset - 26; -- loop back to A
		end if;
		
		add_up <= up_offset;	-- save offset to up
	end process;
	
	addDownProc: process (offset, DATA) is
		variable down_offset: std_logic_vector(7 downto 0);
	begin
		down_offset := offset;	-- save offset
		down_offset := DATA - down_offset;	-- sub it from DATA
		if (down_offset < 65) then	-- if below ASCII A
			down_offset := down_offset + 26; -- loop back to Z
		end if;
		
		add_down <= down_offset;	-- save offset to down
	end process;
	
	------------(FSM)------------
	MealyProc: process (RST, CLK, DATA) is
	begin
	
		-- flip value every CLK
		if (rising_edge(CLK)) then
			add_sub <= "01" - add_sub;
		end if;
		
		fsm_out <= add_sub;	-- save value

		-- use # on RST
		if (RST = '1') then
			fsm_out <= "10";
		end if;
		
		-- use # if number
		if (DATA > 47 and DATA < 58) then
			fsm_out <= "11";
		end if;
		
		
	end process;
	
	------------(MUX)------------
	multiplexorProc: process (fsm_out, add_up, add_down) is
	begin
		case fsm_out is
			when "00" => CODE <= add_up;	-- use up letter
			when "01" => CODE <= add_down; -- use down letter
			when others => CODE <= "00100011";	-- #
		end case;
		
	end process;
	
	

end behavioral;
