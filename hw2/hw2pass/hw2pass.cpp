//===-- Frequent Path Loop Invariant Code Motion Pass --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------===//
//
// EECS583 W24 - This pass can be used as a template for your FPLICM homework
//               assignment.
//               The passes get registered as "fplicm-correctness" and
//               "fplicm-performance".
//
//
////===-------------------------------------------------------------------===//
#include "llvm/Analysis/BlockFrequencyInfo.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopIterator.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/LoopUtils.h"

/* *******Implementation Starts Here******* */
// You can include more Header files here

#include <vector>
#include <set>

/* *******Implementation Ends Here******* */

using namespace llvm;

namespace {
  struct HW2CorrectnessPass : public PassInfoMixin<HW2CorrectnessPass> {

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
      llvm::BlockFrequencyAnalysis::Result &bfi = FAM.getResult<BlockFrequencyAnalysis>(F);
      llvm::BranchProbabilityAnalysis::Result &bpi = FAM.getResult<BranchProbabilityAnalysis>(F);
      llvm::LoopAnalysis::Result &li = FAM.getResult<LoopAnalysis>(F);
      /* *******Implementation Starts Here******* */

      std::set<llvm::LoadInst*> freqLoads;
      std::set<llvm::StoreInst*> freqStores;
      std::set<llvm::StoreInst*> inFreqStores;
      std::set<llvm::BasicBlock*> freqPath;
      std::set<llvm::BasicBlock*> inFreqPath;
      std::set<llvm::LoadInst*> invariantLoads;

      llvm::BasicBlock* preHeader;
      
      // iterate over all loops
      for (llvm::Loop *L : li) {
        llvm::errs() << "Found a loop:\n";

        // the header BB of the loop
        llvm::BasicBlock *header = L->getHeader();
        preHeader = L->getLoopPreheader();
        llvm::errs() << "Loop header:\n " << *header << "\n";
        
        // iterate all the BB in all the loops and find the frequent path.
        llvm:: BasicBlock *BB = header;

        while (BB) {
          llvm::BasicBlock *nextBB;
          freqPath.insert(BB);
          // llvm::errs() << "Basic Block:\n " << *BB << "\n";

          // Analyze branch probabilities for the current block
          llvm::BranchInst *branchInst = llvm::dyn_cast<llvm::BranchInst>(BB->getTerminator());
          // check the terminator's edges and pick the most frequent (above 80% one)
          if (branchInst) {
            llvm::errs() << " br Inst:\n " << *branchInst << "\n";
            for (llvm::BasicBlock *succBB : successors(BB)) {
              if (branchInst->isConditional()) {
                if (bpi.getEdgeProbability(BB, succBB) >= llvm::BranchProbability(80, 100)) {
                  llvm::errs() << "Br prob: " << bpi.getEdgeProbability(BB, succBB) << "\n";
                  nextBB = succBB;
                  break;
                }
              } else {
                nextBB = succBB;
                break;
              }
            } 
          } else {
            break;
          }
          if (nextBB == header) break;
          BB = nextBB;
        }
        
        llvm::errs() << "\n\nBasic Block in Freq pth: \n";
        for (llvm::BasicBlock *BB : freqPath) {
          llvm::errs() << BB << "\n";
        }

        llvm::errs() << "\n\nBasic Block NOT in Freq pth: \n";
        for (llvm::BasicBlock &BB : F) {
          if (freqPath.find(&BB) == freqPath.end() && !BB.isEntryBlock() && !isExitBlock(BB)) {
            inFreqPath.insert(&BB);
            llvm::errs() << &BB << "  not in fp \n";
          }
        }

        if (inFreqPath.empty()) {
          llvm::errs() << "no infreq bb, fin ";
          return PreservedAnalyses::all();
        }

        llvm::errs() << "\n\nLoop Preheader: \n";
        llvm::errs() << preHeader << "\n";

        // loop through the freq path and check all loads
        // llvm::errs() << "Load in fp: \n";
        for (llvm::BasicBlock *BB : freqPath) {
          for (llvm::Instruction &I : *BB) {
            llvm::LoadInst *loadInst = llvm::dyn_cast<llvm::LoadInst>(&I);
            if (loadInst) {
              freqLoads.insert(loadInst);
            }
            llvm::StoreInst *storeInst = llvm::dyn_cast<llvm::StoreInst>(&I);
            if (storeInst) {
              freqStores.insert(storeInst);
            }
          }
        }

        for (llvm::BasicBlock *BB : inFreqPath) {
          for (llvm::Instruction &I : *BB) {
            llvm::StoreInst *storeInst = llvm::dyn_cast<llvm::StoreInst>(&I);
            if (storeInst) {
              inFreqStores.insert(storeInst);
            }
          }
        }

        llvm::errs() << "\n\nLD in frq: \n";
        for (llvm::LoadInst* LI : freqLoads) {
          llvm::errs() << *LI << "\n";
        }

        llvm::errs() << "\n\nST in frq: \n";
        for (llvm::StoreInst* SI : freqStores) {
          llvm::errs() << *SI << "\n";
        }

        llvm::errs() << "\n\nST in INfrq: \n";
        for (llvm::StoreInst* SI : inFreqStores) {
          llvm::errs() << *SI << "\n";
        }

        for (llvm::LoadInst* LI : freqLoads) {
          bool findInFreq = false;

          llvm::Value* loadPointerValue = LI->getPointerOperand();
          llvm::errs() << "\n\n checking: " << *LI << '\n';
          // llvm::errs() << "ld opreand: " << *loadPointerValue << '\n';

          // if find a corresponding store in freqPath, then variant.
          for (llvm::StoreInst* SI : freqStores) {
            llvm::Value* storePointerValue = SI->getPointerOperand();
            if (loadPointerValue == storePointerValue) {
              llvm::errs() << "same op as " << *SI << '\n';
              findInFreq = true;
              break;
            }
          }

          if (findInFreq) continue;
          // llvm::errs() << "here\n";
          invariantLoads.insert(LI);
        }
      }

