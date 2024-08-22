
#include "llvm/Transforms/Utils/MemoryAccessPatterns.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GetElementPtrTypeIterator.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/CommandLine.h"

#include <map>
#include <string>

static llvm::cl::opt<bool> AnalyzeMemoryAccessPatterns(
    "analyze-memory-access-patterns", 
    llvm::cl::desc("Enable memory access patterns analysis"),
    llvm::cl::init(false));
    
static llvm::cl::opt<bool> MemoryAccesses(
    "memory-access", 
    llvm::cl::desc("Enable memory access count analysis"),
    llvm::cl::init(false));


using namespace llvm;

PreservedAnalyses MemoryAccessPatternsPass::run(Module &M, ModuleAnalysisManager &AM) {
	
    if (MemoryAccesses){
	for (Function &F : M) {
	    unsigned LoadCount = 0;
	    unsigned StoreCount = 0;

	    // Iterate over all basic blocks in the function
	    for (BasicBlock &BB : F) {
	      // Iterate over all instructions in the basic block
	      for (Instruction &I : BB) {
		if (isa<LoadInst>(&I)) { // Use &I to get a pointer to Instruction
		  LoadCount++;
		} else if (isa<StoreInst>(&I)) {
		  StoreCount++;
		}
	      }
	    }

	    // Print the counts of loads and stores for the function
	    errs() << "Function: " << F.getName() << "\n";
	    errs() << "  Load instructions: " << LoadCount << "\n";
	    errs() << "  Store instructions: " << StoreCount << "\n";
  }
  }

    if (!AnalyzeMemoryAccessPatterns) {
        return PreservedAnalyses::all(); // Skip analysis if the option is not enabled
    }

    const DataLayout &DL = M.getDataLayout();
    std::map<Value*, std::string> VariableNames; 
    std::map<std::string, MemoryAccessInfo> AccessMap;

    // First pass: Collect variable names
    for (Function &F : M) {
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                if (LoadInst *LI = dyn_cast<LoadInst>(&I)) {
                    Value *Ptr = LI->getPointerOperand();
                    std::string VarName = getVariableName(Ptr, &F, &I);
                    uint64_t Size = DL.getTypeStoreSize(LI->getType());
                    VariableNames[Ptr] = VarName;
                    AccessMap[VarName].Size = Size;
                    AccessMap[VarName].Count = 0;
                } else if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
                    Value *Ptr = SI->getPointerOperand();
                    std::string VarName = getVariableName(Ptr, &F, &I);
                    uint64_t Size = DL.getTypeStoreSize(SI->getValueOperand()->getType());
                    VariableNames[Ptr] = VarName;
                    AccessMap[VarName].Size = Size;
                    AccessMap[VarName].Count = 0;
                }
            }
        }
    }

    // Second pass: Perform counting using pre-collected names
    for (Function &F : M) {
        for (const auto &Entry : AccessMap) {
            AccessMap[Entry.first].Count=0;
        }

        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                if (LoadInst *LI = dyn_cast<LoadInst>(&I)) {
                    Value *Ptr = LI->getPointerOperand();
                    std::string VarName = VariableNames[Ptr];
                    uint64_t Size = DL.getTypeStoreSize(LI->getType());
                    AccessMap[VarName].Count++;
                } else if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
                    Value *Ptr = SI->getPointerOperand();
                    std::string VarName = VariableNames[Ptr];
                    uint64_t Size = DL.getTypeStoreSize(SI->getValueOperand()->getType());
                    AccessMap[VarName].Count++;
                }
            }
        }

        // Print the memory access patterns for the function
        errs() << F.getParent()->getSourceFileName() << ":" << F.getName() << "()\n";
        for (const auto &Entry : AccessMap) {
            if (Entry.second.Count!=0){
            errs() << Entry.first << ": " << (Entry.second.Count)*(Entry.second.Size) << " bytes (" 
                   << Entry.second.Count << " time" << (Entry.second.Count > 1 ? "s" : "") << ")\n";
        }
        }
        errs() << "\n";
    }

    return PreservedAnalyses::all();
}

std::string MemoryAccessPatternsPass::getVariableName(Value *V, Function *F, Instruction *I) {
    if (GlobalVariable *GV = dyn_cast<GlobalVariable>(V)) {
        return GV->getName().str();
    }

    if (AllocaInst *AI = dyn_cast<AllocaInst>(V)) {
        if (AI->hasName()) {
            return AI->getName().str();
        } else {
            unsigned &Counter = LocalVarCounters[F];
            return "UnnamedLocalVar[" + std::to_string(Counter++) + "]";
        }
    }

    if (GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(V)) {
        return getArrayOrStructElementName(GEP, F, I);
    }

    if (Instruction *Inst = dyn_cast<Instruction>(V)) {
        if (Inst->hasName()) {
            return Inst->getName().str();
        } else {
            unsigned &Counter = LocalVarCounters[F];
            return "UnnamedVar[" + std::to_string(Counter++) + "]";
        }
    }
    return "UnnamedVar";
}



std::string MemoryAccessPatternsPass::getArrayOrStructElementName(GetElementPtrInst *GEP, Function *F, Instruction *I) {
    Value *PtrOperand = GEP->getPointerOperand();
    std::string BaseName = getVariableName(PtrOperand, F, I);

    for (unsigned i = 1; i < GEP->getNumOperands(); ++i) {
        Value *IndexValue = GEP->getOperand(i);
        Type *SourceElementType = GEP->getSourceElementType();

        if (SourceElementType->isArrayTy()) {
            if (ConstantInt *CI = dyn_cast<ConstantInt>(IndexValue)) {
                unsigned Index = CI->getZExtValue();
                BaseName += "[" + std::to_string(Index) + "]";
            } else {
                BaseName += "[?]"; 
            }
        } else if (SourceElementType->isStructTy()) {
            if (ConstantInt *CI = dyn_cast<ConstantInt>(IndexValue)) {
                unsigned FieldIndex = CI->getZExtValue();
                BaseName += ".field" + std::to_string(FieldIndex);
            }
        }
    }

    return BaseName;
}


