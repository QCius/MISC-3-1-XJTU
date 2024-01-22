`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/12/08 21:49:47
// Design Name: 
// Module Name: MainDec
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


module MainDec(
	input [5:0] Op,
	output MemToReg, MemWrite,
	output Branch, ALUSrc,
	output RegDst, RegWrite,
	output Jump,
	output [1:0] ALUControl);

	reg[8:0] Controls;

	assign{RegWrite,RegDst,ALUSrc,Branch,MemWrite,MemToReg,Jump,ALUControl}=Controls;

	always@(*)
		case(Op)
			6'b000000: Controls <=9'b110000010;// RTYPE
			6'b100011: Controls<=9'b101001000;//LW
			6'b101011: Controls <=9'b001010000;//SW
			6'b000100: Controls <=9'b000100001;//BEQ
			6'b001000: Controls<=9'b101000000;//ADDI
			6'b000010: Controls <=9'b000000100;//J
			default: Controls<=9'bzzzzzzzzz;//illegal Op
		endcase
endmodule

