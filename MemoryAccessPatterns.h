
#ifndef LLVM_TRANSFORMS_UTILS_MEMORYACCESSPATTERNS_H
#define LLVM_TRANSFORMS_UTILS_MEMORYACCESSPATTERNS_H

#include "llvm/IR/PassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include <map>
#include <string>

namespace llvm {

class MemoryAccessPatternsPass : public PassInfoMixin<MemoryAccessPatternsPass> {
    std::map<Function*, unsigned> LocalVarCounters; 
    
public:
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);

private:
    std::string getVariableName(Value *V, Function *F, Instruction *I);
    std::string getArrayOrStructElementName(GetElementPtrInst *GEP, Function *F, Instruction *I);
    struct MemoryAccessInfo {
    unsigned Size = 0;  
    unsigned Count = 0; 
};
};

} 

#endif 


