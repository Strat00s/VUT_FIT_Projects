proc isim_script {} {

   add_divider "Signals of the Vigenere Interface"
   add_wave_label "" "CLK" /testbench/clk
   add_wave_label "" "RST" /testbench/rst
   add_wave_label "-radix ascii" "DATA" /testbench/tb_data
   add_wave_label "-radix ascii" "KEY" /testbench/tb_key
   add_wave_label "-radix ascii" "CODE" /testbench/tb_code

   add_divider "Vigenere Inner Signals"
   add_wave_label "" "STATE" /testbench/uut/add_sub
   # sem doplnte vase vnitrni signaly. chcete-li v diagramu zobrazit desitkove
   # cislo, vlozte do prvnich uvozovek: -radix dec

   add_wave_label "-radix dec" "OFFSET" /testbench/uut/offset
   add_wave_label "-radix ascii" "UP" /testbench/uut/add_up
   add_wave_label "-radix ascii" "DOWN" /testbench/uut/add_down
   add_wave_label "" "FSM_OUT" /testbench/uut/fsm_out

   run 8 ns
}
