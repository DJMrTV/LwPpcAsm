#include <ios>
#include<lib.h>
#include<gtest/gtest.h>
#include<iostream>

TEST(CoreGeneration, Instruction){
  auto instruction = Instruction(OpCode::Branch);
  auto instruction_data = (uint32_t) instruction;

  ASSERT_EQ(instruction_data, 0x48000000);
}


TEST(CoreGeneration, Branch){
  auto instruction = Branch::from_to(0x028CC534, 0x028CC450);
  uint32_t instruction_data = instruction;
  auto instruction_manual_offset = Branch(-0xE4);
  uint32_t instruction_manual_offset_data = instruction_manual_offset;

  ASSERT_EQ(instruction_manual_offset_data, instruction_data);

  std::cout << std::hex << instruction_data << std::endl;

  //std::cout << instruction.branch_data.aa << std::endl;
  //std::cout << instruction.branch_data.lk << std::endl;

  //ASSERT_EQ(instruction_data, 0x4280ff1c);
}


TEST(CoreGeneration, OrImmediate){
    auto instruction = OrImmediate(1, 1, 0x10);
    uint32_t instruction_data = instruction;

    std::cout << std::hex << instruction_data << std::endl;

    //std::cout << instruction.branch_data.aa << std::endl;
    //std::cout << instruction.branch_data.lk << std::endl;

    ASSERT_EQ(instruction_data, 0x60210010);
}


TEST(CoreGeneration, LoadImmediateShifted){
    auto instruction = LoadImmediateShifted(10, 0x10);
    uint32_t instruction_data = instruction;

    std::cout << std::hex << instruction_data << std::endl;

    //std::cout << instruction.branch_data.aa << std::endl;
    //std::cout << instruction.branch_data.lk << std::endl;

    ASSERT_EQ(instruction_data, 0x3d400010);
}

TEST(CoreGeneration, MoveToSpecialPurposeRegister){
    auto instruction = MoveToSpecialPurposeRegister(10, CTR);
    uint32_t instruction_data = instruction;

    std::cout << std::hex << instruction_data << std::endl;

    //std::cout << instruction.branch_data.aa << std::endl;
    //std::cout << instruction.branch_data.lk << std::endl;

    ASSERT_EQ(instruction_data, 0x7d4903a6);
}

TEST(CoreGeneration, BranchCounterRegister){
    auto instruction = BranchCounterRegister();
    uint32_t instruction_data = instruction;

    std::cout << std::hex << instruction_data << std::endl;

    //std::cout << instruction.branch_data.aa << std::endl;
    //std::cout << instruction.branch_data.lk << std::endl;

    ASSERT_EQ(instruction_data,  0x4e800420);
}



