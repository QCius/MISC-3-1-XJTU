`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/12/08 21:40:47
// Design Name: 
// Module Name: CU
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


module Controller(
    input clk,
    input state,
	input [31:0] instruction,
	output MemToReg, MemWrite,
	output Branch,ALUSrc,
	output RegDst,RegWrite,
	output Jump,
	output [4:0] ALUControl);
	reg [5:0]Op,Funct;
always @(posedge clk) begin
    // 从指令中提取字段
    if(state == 3'b001)begin
    Op = instruction[31:26];
    Funct=instruction[5:0];
    end
end
	wire[1:0] ALUOp;
	wire Branch;
	MainDec MainDec_1(Op,MemToReg,MemWrite,Branch,ALUSrc,RegDst,RegWrite,Jump,ALUOp);
	ALUDec   ALUDec_1(Funct,ALUOp,ALUControl);	
endmodule
