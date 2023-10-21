#include "llvm/Analysis/DomTreeUpdater.h"
#include "llvm/Analysis/MemorySSAUpdater.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include <vector>
#include <unordered_set>

using namespace llvm;

namespace
{
    struct LICMPass : public PassInfoMixin<LICMPass>
    {
        bool isLoopInvariant(Instruction *I, Loop &L)
        {
            for (Value *operand : I->operands())
            {
                if (!L.isLoopInvariant(operand))
                    return false;
            }
            return true;
        }
        PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                              LoopStandardAnalysisResults &AR, LPMUpdater &U)
        {
            errs() << "Running loop pass!\n";
            auto *Preheader = L.getLoopPreheader();
            bool changed = false;
            bool has_converged = false;
            for (auto *BB : L.blocks())
            {
                for (BasicBlock::iterator II = BB->begin(), E = BB->end(); II != E;)
                {
                    Instruction &I = *II++;
                    if (isLoopInvariant(&I, L) && !I.isTerminator()) // Use L directly
                    {
                        // Move the instruction to the loop preheader
                        I.moveBefore(Preheader->getTerminator()); // Use . not ->
                        I.eraseFromParent();
                        changed = true;
                    }
                }
            }

            return changed ? PreservedAnalyses::all() : PreservedAnalyses::none();
        };
    };

    static StringRef name()
    {
        return "LICMPass";
    }

}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo()
{
    return {.APIVersion = LLVM_PLUGIN_API_VERSION,
            .PluginName = "LICMPass",
            .PluginVersion = "v0.1",
            .RegisterPassBuilderCallbacks = [](PassBuilder &PB)
            {
                PB.registerPipelineParsingCallback(
                    [](StringRef Name, LoopPassManager &LPM, ArrayRef<PassBuilder::PipelineElement>)
                    {
                        if (Name == "LICMPass")
                        {
                            errs() << "Loading LICMPass\n";
                            LPM.addPass(LICMPass());
                            return true;
                        }
                        return false;
                    });
            }};
}