      llvm::errs() << "\n\nInvariant LDs: \n";
      for (llvm::LoadInst* LI : invariantLoads) {
        llvm::errs() << *LI << "\n";
      }

      // hoist almost invarians lds to pre-header

      // find the st dependson almost invariant in infreq-path and fix up

      /* *******Implementation Ends Here******* */
      // Your pass is modifying the source code. Figure out which analyses
      // are preserved and only return those, not all.
      return PreservedAnalyses::all();
    }

    bool isExitBlock(llvm::BasicBlock &BB) {
      llvm::ReturnInst *retInst =  llvm::dyn_cast<llvm::ReturnInst>(BB.getTerminator());
      if (retInst) return true; 
      else return false;
    }
  };
  struct HW2PerformancePass : public PassInfoMixin<HW2PerformancePass> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
      llvm::BlockFrequencyAnalysis::Result &bfi = FAM.getResult<BlockFrequencyAnalysis>(F);
      llvm::BranchProbabilityAnalysis::Result &bpi = FAM.getResult<BranchProbabilityAnalysis>(F);
      llvm::LoopAnalysis::Result &li = FAM.getResult<LoopAnalysis>(F);
      /* *******Implementation Starts Here******* */
      // This is a bonus. You do not need to attempt this to receive full credit.
      /* *******Implementation Ends Here******* */

      // Your pass is modifying the source code. Figure out which analyses
      // are preserved and only return those, not all.
      return PreservedAnalyses::all();
    }
  };
}

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "HW2Pass", "v0.1",
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](StringRef Name, FunctionPassManager &FPM,
        ArrayRef<PassBuilder::PipelineElement>) {
          if(Name == "fplicm-correctness"){
            FPM.addPass(HW2CorrectnessPass());
            return true;
          }
          if(Name == "fplicm-performance"){
            FPM.addPass(HW2PerformancePass());
            return true;
          }
          return false;
        }
      );
    }
  };
}