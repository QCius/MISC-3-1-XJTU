`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/12/08 22:24:48
// Design Name: 
// Module Name: SigExt
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


module SigExt(input [15:0] original,
    input Alusrc,    // 0: Zero-extend; 1: Sign-extend.
    output reg [31:0] extended
    );
    
    always @(*) begin
        extended[15:0] <= original;   
        if(Alusrc == 0) begin  
            extended[31:16] <= 0;
        end
        else begin   
            if(original[15] == 0) extended[31:16] <= 0;
            else extended[31:16] <= 16'hFFFF;
        end
    end
    
endmodule
