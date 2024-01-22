`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/12/08 21:55:33
// Design Name: 
// Module Name: decoder38
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module Decoder38(I, Y);
	input [2:0] I;
	output reg[7:0] Y;
	
	always@(*) begin
		case(I)
			3'b000: Y = 8'b0000_0001;
			3'b001: Y = 8'b0000_0010;
			3'b010: Y = 8'b0000_0100;
			3'b011: Y = 8'b0000_1000;
			3'b100: Y = 8'b0001_0000;
			3'b101: Y = 8'b0010_0000;
			3'b110: Y = 8'b0100_0000;
			3'b111: Y = 8'b1000_0000;
		endcase
	end
endmodule

