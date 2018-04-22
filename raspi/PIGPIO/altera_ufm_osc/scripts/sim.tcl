#
# sim.tcl
#
# ModelSim simulation script
#

set top [pwd]
set src     $top/src
set test    $top/test
set scripts $top/scripts

# Create a list of VHDL files to build
#
set vfiles ""

# Top-level (synthesizeable) designs
lappend vfiles $src/toggle.vhd

# Testbench
lappend vfiles $test/toggle_tb.vhd

# Check/create Modelsim work directory
set mwork mwork
if {[file exists $mwork]} {
	echo "$mwork exists, deleting ..."
	file delete -force $mwork
}
echo "################################################"
echo "Setup ModelSim work directory: $mwork ..."
echo "################################################"

# Create a work subdirectory
vlib $mwork

# Map the library
vmap work [pwd]/$mwork

# Build the files
echo "################################################"
echo "Building files"
echo "################################################"

foreach vfile $vfiles {

    echo "------------------------------------------------------------------------------"
    echo "Compile $vfile"
    echo "------------------------------------------------------------------------------"
    vcom $vfile
}

echo "Testbenches:"
echo ""
echo "  toggle_tb - Toggle I/O testbench"

# Simulate the design
proc toggle_tb {} {
	global scripts
	vsim toggle_tb
	source scripts/toggle_tb.do
	run -a
}

