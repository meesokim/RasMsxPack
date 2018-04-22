# synth.tcl
# 
# Quartus synthesis script.

package require ::quartus::project
package require ::quartus::flow

# Paths
set top        [pwd]
set scripts    $top/scripts
set src        $top/src
set work       $top/qwork

# Create the Quartus work folder
if {![file exists $work]} {
    puts "Creating the work directory"
    file mkdir $work
}

# Create all the generated files in the work directory    
cd $work

# Name the project after the "top" component name.
project_new -overwrite "toggle"

# VHDL source
set_global_assignment -name VHDL_FILE $src/toggle.vhd

# Device
set_global_assignment -name FAMILY "MAX II"
set_global_assignment -name DEVICE EPM570T100C3

# Tri-state unused I/O
set_global_assignment -name RESERVE_ALL_UNUSED_PINS "AS INPUT TRI-STATED"

# Enable weak-pull up on the rstN signal
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to rstN

# Pin assignments
set_location_assignment PIN_2 -to rstN
set_location_assignment PIN_1 -to q

# Place-and-route
puts "Running place-and-route"
execute_flow -compile

# Use one of the following to save the settings
#project_close
export_assignments

# Return to the top directory    
cd $top
puts "All done!"

