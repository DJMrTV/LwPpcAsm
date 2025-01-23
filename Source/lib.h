#include <cassert>
#include<cstdint>

enum class OpCode : uint8_t {
    AddImmediate = 14,
    AddImmediateShifted = 15,
    Branch = 18,
    BranchConditionalToSpecialPurposeRegister = 19,
    OrImmediate = 24,
    MoveToSpecialPurposeRegister = 31,
};

struct OpBranchData {
    OpCode opcode: 6;
    int li: 24;
    unsigned int aa: 1;
    unsigned int lk: 1;
};


struct Instruction {
    union {
    public:
        uint32_t raw_data;
    };

    constexpr Instruction(OpCode opcode) {
        this->raw_data = 0;
        /*auto shifted_opcode = ((uint8_t)opcode) << 26;
        this->raw_data |= shifted_opcode */

        write_data((uint8_t) opcode, 6, 0);

    }


    constexpr operator Instruction() {
        return *this;
    }

    constexpr operator uint32_t() {
        return raw_data;
    }

    constexpr operator uint8_t *() {
        return (uint8_t *) this;
    }

    constexpr operator char *() {
        return (char *) this;
    }

protected:
    constexpr void write_data(uint32_t value, uint8_t length, uint8_t offset) {
        const uint32_t rightmost_bound_from_left = offset + length;
        const uint32_t rightmost_bound = 32 - rightmost_bound_from_left;

        assert(rightmost_bound < 32);

        //value must be contained inside of this mask to not clip the bounds
        const auto contained_mask = ~(0xFFFFFFFF << length);
        const auto masked_value = (contained_mask & value);
        assert(masked_value == value);

        raw_data |= value << rightmost_bound;
    }

    constexpr void write_data_signed(int32_t value, uint8_t length, uint8_t offset) {
        const bool is_negative = (value < 0);

        uint32_t normal_data = (uint32_t) value;

        if (is_negative) {
            auto mask = ~(0xFFFFFFFF << (length - 1));
            normal_data &= mask;
        }

        //write the sign bit
        write_data(is_negative, 1, offset);
        write_data(normal_data, length - 1, offset + 1);
    }
};

static constexpr uint8_t LI_OFFSET = 6;
static constexpr uint8_t AA_OFFSET = 30;
static constexpr uint8_t LK_OFFSET = 31;

static_assert(sizeof(Instruction) == 4);
#define BRANCH_INSTRUCTION_VERSION(name, aa_val, lk_val)\
struct name: public Instruction{                        \
public:\
  constexpr name(): Instruction(OpCode::Branch){\
    write_data(aa_val,1,AA_OFFSET);\
    write_data(lk_val,1,LK_OFFSET);\
  }\
  static constexpr name from_to(uint32_t from, uint32_t to){\
    int32_t offset = (int32_t)to - (int32_t)from;\
    return name(offset);\
  }\
  constexpr name(int32_t offset): name(){\
    assert((offset % 0x4) == 0);\
    offset = offset / 4;\
    assert(offset < 0x7FFFFF);\
    assert(offset > -0x7FFFFF);\
    write_data_signed(offset,24,LI_OFFSET);\
  }\
}

BRANCH_INSTRUCTION_VERSION(Branch, 0, 0);

BRANCH_INSTRUCTION_VERSION(BranchAbsolute, 1, 0);

BRANCH_INSTRUCTION_VERSION(BranchLink, 0, 1);

BRANCH_INSTRUCTION_VERSION(BranchLinkAbsolute, 1, 1);

#undef BRANCH_INSTRUCTION_VERSION


struct OrImmediate : public Instruction {
private:
    static constexpr uint8_t REG_A_OFFSET = 6;
    static constexpr uint8_t REG_B_OFFSET = 11;
    static constexpr uint8_t IMMEDIATE_OFFSET = 16;
public:
    constexpr OrImmediate(uint8_t register_a, uint8_t register_b, uint16_t immediate_val) : Instruction(
            OpCode::OrImmediate) {
        write_data(register_a, 5, REG_A_OFFSET);
        write_data(register_b, 5, REG_B_OFFSET);
        write_data(immediate_val, 16, IMMEDIATE_OFFSET);
    }
};

