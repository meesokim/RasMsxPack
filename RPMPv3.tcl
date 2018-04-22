# Copyright (C) 2017  Intel Corporation. All rights reserved.
# Your use of Intel Corporation's design tools, logic functions 
# and other software and tools, and its AMPP partner logic 
# functions, and any output files from any of the foregoing 
# (including device programming or simulation files), and any 
# associated documentation or information are expressly subject 
# to the terms and conditions of the Intel Program License 
# Subscription Agreement, the Intel Quartus Prime License Agreement,
# the Intel FPGA IP License Agreement, or other applicable license
# agreement, including, without limitation, that your use is for
# the sole purpose of programming logic devices manufactured by
# Intel and sold by Intel or its authorized distributors.  Please
# refer to the applicable agreement for further details.

# Quartus Prime: Generate Tcl File for Project
# File: RPMPv3.tcl
# Generated on: Sat Apr 21 21:15:27 2018

# Load Quartus Prime Tcl Project package
package require ::quartus::project

set need_to_close_project 0
set make_assignments 1

# Check that the right project is open
if {[is_project_open]} {
	if {[string compare $quartus(project) "RPMPv3"]} {
		puts "Project RPMPv3 is not open"
		set make_assignments 0
	}
} else {
	# Only open if not already open
	if {[project_exists RPMPv3]} {
		project_open -revision RPMPv3 RPMPv3
	} else {
		project_new -revision RPMPv3 RPMPv3
	}
	set need_to_close_project 1
}

# Make assignments
if {$make_assignments} {
	set_global_assignment -name FAMILY "MAX II"
	set_global_assignment -name DEVICE EPM240T100C5
	set_global_assignment -name ORIGINAL_QUARTUS_VERSION 17.1.0
	set_global_assignment -name PROJECT_CREATION_TIME_DATE "19:09:52  APRIL 21, 2018"
	set_global_assignment -name LAST_QUARTUS_VERSION "17.1.0 Lite Edition"
	set_global_assignment -name PROJECT_OUTPUT_DIRECTORY output_files
	set_global_assignment -name MIN_CORE_JUNCTION_TEMP 0
	set_global_assignment -name MAX_CORE_JUNCTION_TEMP 85
	set_global_assignment -name DEVICE_FILTER_PACKAGE TQFP
	set_global_assignment -name DEVICE_FILTER_PIN_COUNT 100
	set_global_assignment -name ERROR_CHECK_FREQUENCY_DIVISOR "-1"
	set_global_assignment -name POWER_EXT_SUPPLY_VOLTAGE_TO_REGULATOR 3.3V
	set_global_assignment -name EDA_SIMULATION_TOOL "ModelSim-Altera (Verilog)"
	set_global_assignment -name EDA_TIME_SCALE "1 ps" -section_id eda_simulation
	set_global_assignment -name EDA_RUN_TOOL_AUTOMATICALLY ON -section_id eda_simulation
	set_global_assignment -name EDA_OUTPUT_DATA_FORMAT "VERILOG HDL" -section_id eda_simulation
	set_global_assignment -name QIP_FILE osc.qip
	set_global_assignment -name VERILOG_FILE RPMPv3.v
	set_global_assignment -name NUM_PARALLEL_PROCESSORS 4
	set_location_assignment PIN_17 -to clk
	set_location_assignment PIN_77 -to r[0]
	set_location_assignment PIN_78 -to r[1]
	set_location_assignment PIN_99 -to r[2]
	set_location_assignment PIN_98 -to r[3]
	set_location_assignment PIN_96 -to r[4]
	set_location_assignment PIN_76 -to r[5]
	set_location_assignment PIN_74 -to r[6]
	set_location_assignment PIN_81 -to r[7]
	set_location_assignment PIN_83 -to r[8]
	set_location_assignment PIN_84 -to r[9]
	set_location_assignment PIN_86 -to r[10]
	set_location_assignment PIN_82 -to r[11]
	set_location_assignment PIN_75 -to r[12]
	set_location_assignment PIN_73 -to r[13]
	set_location_assignment PIN_97 -to r[14]
	set_location_assignment PIN_95 -to r[15]
	set_instance_assignment -name AUTO_CLOCK_ENABLE_RECOGNITION ON -to clk

	# Commit assignments
	export_assignments

	# Close project
	if {$need_to_close_project} {
		project_close
	}
}
