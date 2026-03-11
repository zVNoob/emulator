#pragma once

#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<cmath>

namespace disassembler {

using namespace std ;
/**
 * @file Disassembler.cpp
 * @details This file contains the code for Disassembler of RISC-V machine code
 * @author Anudeep Rao Perala
 */
#define size_of_instruction_hex 8



/**
 * @brief This function converts given character to its Binary format
 * @param num Input
 * @return This function returns binary number format of the input character for concatenation
 */
string convert_to_binary(char num){
    string con ;
    if(num == '0') con = "0000" ;
    else if(num == '1') con = "0001" ;
    else if(num == '2') con = "0010" ;
    else if(num == '3') con = "0011" ;
    else if(num == '4') con = "0100" ;
    else if(num == '5') con = "0101" ;
    else if(num == '6') con = "0110" ;
    else if(num == '7') con = "0111" ;
    else if(num == '8') con = "1000" ;
    else if(num == '9') con = "1001" ;
    else if(num == 'a') con = "1010" ;
    else if(num == 'b') con = "1011" ;
    else if(num == 'c') con = "1100" ;
    else if(num == 'd') con = "1101" ;
    else if(num == 'e') con = "1110" ;
    else if(num == 'f') con = "1111" ;
    return con ;
}

/**
* @brief Converts the given Binary value to Decimal value
* @param token This is used to specify if the given binary value is in it's signed/ unsigned format
* @param binary Binary value
* @return Decimal value
*/
int binary_to_decimal(int token,string binary){
    //if token = 0 -> unsigned integer
    //if token = 1 -> signed integer
    int decimal = 0;
    int max_pow = (int)pow(2,binary.size());

    for(int i =0;i<binary.size();i++){
        int radical = binary.size()-1-i ;
        if(binary[i] == '0') decimal += 0 ;
        else decimal += (int)pow(2,radical);
    }

    if(token == 1 && binary[0]=='1') decimal -= max_pow ;
    return decimal ;
}

std::string get_reg_name(int reg_num){
  switch(reg_num){
    case 0:return "zero";
    case 1:return "ra";
    case 2:return "sp";
    case 3:return "gp";
    case 4:return "tp";
    case 5:return "t0";
    case 6:return "t1";
    case 7:return "t2";
    case 8:return "s0";
    case 9:return "s1";
    case 10:return "a0";
    case 11:return "a1";
    case 12:return "a2";
    case 13:return "a3";
    case 14:return "a4";
    case 15:return "a5";
    case 16:return "a6";
    case 17:return "a7";
    case 18:return "s2";
    case 19:return "s3";
    case 20:return "s4";
    case 21:return "s5";
    case 22:return "s6";
    case 23:return "s7";
    case 24:return "s8";
    case 25:return "s9";
    case 26:return "s10";
    case 27:return "s11";
    case 28:return "t3";
    case 29:return "t4";
    case 30:return "t5";
    case 31:return "t6";
  }
  return "x" + std::to_string(reg_num);
}

/**
 * @brief This function is used extract substring from string
 * @param parent Parent string from which substring is generated
 * @param start Starting index of element in parent string
 * @param end End index of element in parent string
 * @return Substring from parent string
 */
string sub_string(string parent,int start,int end){
  string sub ;
  for(int i = start;i<=end;i++) sub += parent[i] ;
  return sub ;
}



/**
 * @brief If the given machine code represents RISC-V R-format instruction this function generates RISC-V instruction from the machine code
 *
 * R-format instructions are: add, sub, xor, or, and, sll, srl, sra
 * @param stringBinary RISC-V instruction machine code from which the RISCV instruction is generated
 * @return RISC-V instruction
 */
string R_format(const string& stringBinary){
  string instruction ;

  string func7 = sub_string(stringBinary,0,6);
  string rs2 = sub_string(stringBinary,7,11);
  string rs1 = sub_string(stringBinary,12,16);
  string func3 = sub_string(stringBinary,17,19);
  string rd = sub_string(stringBinary,20,24);
  
  if(func3=="000" && func7 =="0000000") instruction += "add " ;
  else if(func3=="000" && func7 =="0100000") instruction += "sub " ;
  else if(func3=="100" && func7 =="0000000") instruction += "xor " ;
  else if(func3=="110" && func7 =="0000000") instruction += "or " ;
  else if(func3=="111" && func7 =="0000000") instruction += "and " ;
  else if(func3=="001" && func7 =="0000000") instruction += "sll " ;
  else if(func3=="101" && func7 =="0000000") instruction += "srl " ;
  else if(func3=="101" && func7 =="0100000") instruction += "sra " ;
  else {
    instruction = "Invalid instruction" ;
    return instruction ;
  }

  instruction += get_reg_name(binary_to_decimal(0,rd));
  instruction += ", " + get_reg_name(binary_to_decimal(0,rs1));
  instruction += ", " + get_reg_name(binary_to_decimal(0,rs2));

  return instruction ;
}

/**
 * @brief If the given machine code represents RISC-V I-format instruction this function generates RISC-V instruction from the machine code
 *
 * I-format instructions are: addi, xori, ori, andi, slli, srli, srai
 * @param stringBinary RISC-V instruction machine code from which the RISCV instruction is generated
 * @return RISC-V instruction
 */
string I_format_immediate(const string& stringBinary){
  string instruction ;
  
  string imm = sub_string(stringBinary,0,11);
  string rs1 = sub_string(stringBinary,12,16);
  string func3 = sub_string(stringBinary,17,19);
  string rd = sub_string(stringBinary,20,24);
  if(func3 == "000") instruction += "addi ";
  else if(func3 == "100") instruction += "xori ";
  else if(func3 == "110") instruction += "ori ";
  else if(func3 == "111") instruction += "andi ";
  else if(func3 == "001" && sub_string(imm,0,5) == "000000") {
    instruction += "slli ";
    imm = sub_string(imm,6,11);
  }
  else if(func3 == "101" && sub_string(imm,0,5) == "000000") {
    instruction += "srli ";
    imm = sub_string(imm,6,11);
  }
  else if(func3 == "101" && sub_string(imm,0,5) == "010000") {
    instruction += "srai ";
    imm = sub_string(imm,6,11);
  }
  else {
    instruction = "Invalid Instruction" ; 
    return instruction ;
  }
  instruction += get_reg_name(binary_to_decimal(0,rd));
  instruction += " , "+get_reg_name(binary_to_decimal(0,rs1));
  instruction += " , "+to_string(binary_to_decimal(0,imm));

  return instruction ;
}

/**
 * @brief If the given machine code represents RISC-V I-format load instruction type this function generates RISC-V instruction from the machine code
 *
 * I-format load instructions are: ld, lh, lw, lbu, lhu, lwu
 * @param stringBinary RISC-V instruction machine code from which the RISCV instruction is generated
 * @return RISC-V instruction
 */
string I_format_load(const string& stringBinary){
  string instruction ;
  string imm = sub_string(stringBinary,0,11);
  string rs1 = sub_string(stringBinary,12,16);
  string func3 = sub_string(stringBinary,17,19);
  string rd = sub_string(stringBinary,20,24);

  if(func3 == "000") instruction += "lb ";
  else if(func3 == "001") instruction += "lh ";
  else if(func3 == "010") instruction += "lw ";
  else if(func3 == "011") instruction += "ld ";
  else if(func3 == "100") instruction += "lbu ";
  else if(func3 == "101") instruction += "lhu " ;
  else if(func3 == "110") instruction += "lwu ";
  else {
    instruction = "Invalid instruction";
    return instruction ;
  }
  instruction += get_reg_name(binary_to_decimal(0,rd));
  instruction += " , "+to_string(binary_to_decimal(1,imm));
  instruction += "("+get_reg_name(binary_to_decimal(0,rs1));
  instruction += ")" ;

  return instruction ;

}

/**
 * @brief If the given machine code represents RISC-V S-format this function generates RISC-V instruction from the machine code
 *
 * S-format instructions are: sb, sh, sw, sd
 * @param stringBinary RISC-V instruction machine code from which the RISCV instruction is generated
 * @return RISC-V instruction
 */
string S_format(const string& stringBinary){
  string instruction ;
  string imm;
  imm += sub_string(stringBinary,0,6);
  string rs2 = sub_string(stringBinary,7,11);
  string rs1 = sub_string(stringBinary,12,16);
  string func3 = sub_string(stringBinary,17,19);
  imm += sub_string(stringBinary,20,24);
  if(func3 == "000") instruction += "sb ";
  else if(func3 == "001") instruction += "sh ";
  else if(func3 == "010") instruction += "sw ";
  else if(func3 == "011") instruction += "sd ";
  else {
    instruction = "Invalid Instruction" ;
    return instruction ;
  }

  instruction += get_reg_name(binary_to_decimal(0,rs2)) ;
  instruction += " , "+to_string(binary_to_decimal(1,imm));
  instruction += "("+get_reg_name(binary_to_decimal(0,rs1));
  instruction += ")";

  return instruction ;

}

/**
 * @brief If the given machine code represents RISC-V B-format this function generates RISC-V instruction from the machine code
 *
 * B-format instructions are: beq, bne, blt, bge, bltu, bgeu
 * @param stringBinary RISC-V instruction machine code from which the RISCV instruction is generated
 * @return RISC-V instruction
 */
string B_format(string stringBinary){
  string instruction ;

  string imm;
  imm += stringBinary[0] ;
  imm += stringBinary[24] ;
  imm += sub_string(stringBinary,1,6) ;
  imm += sub_string(stringBinary,20,23);
  imm += "0" ;

  string rs2 = sub_string(stringBinary,7,11);
  string rs1 = sub_string(stringBinary,12,16);
  string func3 = sub_string(stringBinary,17,19);

  if(func3 == "000") instruction += "beq ";
  else if(func3 == "001") instruction += "bne ";
  else if(func3 == "100") instruction += "blt ";
  else if(func3 == "101") instruction += "bge ";
  else if(func3 == "110") instruction += "bltu ";
  else if(func3 == "111") instruction += "bgeu ";

  instruction += get_reg_name(binary_to_decimal(0,rs1));
  instruction += " , "+get_reg_name(binary_to_decimal(0,rs2));
  instruction += " , "+to_string(binary_to_decimal(1,imm));
  return instruction ;
}

/**
 * @brief If the given machine code represents RISC-V jal format this function generates RISC-V instruction from the machine code
 *
 * @param stringBinary RISC-V instruction machine code from which the RISCV instruction is generated
 * @return jal RISC-V instruction
 */
string jal(string stringBinary){
  string instruction ;
  string imm ;
  imm += stringBinary[0] ;
  imm += sub_string(stringBinary,12,19) ;
  imm += stringBinary[11];
  imm += sub_string(stringBinary,1,10);
  imm += "0" ;

  string rd = sub_string(stringBinary,20,24);
  instruction +="jal " ;
  instruction +=get_reg_name(binary_to_decimal(0,rd)) ;
  instruction += " , "+to_string(binary_to_decimal(1,imm));
  return instruction ;
}

/**
 * @brief If the given machine code represents RISC-V jalr format this function generates RISC-V instruction from the machine code
 *
 * @param stringBinary RISC-V instruction machine code from which the RISCV instruction is generated
 * @return jalr RISC-V instruction
 */
string jalr(const string& stringBinary){
  string instruction ;
  string imm = sub_string(stringBinary,0,11) ;
  string rs1 = sub_string(stringBinary,12,16);
  string func3 = sub_string(stringBinary,17,19);
  string rd = sub_string(stringBinary,20,24);
  instruction += "jalr ";
  instruction += get_reg_name(binary_to_decimal(0,rd));
  instruction += ", "+get_reg_name(binary_to_decimal(0,rs1));
  instruction += " , "+to_string(binary_to_decimal(1,imm)) ;
  return instruction ;
}

/**
 * @brief If the given machine code represents RISC-V lui format this function generates RISC-V instruction from the machine code
 *
 * @param stringBinary RISC-V instruction machine code from which the RISCV instruction is generated
 * @return lui RISC-V instruction
 */
string lui(const string& stringBinary){
  string instruction ;
  string imm = sub_string(stringBinary,0,19);
  string rd = sub_string(stringBinary,20,24);
  string hex_imm = "0x";
  hex_imm += to_string(binary_to_decimal(0,sub_string(stringBinary,0,3)));
  hex_imm += to_string(binary_to_decimal(0,sub_string(stringBinary,4,7)));
  hex_imm += to_string(binary_to_decimal(0,sub_string(stringBinary,8,11)));
  hex_imm += to_string(binary_to_decimal(0,sub_string(stringBinary,12,15)));
  hex_imm += to_string(binary_to_decimal(0,sub_string(stringBinary,16,19)));
  instruction = "lui "+get_reg_name(binary_to_decimal(0,rd))+" , "+hex_imm ;
  return instruction ;
}

/**
 *
 * @brief This function conversion of given machine code to binary value is done and based on opcode the corresponding instruction type is found and is processed accordingly
 *
 * @param total_instructions Total number of instructions given as input
 * @param instruction This is machine code of the RISC-V instruction
 * @param inst_no States the instruction number
 * @return Disassembled RISC-V instruction
 */
string disassembler(string instruction){
  string disassem_instruction ;

    string instruction_binary ;

    for(int i = 0;i<size_of_instruction_hex;i++) instruction_binary += convert_to_binary(instruction[i]);
  
    string opcode = sub_string(instruction_binary,25,31);
    if(opcode == "0110011") disassem_instruction = R_format(instruction_binary);
    else if(opcode == "0010011") disassem_instruction = I_format_immediate(instruction_binary);
    else if(opcode == "0000011") disassem_instruction = I_format_load(instruction_binary);
    else if(opcode == "0100011") disassem_instruction = S_format(instruction_binary);
    else if(opcode == "1100011") disassem_instruction = B_format(instruction_binary);
    else if(opcode == "1101111") disassem_instruction = jal(instruction_binary); //This is J format
    else if(opcode == "1100111") disassem_instruction = jalr(instruction_binary); //This is I format
    else if(opcode == "0110111") disassem_instruction = lui(instruction_binary) ; //This is U format
    else if(opcode == "1110011") disassem_instruction = "system";
    else disassem_instruction = "Invalid instruction"; 

    return disassem_instruction ;
}

}
