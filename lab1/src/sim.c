#include<stdio.h>
#include "shell.h"

void process_instruction()
{
   /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
     
    uint32_t instruction=mem_read_32(CURRENT_STATE.PC);//从内存中读取指令
    uint32_t opcode=(instruction>>26)&0x3F;//提取指令的操作码字段
    uint32_t rs=(instruction>>21)&0x1F;
    uint32_t rt=(instruction>>16)&0x1F;
    uint32_t rd=(instruction>>11)&0x1F;
    uint32_t shamt=(instruction>>6)&0x1F;
    uint32_t funct=instruction&0x3F;
    int16_t imme=(int16_t)(instruction&0xFFFF);//立即数
    int16_t offset=(int16_t)(instruction&0xFFFF);//偏移量   
    uint32_t lodadd=CURRENT_STATE.REGS[rs]+offset;//计算加载地址   
    uint32_t stradd=CURRENT_STATE.REGS[rs]+offset;//计算存储地址
    uint32_t datastr=CURRENT_STATE.REGS[rt];//获取要存储的数据   
    uint32_t taradd=CURRENT_STATE.PC+(offset<<2);//计算目标地址
    uint32_t movadd=(instruction&0x03FFFFFF)<<2;//提取跳转目标地址（26位地址向左移2位）
    
    
    switch (opcode) {
        case 0x00://R型指令
            switch (funct) {  
                case 0x00://SLL
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]<<shamt;
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x02://SRL
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]>>shamt;
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x03://SRA
                    NEXT_STATE.REGS[rd]=(int32_t)CURRENT_STATE.REGS[rt]>>shamt;
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x04://SLLV
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]<<(CURRENT_STATE.REGS[rs]&0x1F);
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x06://SRLV
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]>>(CURRENT_STATE.REGS[rs]&0x1F);
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x07://SRAV
                    NEXT_STATE.REGS[rd]=(int32_t)CURRENT_STATE.REGS[rt]>>(CURRENT_STATE.REGS[rs]&0x1F);
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x08://JR
                    NEXT_STATE.PC=CURRENT_STATE.REGS[rs];
                    break;
                    
                case 0x09://JALR
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.PC+4;
                    NEXT_STATE.PC=CURRENT_STATE.REGS[rs];
                    break; 
                
                case 0x20://ADD
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]+CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x21://ADDI
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]+CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x22://SUB
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]-CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x23://SUBU
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]-CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x24://AND
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]&CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x25://OR
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]|CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x26://XOR
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]^CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC+=4;
                    break;
                  
                case 0x27://NOR
                    NEXT_STATE.REGS[rd]=~(CURRENT_STATE.REGS[rs]|CURRENT_STATE.REGS[rt]);
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x2A://SLT
                    NEXT_STATE.REGS[rd]=(CURRENT_STATE.REGS[rs]<CURRENT_STATE.REGS[rt])?1:0;
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x2B://SLTU
                    NEXT_STATE.REGS[rd]=(CURRENT_STATE.REGS[rs]<CURRENT_STATE.REGS[rt])?1:0;
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x18://MULT
                    {
                        int64_t result=(int64_t)CURRENT_STATE.REGS[rs]*(int64_t)CURRENT_STATE.REGS[rt];
                        NEXT_STATE.HI=(result>>32)&0xFFFFFFFF;
                        NEXT_STATE.LO=result&0xFFFFFFFF;
                        NEXT_STATE.PC+=4;
                    }
                    break;
                    
                case 0x10://MFHI
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.HI;
                    NEXT_STATE.PC+=4;
                    break;                                  
                    
                case 0x11://MTHI
                    NEXT_STATE.HI=CURRENT_STATE.REGS[rs];
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x13://MTLO
                    NEXT_STATE.LO=CURRENT_STATE.REGS[rs];
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x12://MFLO
                    NEXT_STATE.REGS[rd]=CURRENT_STATE.LO;
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x19://MULTU
                    {
                        uint64_t result=(uint64_t)CURRENT_STATE.REGS[rs] * (uint64_t)CURRENT_STATE.REGS[rt];
                        NEXT_STATE.HI=(result>>32)&0xFFFFFFFF;
                        NEXT_STATE.LO=result&0xFFFFFFFF;
                        NEXT_STATE.PC+=4;
                    }
                    break;
                    
                case 0x1A://DIV
                    if (CURRENT_STATE.REGS[rt]!=0) {
                        int32_t dividend=CURRENT_STATE.REGS[rs];
                        int32_t divisor=CURRENT_STATE.REGS[rt];
                        NEXT_STATE.LO=dividend/divisor;
                        NEXT_STATE.HI=dividend%divisor;
                    } 
                    else {
                        NEXT_STATE.LO = 0; 
                        NEXT_STATE.HI = 0;
                       //处理除数为零的情况
                    }
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x1B://DIVU
                    if (CURRENT_STATE.REGS[rt]!=0) {
                        uint32_t dividend=CURRENT_STATE.REGS[rs];
                        uint32_t divisor=CURRENT_STATE.REGS[rt];
                        NEXT_STATE.LO=dividend/divisor;
                        NEXT_STATE.HI=dividend%divisor;
                    } 
                    else {
                        NEXT_STATE.LO = 0; 
                        NEXT_STATE.HI = 0;
                       //处理除数为零的情况
                    }
                    NEXT_STATE.PC+=4;
                    break;
                    
                case 0x0C://SYSCALL
                   //解码SYSCALL指令
                    uint32_t v0=CURRENT_STATE.REGS[2];//$v0寄存器的值
                    if (v0==0x0A) {
                       //如果$v0寄存器的值为0x0A，停止模拟器的运行
                        RUN_BIT=FALSE;
                    }
                    NEXT_STATE.PC+=4;
                    break;
                    
                default://未被定义的功能码
                    NEXT_STATE.PC+=4;
                    break;
            }           
            break;

        case 0x01://I型指令
            switch (funct) {
                case 0x00://BLTZ
                    if ((int32_t)CURRENT_STATE.REGS[rs]<0) {
                        NEXT_STATE.PC=taradd;
                    } 
                    else {
                        NEXT_STATE.PC+=4;
                    }
                    break;
                case 0x01://BGEZ
                    if ((int32_t)CURRENT_STATE.REGS[rs]>=0) {
                        NEXT_STATE.PC=taradd;
                    } 
                    else {
                        NEXT_STATE.PC+=4;
                    }
                    break;
                case 0x10://BLTZAL
                    if ((int32_t)CURRENT_STATE.REGS[rs]<0) {
                        NEXT_STATE.REGS[31]=CURRENT_STATE.PC+4;
                        NEXT_STATE.PC=taradd;
                    } 
                    else {
                        NEXT_STATE.PC+=4;
                    }
                    break;
                case 0x11://BGEZAL
                    if ((int32_t)CURRENT_STATE.REGS[rs]>=0) {
                        NEXT_STATE.REGS[31]=CURRENT_STATE.PC+4;
                        NEXT_STATE.PC=taradd;
                    } 
                    else {
                        NEXT_STATE.PC+=4;
                    }
                    break;
                default://未被定义的功能码
                    NEXT_STATE.PC+=4;
                    break;
            }
            break;
        case 0x02://J                                  
            NEXT_STATE.PC=(CURRENT_STATE.PC&0xF0000000)|movadd;
            break;

        case 0x03://JAL
            NEXT_STATE.REGS[31]=CURRENT_STATE.PC+4;
            NEXT_STATE.PC=(CURRENT_STATE.PC&0xF0000000)|movadd;
            break;   
            
        case 0x04://BEQ
            if (CURRENT_STATE.REGS[rs]==CURRENT_STATE.REGS[rt]) {
                NEXT_STATE.PC+=(offset<<2);
            } 
            else {
                NEXT_STATE.PC+=4;
            }
            break;

        case 0x05://BNE
            if (CURRENT_STATE.REGS[rs]!=CURRENT_STATE.REGS[rt]) {
                NEXT_STATE.PC+=(offset<<2);
            } 
            else {
                NEXT_STATE.PC+=4;
            }
            break;

        case 0x06://BLEZ
            if ((int32_t)CURRENT_STATE.REGS[rs]<=0) {
                NEXT_STATE.PC+=(offset<<2);
            } 
            else {
                NEXT_STATE.PC+=4;
            }
            break;

        case 0x07://BGTZ
            if ((int32_t)CURRENT_STATE.REGS[rs]>0) {
                NEXT_STATE.PC+=(offset<<2);
            } 
            else {
                NEXT_STATE.PC+=4;
            }
            break;
            
        case 0x08://ADDI
            NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]+imme;
            NEXT_STATE.PC+=4;
            break;
                    
        case 0x09://ADDIU
            NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]+imme;
            NEXT_STATE.PC+=4;
            break;
                    
        case 0x0A://SLTI
            NEXT_STATE.REGS[rt]=(int32_t)CURRENT_STATE.REGS[rs]<imme?1:0;
            NEXT_STATE.PC+=4;
            break;
                    
        case 0x0B://SLTIU
            NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]<(uint32_t)imme?1:0;
            NEXT_STATE.PC+=4;
            break;
                    
        case 0x0C://ANDI
            NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]&(uint32_t)imme;
            NEXT_STATE.PC+=4;
            break;
                    
        case 0x0D://ORI
            NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]|(uint32_t)imme;
            NEXT_STATE.PC+=4;
            break;
                    
        case 0x0E://XORI
            NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]^(uint32_t)imme;
            NEXT_STATE.PC+=4;
            break;
                    
        case 0x0F://LUI
            NEXT_STATE.REGS[rt]=(uint32_t)(imme<<16);
            NEXT_STATE.PC+=4;
            break;
        
        case 0x20://LB
            NEXT_STATE.REGS[rt]=(int32_t)(int8_t)mem_read_32(lodadd);
            NEXT_STATE.PC+=4;
            break;
            
        case 0x21://LH
            NEXT_STATE.REGS[rt]=(int32_t)(int16_t)mem_read_32(lodadd);
            NEXT_STATE.PC+=4;
            break;
            
        case 0x23://LW
            NEXT_STATE.REGS[rt]=mem_read_32(lodadd);
            NEXT_STATE.PC+=4;
            break;
            
        case 0x24://LBU
            NEXT_STATE.REGS[rt]=(uint32_t)(uint8_t)mem_read_32(lodadd);
            NEXT_STATE.PC+=4;
            break;
            
        case 0x25://LHU
            NEXT_STATE.REGS[rt]=(uint32_t)(uint16_t)mem_read_32(lodadd);
            NEXT_STATE.PC+=4;
            break;
            
        case 0x28://SB
            mem_write_32(stradd, (datastr&0xFF));
            NEXT_STATE.PC+=4;
            break;
            
        case 0x29://SH
            mem_write_32(stradd, (datastr&0xFFFF));
            NEXT_STATE.PC+=4;
            break;
            
        case 0x2B://SW
            mem_write_32(stradd, datastr);
            NEXT_STATE.PC+=4;
            break;   
        
        default://未被定义的操作码
            NEXT_STATE.PC+=4;
            break;
    }
}

