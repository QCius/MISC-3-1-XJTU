`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/12/08 23:20:10
// Design Name: 
// Module Name: sim_CPU
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


module sim_CPU;
    reg clk, Reset;   
    wire [31:0] instruction;  
    wire [31:0] result; 
    
    CPU cpu(
        .clk(clk),
        .rst(Reset),
        .instruction1(instruction),
        .result( result)
    );
    
    always #5 clk = ~clk;   
    
    initial begin
        clk = 1;
        Reset = 0;
        #25;
        Reset = 1;  
        #30;
        Reset = 0; 
        #11000;       
        $stop; 
    end
endmodule
