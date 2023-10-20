#include "llvm/Analysis/DomTreeUpdater.h"
#include "llvm/Analysis/MemorySSAUpdater.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/Pass.h"
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
        PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                              LoopStandardAnalysisResults &AR, LPMUpdater &U)
        {
            errs() << "Running loop pass!\n";
            auto *Preheader = L.getLoopPreheader();
            std::vector<Instruction *> instructions_to_hoist;
            std::unordered_set<Value *> LI;
            bool has_converged = false;
            while (!has_converged)
            {
                has_converged = true;
                for (auto *BB : L.blocks())
                {
                    for (auto &I : *BB)
                    {
                        bool contains_loop_invariants = true;
                        for (const auto &Use : I.operands())
                        {
                            if (LI.find(&*Use) == LI.end())
                            {
                                if (auto *UseInst = dyn_cast<Instruction>(&*Use))
                                {
                                    if (L.contains(UseInst))
                                    {
                                        contains_loop_invariants = false;
                                        break;
                                    }
                                }
                            }
                        }
                        if (LI.find(&I) != LI.end() || !isSafeToSpeculativelyExecute(&I) || I.mayReadOrWriteMemory() || !contains_loop_invariants)
                            continue;
                        LI.insert(&I);
                        instructions_to_hoist.push_back(&I);
                        has_converged = false;
                    }
                }
            }

            for (auto *Inst : instructions_to_hoist)
            {
                Inst->moveBefore(Preheader->getTerminator());
            }
            return instructions_to_hoist.empty() ? PreservedAnalyses::all() : PreservedAnalyses::none();
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
                    [](StringRef name, FunctionPassManager &FPM,
                       ArrayRef<PassBuilder::PipelineElement>)
                    {
                        if (name != "LICMPass")
                            return false;
                        FPM.addPass(createFunctionToLoopPassAdaptor(LICMPass()));
                        return true;
                    });
            }};
}