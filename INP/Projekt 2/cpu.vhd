-- cpu.vhd: Simple 8-bit CPU (BrainF*ck interpreter)
-- Copyright (C) 2020 Brno University of Technology,
--                    Faculty of Information Technology
-- Author: Lukáš Baštýø (xbasty00)
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- ram[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_WE    : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti 
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

 	-- FSM states
 	type states is (
		start_state,		--start state
		fetch_state,		--get data
		decode_state,	--decode "input" to states
		end_state,		--end state (nothing to do)
		pc_inc_state,	--increase pc
		pc_dec_state,	--decrease pc
		mx_inc_state,	--increase RAM value
		mx_dec_state,	--decrease RAM value
		pc_end_state,	--write changes after mx
		ptr_inc_state,
		ptr_dec_state,
		ptr_done_state,
		loop_start_state,
		loop_start_done_state,
		loop_skip_state,
		loop_end_state,
		write_state,
		read_state
	);
	signal state :  states := start_state;	--assign initial state
	signal next_state : states;
	
 
 -- Program Counter - PC
	signal pc_code_addr : std_logic_vector (11 downto 0);
	signal pc_2_ras : std_logic_vector (11 downto 0);
	signal pc_inc : std_logic;
	signal pc_dec : std_logic;
	signal pc_load : std_logic;
	
 -- Return Address Stack - RAS
	type t_ras_array is array (0 to 15) of std_logic_vector(11 downto 0);	--address array
	signal ras_array : t_ras_array;
	signal ras_push : std_logic;
	signal ras_pop : std_logic;
	
 -- Loop Counter - CNT
	signal cnt_inc : std_logic;
	signal cnt_dec : std_logic;
	shared variable loop_cnt : integer := 0;
	shared variable before_skip : integer := 0;
	
 -- Ram Pointer - PTR
	signal ptr_data_addr : std_logic_vector (9 downto 0);
	signal ptr_inc : std_logic;
	signal ptr_dec : std_logic;
	
 -- Multiplexor - MX
	signal mx_data_wdata : std_logic_vector (7 downto 0) := (others => '0');
	signal mx_sel : std_logic_vector (1 downto 0) := (others => '0');


begin

 -- PC
	PCProc: process (RESET, CLK, pc_inc, pc_dec, pc_load) is
		begin
			if (RESET = '1') then							--reset
				pc_code_addr <= (others => '0');
			elsif (rising_edge(CLK)) then					--on CLK
				if (pc_inc = '1') then
					pc_code_addr <= pc_code_addr + 1;		--add
				elsif (pc_dec = '1') then
					pc_code_addr <= pc_code_addr - 1;		--sub
				elsif (pc_load = '1') then
					pc_code_addr <= ras_array(loop_cnt-1);	--load RAS
				end if;
			end if;
		end process;
	pc_2_ras <= pc_code_addr;	--save current position
	CODE_ADDR <= pc_code_addr;
	
 -- RAS TODO
	RASProc: process (RESET, CLK, pc_2_ras, ras_push, ras_pop) is
		begin
			if (RESET = '1') then
				--zero the array on reset
				ras_array <= ("000000000000", "000000000000", "000000000000", "000000000000", "000000000000", "000000000000", "000000000000", "000000000000", "000000000000", "000000000000", "000000000000", "000000000000", "000000000000", "000000000000", "000000000000", "000000000000");
			elsif (rising_edge(CLK)) then						--on CLK
				if (ras_push = '1') then
					ras_array(loop_cnt) <= pc_2_ras;			--save
				elsif (ras_pop = '1') then
					ras_array(loop_cnt-1) <= "000000000000";	--zero
				end if;
			end if;
		end process;

 -- CNT TODO 
	CNTProc: process (RESET, CLK, cnt_inc, cnt_dec) is
		begin
			if (RESET = '1') then				--reset
				loop_cnt := 0;
			elsif (rising_edge(CLK)) then		--on CLK
				if (cnt_inc = '1') then
					loop_cnt := loop_cnt + 1;	--increase loop count
				elsif (cnt_dec = '1') then
					loop_cnt := loop_cnt - 1;	--decrease loop count
				end if;
			end if;
		end process;

 -- PTR
	PTRProc: process (RESET, CLK, ptr_inc, ptr_dec) is 
		begin
			if (RESET = '1') then						--reset
				ptr_data_addr <= (others => '0');
			elsif (rising_edge(CLK)) then				--on CLK
				if (ptr_inc = '1') then
					ptr_data_addr <= ptr_data_addr + 1;	--move "right"
				elsif (ptr_dec = '1') then
					ptr_data_addr <= ptr_data_addr - 1;	--move "left"
				end if;
			end if;
		end process;
	DATA_ADDR <= ptr_data_addr;	--save

 -- Multiplexost	
	multiplexorProc: process (RESET, CLK, mx_sel) is
		begin
			if (RESET = '1') then										--reset
				mx_data_wdata <= (others => '0');
			elsif (rising_edge(CLK)) then
				case mx_sel is 
					when "00" => mx_data_wdata <= IN_DATA;				--read data from keyboard
					when "01" => mx_data_wdata <= DATA_RDATA + 1;		--add 1
					when "10" => mx_data_wdata <= DATA_RDATA - 1;		--sub 1
					when others => mx_data_wdata <= (others => '0');	--should not even happen, but is required
				end case;
			end if;
		end process;
	DATA_WDATA <= mx_data_wdata;	--save

 -- FSM		
	fsmProc: process(RESET, CLK, EN, OUT_BUSY, IN_VLD, CODE_DATA, DATA_RDATA, state) is
		begin
			if (RESET = '1') then			--reset back to start
				state <= start_state;
			elsif (rising_edge(CLK)) then	--on CLK
				if (EN = '1') then
					state <= next_state;	--go to next state
				end if;
			end if;

			----(INIT)----
			pc_inc   <= '0';
			pc_dec   <= '0';
			pc_load  <= '0';
			ptr_inc  <= '0';
			ptr_dec  <= '0';
			ras_push <= '0';
			ras_pop  <= '0';
			cnt_inc  <= '0';
			cnt_dec  <= '0';
			mx_sel   <= "00";
			CODE_EN  <= '0';
			DATA_EN  <= '0';
			DATA_WE  <= '0';
			OUT_WE   <= '0';
			IN_REQ   <= '0';
			
			case state is 
				when start_state =>	next_state <= fetch_state;	--on start, move to fetch
				
				when fetch_state =>	CODE_EN <= '1';	--get new data
										next_state <= decode_state;
				
				----(Get input from data)----
				when decode_state => 	case CODE_DATA is
											when "00111110" => next_state <= ptr_inc_state;
											when "00111100" => next_state <= ptr_dec_state;
											when "00101011" => next_state <= pc_inc_state;
											when "00101101" => next_state <= pc_dec_state;
											when "01011011" => next_state <= loop_start_state;
											when "01011101" => next_state <= loop_end_state;
											when "00101110" => next_state <= write_state;
											when "00101100" => next_state <= read_state;
											when "00000000" => next_state <= end_state;
											when others  =>	pc_inc <= '1';
																	next_state <= fetch_state;	--read next input
										end case;

				----(Move in RAM cells)----
				when ptr_inc_state => pc_inc <= '1';
										ptr_inc <= '1';
										next_state <= ptr_done_state;
				when ptr_dec_state =>	pc_inc <= '1';
										ptr_dec <= '1';
										next_state <= ptr_done_state;
				when ptr_done_state => 	DATA_EN <= '1';	--update RAM
										DATA_WE <= '0';
										next_state <= fetch_state;

				----(Change RAM value)----
				when pc_inc_state =>	DATA_EN <= '1';	--update RAM
										DATA_WE <= '0';
										next_state <= mx_inc_state;
				when pc_dec_state =>	DATA_EN <= '1';	--update RAM
										DATA_WE <= '0';
										next_state <= mx_dec_state;
				when mx_inc_state =>	mx_sel <= "01";	--change RAM
										next_state <= pc_end_state;
				when mx_dec_state =>	mx_sel <= "10";	--change RAM
										next_state <= pc_end_state;
				when pc_end_state =>	DATA_EN <= '1';	--overide RAM
										DATA_WE <= '1';
										pc_inc <= '1';
										next_state <= fetch_state;
				
				----(Write to display)----
				when write_state =>	if (OUT_BUSY = '1') then		--wait whyle busy
												DATA_EN <= '1';
												DATA_WE <= '0';
												next_state <= write_state;
											else
												OUT_WE <= '1';				--out write enable
												OUT_DATA <= DATA_RDATA;	--assing data
												pc_inc <= '1';
												next_state <= fetch_state;	--move on
											end if;
				
				----(Hopefully read from keyboard)----
				when read_state =>	if (IN_VLD = '1') then
												DATA_EN <= '1';
												DATA_WE <= '1';
												pc_inc <= '1';
												next_state <= fetch_state;
											else
												IN_REQ <= '1';
												mx_sel <= "00";
												next_state <= read_state;
											end if;
		
				----(Loop)----
				when loop_start_state =>	pc_inc <= '1';		--loop start + 1
													DATA_EN <= '1';	--update RAM
													DATA_WE <= '0';
													next_state <= loop_start_done_state;
											
				when loop_start_done_state =>	if (DATA_RDATA = "00000000") then	--skipping the loop
															pc_inc <= '1';
															CODE_EN <= '1';
															before_skip := loop_cnt;				--save loop cnt value
															next_state <= loop_skip_state;			--skip
														else
															DATA_EN <= '1';						--update RAM
															DATA_WE <= '0';
															ras_push <= '1';						--save to RAS
															next_state <= fetch_state;				--fetch
														end if;
														cnt_inc <= '1';							--increase loop count
													
				when loop_skip_state =>	if (CODE_DATA = "01011101") then	--on closing brackets -> 
													if (before_skip = loop_cnt) then
														next_state <= fetch_state;		--move on
													else
														pc_inc <= '1';					--read next data
														CODE_EN <= '1';
														next_state <= loop_skip_state;	--continue
													end if;
													cnt_dec <= '1';					--decrease loop count
												------------------------------------------------------------
												elsif (CODE_DATA = "01011011") then	--on opening brackets -> 
													cnt_inc <= '1';						--increase loop count
													pc_inc <= '1';							--read next data
													CODE_EN <= '1';
													next_state <= loop_skip_state;			--continue
												---------------------------------------------------
												else									--on anything else ->
													pc_inc <= '1';					--read next data
													CODE_EN <= '1';
													next_state <= loop_skip_state;	--continue
												end if;	
				
				when loop_end_state =>	if (DATA_RDATA = "00000000") then
													pc_inc <= '1';
													ras_pop <= '1';
													cnt_dec <= '1';
													next_state <= fetch_state;
												else
													pc_load <= '1';
													next_state <= fetch_state;
												end if;
										
				when end_state => CODE_EN <= '0';	--we don't need to read anything else after end
								
				when others => next_state <= end_state;
				end case;
			
		end process;
 
end behavioral;
