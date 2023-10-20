#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/PostDominators.h"

using namespace llvm;

namespace
{
    struct LICMPass : public PassInfoMixin<LICMPass>
    {
        PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM)
        {
            auto &FAM = AM.getResult<FunctionAnalysisManagerModuleProxy>(M).getManager();
            for (auto &F : M)
            {
                errs() << "I saw a function called " << F.getName() << "!\n";
                if (F.isDeclaration())
                    continue;
                auto &LI = FAM.getResult<LoopAnalysis>(F);
                DominatorTree *DT = new DominatorTree(F);
                for (auto &L : LI)
                {
                    errs() << "Loop found!\n";
                    for (auto &BB : L->blocks())
                    {
                        for (auto &I : *BB)
                        {
                            errs() << I << "\n";
                        }
                    }
                }
            }
            return PreservedAnalyses::all();
        };
    };

}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo()
{
    return {
        .APIVersion = LLVM_PLUGIN_API_VERSION,
        .PluginName = "LICM pass",
        .PluginVersion = "v0.1",
        .RegisterPassBuilderCallbacks = [](PassBuilder &PB)
        {
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel Level)
                {
                    MPM.addPass(LICMPass());
                });
        }};
}
