`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/12/08 21:51:09
// Design Name: 
// Module Name: ALUdec
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


module ALUDec(//ALU
	input [5:0] Funct,
	input [1:0] ALUOp,
	output reg[4:0] ALUControl);

	always@(*)
		case(ALUOp)
			2'b00: ALUControl<=5'b00000;// add(for lw/sw/addi)
			2'b01: ALUControl <=5'b00001;//sub(for beq)
			default: case(Funct)     //R-type Instructions
				6'b100000: ALUControl<=5'b00000; //add
				6'b100010: ALUControl <=5'b00001;//sub
				6'b100100: ALUControl <=5'b00010;//and
				6'b100101: ALUControl<=5'b00011;//or
				default:      ALUControl<=3'bxxx;
				endcase
		endcase
endmodule