struct NOP : public OrImmediate {
    constexpr NOP() : OrImmediate(0, 0, 0) {}
};

struct AddImmediateShifted : public Instruction {
private:
    static constexpr uint8_t REG_A_OFFSET = 6;
    static constexpr uint8_t REG_B_OFFSET = 11;
    static constexpr uint8_t IMMEDIATE_OFFSET = 16;
public:
    constexpr AddImmediateShifted(uint8_t register_a, uint8_t register_b, uint16_t immediate_val) : Instruction(
            OpCode::AddImmediateShifted) {
        write_data(register_a, 5, REG_A_OFFSET);
        write_data(register_b, 5, REG_B_OFFSET);
        write_data(immediate_val, 16, IMMEDIATE_OFFSET);
    }
};

struct LoadImmediateShifted : public AddImmediateShifted {
    constexpr LoadImmediateShifted(uint8_t register_a, uint16_t immediate_val) :
            AddImmediateShifted(register_a, 0, immediate_val) {}
};

struct AddImmediate : public Instruction {
private:
    static constexpr uint8_t REG_A_OFFSET = 6;
    static constexpr uint8_t REG_B_OFFSET = 11;
    static constexpr uint8_t IMMEDIATE_OFFSET = 16;
public:
    constexpr AddImmediate(uint8_t register_a, uint8_t register_b, uint16_t immediate_val) : Instruction(
            OpCode::AddImmediate) {
        write_data(register_a, 5, REG_A_OFFSET);
        write_data(register_b, 5, REG_B_OFFSET);
        write_data(immediate_val, 16, IMMEDIATE_OFFSET);
    }
};

struct LoadImmediate : public AddImmediate {
    constexpr LoadImmediate(uint8_t register_a, uint16_t immediate_val) :
            AddImmediate(register_a, 0, immediate_val) {}
};



constexpr uint8_t CTR = 9;
constexpr uint8_t LR = 8;
constexpr uint8_t XER = 1;

struct MoveToSpecialPurposeRegister : public Instruction {
private:
    static constexpr uint8_t REG_A_OFFSET = 6;
    static constexpr uint8_t SPR_OFFSET = 11;
    static constexpr uint8_t DATA_1_OFFSET = 21;
    static constexpr uint8_t DATA_2_OFFSET = 30;
public:
    constexpr MoveToSpecialPurposeRegister(uint8_t register_a, uint8_t spr) : Instruction(
            OpCode::MoveToSpecialPurposeRegister) {
        write_data(register_a, 5, REG_A_OFFSET);
        auto spr_swapped = ((spr & 0b11111) << 5) |
                           ((spr >> 5) & 0b11111);

        write_data(spr_swapped, 10, SPR_OFFSET);
        write_data(467, 10, DATA_1_OFFSET);
        write_data(0, 1, DATA_2_OFFSET);
    }
};

//im gonna have to rework this at some point to actually factor in the instruction this derives from for now though im just not gonna do that
struct BranchCounterRegister : public Instruction {
private:
    static constexpr uint8_t CONDITION_OFFSET = 6;
    static constexpr uint8_t BIT_OFFSET = 11;
    static constexpr uint8_t BLANK = 16;
    static constexpr uint8_t MAGIC = 21;
    static constexpr uint8_t LINK = 31;
public:
    constexpr BranchCounterRegister() : Instruction(OpCode::BranchConditionalToSpecialPurposeRegister) {
        write_data(0b10100, 5, CONDITION_OFFSET);
        write_data(0b00000, 5, BIT_OFFSET);
        write_data(0b00000, 5, BLANK);
        write_data(528, 10, MAGIC);
        write_data(0, 1, LINK);
    }
};
