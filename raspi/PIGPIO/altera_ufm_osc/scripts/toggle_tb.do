onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate -divider {External signals}
add wave -noupdate -format Logic /toggle_tb/rstn
add wave -noupdate -format Logic /toggle_tb/q
add wave -noupdate -divider {Internal signals}
add wave -noupdate -format Logic /toggle_tb/u1/clk
add wave -noupdate -format Logic /toggle_tb/u1/div_load
add wave -noupdate -format Logic /toggle_tb/u1/div_done
add wave -noupdate -format Literal /toggle_tb/u1/div_d
add wave -noupdate -format Literal /toggle_tb/u1/div_q
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {0 ps} 0} {{Cursor 2} {41543277 ps} 0} {{Cursor 3} {51537179 ps} 0}
configure wave -namecolwidth 191
configure wave -valuecolwidth 40
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ps
update
WaveRestoreZoom {0 ps} {266700 ns}
