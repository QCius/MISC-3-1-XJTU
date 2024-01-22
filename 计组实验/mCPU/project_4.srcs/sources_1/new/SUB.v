`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/12/09 21:10:29
// Design Name: 
// Module Name: SUB
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


module SUB(F, CF, A, B, EN);
  parameter SIZE = 32;
  output reg [SIZE-1:0] F;
  output reg CF;
  input [SIZE-1:0] A, B;
  input EN;
  
  always @(A, B, EN) begin
    if (EN) begin
    {CF,F} = A -B;
    end
    else begin
      F <= 32'bz;
      CF <= 1'bz;
    end
  end
endmodule

