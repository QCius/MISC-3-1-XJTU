`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/12/08 22:02:21
// Design Name: 
// Module Name: IM
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


module IM (
    input clk,
    input rst, 
    input [2:0] state,
    input wire [31:0] Addr,
    output reg [31:0] instruction
);

reg [31:0] RAM[0:1023];

initial begin
    $readmemh("C:/Users/mCPU/instructions.txt", RAM);
end

always @(posedge clk or posedge rst) begin
    if (rst) begin
        instruction <= 32'h0; // Reset value
    end else if(state == 3'b000) begin
        instruction <= RAM[Addr/4];
    end
end

endmodule

