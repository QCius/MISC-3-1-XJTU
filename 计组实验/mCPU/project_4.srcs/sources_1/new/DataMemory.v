`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/12/08 22:49:33
// Design Name: 
// Module Name: DataMemory
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


module DataMemory(
    input clk,
    input [2:0]state,
    input [31:0] DAddr,
    input [31:0] DataIn,
    input RD,    // �����ƣ�1��Ч
    input WR,    // д���ƣ�1��Ч
    output [31:0] DataOut    // ����32λ����
    );
    reg [31:0] RAM[0:1023];
    assign DataOut[31:0]   = (RD==1) ? RAM[DAddr] : 32'bz;
    always @(posedge clk) begin
        if(WR == 1 && state == 3'b100) begin
            RAM[DAddr] <= DataIn[31:0];
        end
    end
endmodule
