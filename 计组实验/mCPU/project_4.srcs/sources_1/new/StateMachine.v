`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/12/09 15:50:08
// Design Name: 
// Module Name: StateMachine
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


module StateMachine (
    input wire clk,
    input wire rst,
    output reg [2:0] state
);

// 定义状态
parameter S0 = 3'b000;
parameter S1 = 3'b001;
parameter S2 = 3'b010;
parameter S3 = 3'b100;
parameter S4 = 3'b110;

// 状态变量
reg [2:0] next_state;

// 启动状态
initial begin
    state <= S0;
end

// 状态变化逻辑
always @(posedge clk) begin
    case (state)
        S0: next_state = S1;
        S1: next_state = S2;
        S2: next_state = S3;
        S3: next_state = S4;
        S4: next_state = S0;
        default: next_state = S0;
    endcase
end

// 更新状态
always @(posedge clk) begin
    state <= next_state;
end
always @(negedge rst) begin
    state <= S0;
end

endmodule

