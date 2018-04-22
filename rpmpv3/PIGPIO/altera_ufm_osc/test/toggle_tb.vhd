-- toggle_tb.vhd
--
-- 8/24/2009 D. W. Hawkins (dwh@ovro.caltech.edu)
--
-- Altera MAX II internal oscillator toggle I/O testbench.
--
-- ----------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------------------------

entity toggle_tb is
	generic (
		-- Period of the simulation output
		--
		-- Note:
		-- The nominal device frequency is 4.4MHz, whereas the
		-- Altera simulation model uses 5.56MHz, so the observed
		-- period will be shorter, eg. PERIOD = 10 us has
		-- a period of 10*4.4/5.56 = 7.91 us. To get an observed
		-- period of 10 us, use 10*5.56/4.4 = 12.6 us
		--
		PERIOD  : real := 12.6e-6
	);
end entity;

-------------------------------------------------------------------

architecture test of toggle_tb is

	component toggle is
		generic (
			PERIOD   : real;
			SIMULATE : boolean
		);
		port (
			rstN : in  std_logic;
			q    : out std_logic
		);
	end component;

	-- Reset
	signal rstN : std_logic;

	-- Toggle
	signal q : std_logic;

begin

    -- ------------------------------------------------------------
    -- Reset generator
    -- ------------------------------------------------------------
    --
	process
	begin
		rstN <= '0';
		wait for 1 us;
		rstN <= '1';
		wait; -- suspend process
	end process;

    -- ------------------------------------------------------------
    -- Toggle
    -- ------------------------------------------------------------
    --
	u1: toggle
		generic map (
			PERIOD   => PERIOD,
			SIMULATE => true
		)
		port map (
			rstN => rstN,                                                
			q    => q                                             
		);

    -- ------------------------------------------------------------
    -- Stimulus
    -- ------------------------------------------------------------
    --
    process
    begin
		-- --------------------------------------------------------
		-- Wait for reset to complete
		-- --------------------------------------------------------
		--
		wait until rising_edge(rstN);
		
		-- --------------------------------------------------------
		-- Wait for multiple periods of the toggle signal
		-- --------------------------------------------------------
		--
		wait for 20*(PERIOD*1.0e12*(1 ps));
		
		-- --------------------------------------------------------
		-- End simulation
		-- --------------------------------------------------------
		--
		wait for 1 us;
    	assert false
    		report "All done!"
    		severity failure;
    end process;
end architecture;
