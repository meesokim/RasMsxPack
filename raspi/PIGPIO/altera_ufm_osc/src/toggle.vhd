-- ----------------------------------------------------------------
-- toggle.vhd
--
-- 8/24/2009 D. W. Hawkins (dwh@ovro.caltech.edu)
--
-- Altera MAX II internal oscillator toggle I/O configuration.
--
-- MAX II CPLDs have a User Flash Memory (UFM) block that
-- includes an oscillator. The nominal frequency of the
-- oscillator is around 18MHz. A divided-by-4 version of the
-- oscillator is available to clock user logic.
-- Reference [1] indicates that the nominal frequency of the
-- user clock signal is 4.4MHz, with a range of 3.3MHz to
-- 5.5MHz (25% variation).
--
-- The toggle design uses the internal oscillator to clock a
-- reloading down-counter. The carry-out of the down-counter
-- is used to reload the counter, and to toggle an output
-- signal. The toggling output is a square-wave. If the toggle
-- period is set on the order of 1 second, it can be used
-- to blink an LED, while shorter periods can be viewed using
-- an oscilloscope.
-- 
-- ----------------------------------------------------------------
-- Notes:
-- ------
--
-- 1. Altera's support for the UFM component is not very good.
--    The code below implements different Altera-provided
--    components for simulation versus synthesis.
--
--    Using the MegaWizard to create a component creates logic
--    based on the maxii_ufm component, however, this component
--    can not easily be instantiated outside of an Altera
--    MegaWizard file with its comment directives. The UFM
--    Users Guide and Quartus help have the instantiation
--    ports for the altufm_none component, but there is no
--    simulation model in the altera_mf.vhd simulation library.
--
-- ----------------------------------------------------------------
-- References
-- ----------
--
-- [1] "MAX II Device Handbook", Altera, version 3.3,
--      August 2009.
--
-- [2] "ALTUFM Megafunction User's Guide", Altera,
--      version 2.0, August 2006.
--
-- [3] "AN496: Using the internal oscillator in MAX II CPLDs",
--     Altera, version 1.0, Dec. 2007.
--
-- ----------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.math_real.all;

-- Altera LPM components
library lpm;
use lpm.lpm_components.all;

-- Altera MegaFunctions
library altera_mf;
use altera_mf.altera_mf_components.all;

-- Altera MAX II components
-- altera translate_off
library maxii;
use maxii.maxii_components.all;
-- altera translate_on

-- ----------------------------------------------------------------

entity toggle is
	generic (
		-- Output toggle period
		PERIOD   : real := 1.0;

		-- Use the simulation model
		SIMULATE : boolean := false
	);
	port (
		-- Reset
		rstN : in  std_logic;

		-- Toggling output
		q    : out std_logic
	);
end entity;

-- ----------------------------------------------------------------

