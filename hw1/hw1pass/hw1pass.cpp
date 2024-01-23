#include "llvm/Analysis/BlockFrequencyInfo.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/CFG.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Format.h"

#include  <iostream>

using namespace llvm;

namespace {

struct HW1Pass : public PassInfoMixin<HW1Pass> {

  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    // These variables contain the results of some analysis passes.
    // Go through the documentation to see how they can be used.
    llvm::BlockFrequencyAnalysis::Result &bfi = FAM.getResult<BlockFrequencyAnalysis>(F);
    llvm::BranchProbabilityAnalysis::Result &bpi = FAM.getResult<BranchProbabilityAnalysis>(F);

    // Add your code here
    int inst_cnt = 0;
    int biased_br_cnt, unbiased_br_cnt, int_alu_cnt, fp_alu_cnt, mem_cnt, others_cnt = 0;
    int* cat_list[6] = {&int_alu_cnt, &fp_alu_cnt, &mem_cnt, &biased_br_cnt, &unbiased_br_cnt, &others_cnt};

    // TODO: loop over BBs
    for (BasicBlock& BB : F) {
      // int bb_biased_br_cnt, bb_unbiased_br_cnt, bb_int_alu_cnt, bb_fp_alu_cnt, bb_mem_cnt, bb_others_cnt = 0;
      for (Instruction& I : BB) {
        unsigned int opcode = I.getOpcode();
        if (isBr(opcode)) {
          if (isBiasedBr(BB, bpi)) {
            biased_br_cnt += bfi.getBlockFreq(&BB).getFrequency();
          } else {
            unbiased_br_cnt += bfi.getBlockFreq(&BB).getFrequency();
          }
        } else if (isIALU(opcode)) {
          int_alu_cnt += bfi.getBlockFreq(&BB).getFrequency();
        } else if (isFPALU(opcode)) {
          fp_alu_cnt += bfi.getBlockFreq(&BB).getFrequency();
        } else if (isMem(opcode)) {
          mem_cnt += bfi.getBlockFreq(&BB).getFrequency();
        } else {
          others_cnt += bfi.getBlockFreq(&BB).getFrequency();
        }
      }

      inst_cnt += BB.size() * bfi.getBlockFreq(&BB).getFrequency();
    }
    // std::cout << F.getName().str() << std::endl;
    // std::cout << inst_cnt << std::endl;
    // std::cout << int_alu_cnt << std::endl;
    // std::cout << fp_alu_cnt << std::endl;
    // std::cout << mem_cnt << std::endl;
    // std::cout << biased_br_cnt << std::endl;
    // std::cout << unbiased_br_cnt << std::endl;
    // std::cout << others_cnt << std::endl;
    errs() << F.getName();
    errs() << ", " << inst_cnt;
    for (int* cat : cat_list) { // iterate all categories
      // std::cout << *cat << std::endl;
      if (inst_cnt == 0) errs() << ", " << format("%.3f", 0.);
      else errs() << ", " << format("%.3f", (float) ( (float) (*cat) / (float) inst_cnt));
    }
    errs() << "\n";
    return PreservedAnalyses::all();
  }

  bool isBr(unsigned int opcode) {
    return (
      opcode == llvm::Instruction::Br
      || opcode == llvm::Instruction::Switch
      || opcode == llvm::Instruction::IndirectBr
    );
  }

  bool isIALU(unsigned int opcode) {
    return (opcode == llvm::Instruction::Add 
    || opcode == llvm::Instruction::Sub 
    || opcode == llvm::Instruction::Mul 
    || opcode == llvm::Instruction::UDiv 
    || opcode == llvm::Instruction::SDiv
    || opcode == llvm::Instruction::URem
    || opcode == llvm::Instruction::Shl
    || opcode == llvm::Instruction::LShr
    || opcode == llvm::Instruction::AShr
    || opcode == llvm::Instruction::And
    || opcode == llvm::Instruction::Or
    || opcode == llvm::Instruction::Xor
    || opcode == llvm::Instruction::ICmp
    || opcode == llvm::Instruction::SRem
    );
  }

  bool isFPALU(unsigned int opcode) {
    return (opcode == llvm::Instruction::FAdd
    || opcode == llvm::Instruction::FSub 
    || opcode == llvm::Instruction::FMul
    || opcode == llvm::Instruction::FDiv
    || opcode == llvm::Instruction::FRem
    || opcode == llvm::Instruction::FCmp
    );
  }

  bool isMem(unsigned int opcode) {
    return (
      opcode == llvm::Instruction::Alloca
      || opcode == llvm::Instruction::Load
      || opcode == llvm::Instruction::Store
      || opcode == llvm::Instruction::GetElementPtr
      || opcode == llvm::Instruction::Fence
      || opcode == llvm::Instruction::AtomicCmpXchg
      || opcode == llvm::Instruction::AtomicRMW
    );
  }

  bool isBiasedBr(llvm::BasicBlock &BB, llvm::BranchProbabilityAnalysis::Result &bpi) {
    for (BasicBlock *SBB : successors(&BB)) {
      if (bpi.isEdgeHot(&BB, SBB)) {
        // biased br
        return true;
      }
    }
    // unbiased br
    return false;
  }
};
}

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "HW1Pass", "v0.1",
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](StringRef Name, FunctionPassManager &FPM,
        ArrayRef<PassBuilder::PipelineElement>) {
          if(Name == "hw1"){
            FPM.addPass(HW1Pass());
            return true;
          }
          return false;
        }
      );
    }
  };
}