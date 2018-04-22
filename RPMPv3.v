module RPMPv3 (clk, r, rs, led, ACK, RATN, ATN, 
	zclk, zmreq, ziorq, zrd, zwr, a, d, zrst, zint, mbd, msltsl, zwait, dd, cd, ad, cmd);

input clk;
input zclk, zmreq, ziorq, zrd, zwr, zrst, msltsl;
output zint, mbd, zwait;
output cd, dd, ad;
input RATN;
output reg ACK;
output ATN;
input [15:0] a;
inout [7:0] d;
input [1:0] cmd;
inout reg [15:0] r;
output [3:0] rs;
output reg [2:0] led;


reg [10:0] wcnt;
reg oscena_sig;
wire osc_w, oscena_w, clk_w, clk0_w;

osc_altufm_osc_518 ufm (.osc(osc_w), .oscena(oscena_w)); 
reduced_osc rosc (.osc(osc_w), .clk(clk_0w));

assign oscena_w = 1'b1;

reg [2:0] RATNr;  always @(posedge osc_w) RATNr <= {RATNr[1:0], RATN};
wire RATN_risingedge = (RATNr==3'b001);  // now we can detect SCK rising edges

reg [2:0] ATNr;   always @(posedge osc_w) ATNr <= {ATNr[1:0], ATN};
wire ATN_risingedge = (ATNr == 3'b001);
assign clk_w = osc_w;

reg rtest;
reg [7:0] errors;
reg [255:0] ioreq;
reg [15:0] addr;
reg [15:0] test_addr;
reg [7:0] rdata;
reg rwait, rint;
wire [7:0] wdata;
wire bdir;
reg rvalid;

assign ad = 0;
assign cd = 0;
assign dd = ((ATN & rvalid) ? 1'b1 : 1'b0);
assign d = (rvalid ? rdata : 8'bZ); 
assign rs[0] = zrd;
assign rs[1] = zmreq;
assign rs[2] = msltsl;
assign rs[3] = zrst;
assign bdir = (zrd == 0 && ziorq == 0 && ioreq[a[7:0]] == 1'b1);
assign wdata = (zwr == 1'b0 ? d : 8'b0);
assign ATN = ((zmreq == 0 || ziorq == 0) && zrst == 1'b1 ? 1'b1 : 1'b0);
assign zwait = rwait;
assign zint = rint;
assign mbd = !bdir;

initial
begin 
	r = 16'h0;
	led = 3'b0;
	oscena_sig = 1'b1;
	ioreq = 256'b100000000000000000000;
	errors = 8'b0;
	rwait = 1'b1;
	rint = 1'b1;
	rvalid = 1'b0;
	wcnt = 10'b0;
end

always @(posedge clk)
begin
	if (wcnt < 100)
		wcnt = wcnt + 1;
end

always @(posedge clk_w)
begin
	led[0] <= !rvalid;
	led[1] <= msltsl;
	led[2] <= !ATN;
end

always @(posedge clk_w) 
begin
	if (ATN_risingedge) begin
		errors <= errors + 1;
		rvalid <= 1'b0;
	end
	if (RATN_risingedge) begin
		case (cmd)
			2'b00: begin
					if (rtest == 1'b1) begin
						r[15:0] <= test_addr;
						test_addr <= test_addr + 1;
					end
					else
						r[15:0] <= a;
					end
			2'b01: begin
					r[15] <= zrd;
					r[14] <= zmreq;
					r[13] <= zrst;
					r[12] <= errors > 0;
					r[7:0] <= wdata;
					if (errors > 0)
						errors <= errors - 1;
					end
			2'b10: begin
					r[7:0] <= 8'bZ;
					rdata <= r[7:0];
					rvalid <= 1'b1;
					if (errors > 0)
						errors <= errors - 1;
					 end
			2'b11: begin
					r[15:0] <= 8'bZ;
					rwait <= r[15];
					rint <= r[14];
					rtest <= r[11];
					if (r[9])
						ioreq[r[4:0]] <= r[8];
					end
		endcase
		ACK <= 1'b1;
	end
	else
		r[15:0] <= a;
	if (RATN == 1'b0)
		ACK <= 1'b0;
end


endmodule

module reduced_osc (osc, clk);

input osc;               // Output of internal osc
output clk;              // Reduced frequency clock

reg [15:0] count;
reg clk;

initial
begin
count = 0;
end

always @ (posedge osc) begin
count = count + 1;
clk <= count[15];
end

endmodule