architecture top of toggle is


	-- ------------------------------------------------------------
	-- altufm_none component
	-- ------------------------------------------------------------
	--
	-- This component definition should not be required, however,
	-- the altera_mf.vhd simulation library does not contain the
	-- component, so Modelsim will not process the file. This
	-- declaration was copied from the Quartus help.
	--
	-- This block is skipped by Quartus.
	--
	-- altera translate_off
	component altufm_none
	generic (
		intended_device_family : string  := "UNUSED";
		erase_time             : natural := 500000000;
		lpm_file               : string  := "UNUSED";
		osc_frequency          : natural := 180000;
		port_arclkena          : string  := "PORT_UNUSED";
		port_drclkena          : string  := "PORT_UNUSED";
		program_time           : natural := 1600000;
		width_ufm_address      : natural := 9;
		lpm_hint               : string  := "UNUSED";
		lpm_type               : string  := "altufm_none"
	);
	port(
		arclk    : in  std_logic;
        arclkena : in  std_logic := '1';
        ardin    : in  std_logic;
        arshft   : in  std_logic;
        busy     : out std_logic;
        drclk    : in  std_logic;
        drclkena : in  std_logic := '1';
        drdin    : in  std_logic;
        drdout   : out std_logic;
        drshft   : in  std_logic;
        erase    : in  std_logic := '0';
        osc      : out std_logic;
		oscena   : in  std_logic;
		program  : in  std_logic := '0';
		rtpbusy  : out std_logic
	);
	end component;
	-- altera translate_on

	-- Nominal oscillator frequency
	constant fosc : real := 4.4e6;

	-- Half-period down counter parameters
	constant COUNT : integer :=
		integer(ceil(fosc*PERIOD/2.0));
	constant VALUE : integer := COUNT - 1; 
	constant WIDTH : integer := integer(ceil(log2(real(COUNT))));

	-- Clock and active high reset
	signal clk  : std_logic;
	signal rst  : std_logic;

	-- Down counter signals
	signal div_load : std_logic;
	signal div_done : std_logic;
	signal div_d    : std_logic_vector(WIDTH-1 downto 0);
	signal div_q    : std_logic_vector(WIDTH-1 downto 0);

	-- Toggle output
	signal q_out : std_logic;

begin

	-- ------------------------------------------------------------
	-- Internal oscillator
	-- ------------------------------------------------------------
	--
	-- Altera's synthesis and simulation libraries for the
	-- altufm_none component are not implemented correctly.
	-- The altufm_none component is recognized only by Quartus,
	-- whereas the maxii_ufm component is the only one available
	-- for simulation. The following logic takes care of the
	-- issue.
	--
	-- The defaults in maxii_components.vhd allow most of the 
	-- UFM signals to be left at their default settings.
	--
	-- altera translate_off
	g1: if (SIMULATE = true) generate
		u1: maxii_ufm
			generic map (
				-- Simulation frequency of 5.56MHz
				OSC_SIM_SETTING => 180000
			)
			port map ( 
				-- UPM signals without defaults
				-- (levels as generated by the MegaWizard tool)
				arclk  => '0',
				ardin  => '0',
				arshft => '0',
				drclk  => '0',
				drdin  => '0',
				drshft => '1',

				-- Oscillator signals
				oscena => '1',  -- always enabled
				osc    => clk
			  );
	end generate;
	-- altera translate_on

	g2: if (SIMULATE = false) generate
		u1: altufm_none
			generic map (
				-- Simulation frequency of 5.56MHz
				OSC_FREQUENCY => 180000
			)
			port map ( 
				-- UPM signals without defaults
				-- (levels as generated by the MegaWizard tool)
				arclk  => '0',
				ardin  => '0',
				arshft => '0',
				drclk  => '0',
				drdin  => '0',
				drshft => '1',
	
				-- Oscillator signals
				oscena => '1',  -- always enabled
				osc    => clk
			  );
	end generate;

	-- ------------------------------------------------------------
	-- Clock divider
	-- ------------------------------------------------------------
	--
    u2: lpm_counter
        generic map (
            LPM_WIDTH     => WIDTH,
            LPM_DIRECTION => "DOWN"
        )
        port map (
            -- Simulation-only output (suppress Quartus II warning)
            --
            -- altera translate_off
            q      => div_q,
            -- altera translate_on

            aclr   => rst,
            clock  => clk,
			sload  => div_load,
            cout   => div_done,
            data   => div_d
        );
	
	-- Active high reset
	rst <= not rstN;

	-- Load value
	div_d <= std_logic_vector(to_unsigned(VALUE, WIDTH));

	-- Load every time carry out asserts
	div_load <= div_done;

	-- ------------------------------------------------------------
	-- Toggle output
	-- ------------------------------------------------------------
	--
	process(clk, rstN)
	begin
		if (rstN = '0') then
			q_out <= '0';
		elsif rising_edge(clk) then
			if (div_done = '1') then
				q_out <= not q_out;
			end if;
		end if;
	end process;
	q <= q_out;

end architecture;




