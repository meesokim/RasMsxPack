MAX II Oscillator Test
----------------------

4/2/2016 D. W. Hawkins (dwh@ovro.caltech.edu)

Simulation
----------

1. Start Modelsim

2. Change the the extracted zip file location and run the simulation script, eg.,

	> cd {C:\temp\altera_ufm_osc}
	> source scripts/sim.tcl

	The script will compile the source

	################################################
	# Setup ModelSim work directory: mwork ...
	################################################
	################################################
	# Building files
	################################################
	# ------------------------------------------------------------------------------
	# Compile C:/temp/altera_ufm_osc/src/toggle.vhd
	# ------------------------------------------------------------------------------
	# Model Technology ModelSim ALTERA vcom 10.3d Compiler 2014.10 Oct  7 2014
	# Start time: 12:44:53 on Apr 02,2016
	# vcom -reportprogress 300 C:/temp/altera_ufm_osc/src/toggle.vhd
	# -- Loading package STANDARD
	# -- Loading package TEXTIO
	# -- Loading package std_logic_1164
	# -- Loading package NUMERIC_STD
	# -- Loading package MATH_REAL
	# -- Loading package LPM_COMPONENTS
	# -- Loading package altera_mf_components
	# -- Loading package VITAL_Timing
	# -- Loading package VITAL_Primitives
	# -- Loading package maxii_atom_pack
	# -- Loading package maxii_components
	# -- Compiling entity toggle
	# -- Compiling architecture top of toggle
	# End time: 12:44:55 on Apr 02,2016, Elapsed time: 0:00:02
	# Errors: 0, Warnings: 0
	# ------------------------------------------------------------------------------
	# Compile C:/temp/altera_ufm_osc/test/toggle_tb.vhd
	# ------------------------------------------------------------------------------
	# Model Technology ModelSim ALTERA vcom 10.3d Compiler 2014.10 Oct  7 2014
	# Start time: 12:44:55 on Apr 02,2016
	# vcom -reportprogress 300 C:/temp/altera_ufm_osc/test/toggle_tb.vhd
	# -- Loading package STANDARD
	# -- Loading package TEXTIO
	# -- Loading package std_logic_1164
	# -- Compiling entity toggle_tb
	# -- Compiling architecture test of toggle_tb
	# End time: 12:44:55 on Apr 02,2016, Elapsed time: 0:00:00
	# Errors: 0, Warnings: 0
	# Testbenches:
	#
	#   toggle_tb - Toggle I/O testbench

3. Run the testbench

	> toggle_tb

	The wave window shows the toggle signals.

-------------------------------------------------------------------------------

Synthesis
---------

1. Start Quartus

2. Change the the extracted zip file location and run the synthesis script, eg.,

	> cd {C:\temp\altera_ufm_osc}
	> source scripts/synth.tcl

	The script will compile the source.

	Creating the work directory
	Running place-and-route
	All done!


The script has pin assignments for the rstN and q pins.
Edit the script for your specific hardware.
