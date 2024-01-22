module CPU(
    input wire clk,    // 时钟
    input wire rst,    // 复位
    input  [31:0] instruction1,  // 输入指令
    output reg [31:0] result  // 输出结果
);
 
reg [31:0] registers [0:31];
integer i;
initial begin
    for(i=0;i<32;i = i + 1)begin
        registers[i] = 32'b0;
    end
    result = 32'b0;
 end

wire MemToReg, MemWrite,Branch,ALUSrc,RegDst,RegWrite,Jump,Zero;
wire [4:0] ALUControl;
reg [5:0] opcode;
reg [4:0] rs, rt, rd;
wire [31:0] immediate;
wire [2:0] state;
reg [31:0] memory [0:1023];
reg [31:0] pc;
wire CF;
wire [31:0] imm_ext;
wire [31:0] Alu_in;
wire [31:0] new_result;
wire [31:0] Dm_out;   
    always @(posedge clk or posedge rst) begin
        //pc_b <= pc;
        if(rst) begin 
            pc <= 32'b0;
            for ( i = 0; i < 32; i = i + 1) begin
                registers[i] <= 32'b0;
            end
         end
        else if (state == 3'b110)begin
            if(Jump == 1) pc <={ pc[31:28],instruction1[26:0]};
            else if(Branch == 1 && new_result == 0) pc <= pc + imm_ext;
            else  pc <= pc +4;
        end
    end
    
    // 立即数生成
assign immediate = instruction1[15:0];
// 控制信号生成
always @* begin
    // 从指令中提取字段
    opcode = instruction1[31:26];
    rs = instruction1[25:21];
    rt = instruction1[20:16];
    rd = instruction1[15:11];    
end

StateMachine sm(clk,rst,state);
IM Im(.clk(clk),.rst(rst),.state(state),.Addr(pc),.instruction(instruction1));
Controller cu(clk,state,instruction1,MemToReg, MemWrite, Branch,ALUSrc,RegDst,RegWrite,Jump,ALUControl);
SigExt sigext(immediate,ALUSrc,imm_ext);
MUX2 mux2_alu(ALUSrc,registers[rt],imm_ext,Alu_in);
ALU alu(.F(new_result),.CF(CF),.A(registers[rs]),.B(Alu_in),.OP(ALUControl));
DataMemory dm(clk,state,new_result,registers[rt],MemToReg,MemWrite,Dm_out);

always @(posedge clk) begin
    if(state == 3'b110)begin
    if (RegWrite && new_result)  begin
        // 写入寄存器
        if(opcode != 6'b000000)
        registers[rt] <= MemToReg ? Dm_out :new_result;
        else
        registers[rd] <= MemToReg ? Dm_out : new_result;
    end
    end
end

always @(posedge clk or posedge rst) begin
    if (rst) begin
        // 复位时初始化
        result <= 32'b0;
    end else begin
        // 输出结果
        result <= MemToReg ? Dm_out :new_result;
    end
end

endmodule
