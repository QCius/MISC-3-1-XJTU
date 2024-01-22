`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/12/08 21:52:28
// Design Name: 
// Module Name: ALU
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


module ALU(F, CF, A, B, OP);
	parameter SIZE = 32;
	output reg [31:0] F;
	output CF;
	input [SIZE-1:0] A, B;
	input [4:0] OP;
	
	parameter ALU_AND = 5'b00010;
	parameter ALU_OR =  5'b00011;
	parameter ALU_NOR = 5'b00100;
	parameter ALU_ADD = 5'b00000;
	parameter ALU_SUB = 5'b00001;

	wire [7:0] EN;
	wire [SIZE-1:0] Fw, Fa;
	assign Fa = A & B;

	always@(*) begin
		case(OP)
			ALU_AND: begin F <= Fa;     end
			ALU_OR: begin F <= A|B;     end
			ALU_NOR: begin F <= ~(A|B); end
			default: F <= Fw;
		endcase
	end

	Decoder38 decoder38_1(OP[2:0], EN);
	ADD add_1(Fw, CF, A, B, EN[0]);
	SUB sub_1(Fw, CF, A, B, EN[1]);
endmodule

