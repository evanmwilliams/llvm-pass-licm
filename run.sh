cd build; 
make clean; 
make; 
cd ..;
`brew --prefix llvm`/bin/clang -S -emit-llvm -O0 -Xclang -disable-O0-optnone hoist_test.c -o hoist_test.ll;
`brew --prefix llvm`/bin/opt -load-pass-plugin=build/licm/LICMPass.dylib -passes='loop(LICMPass)' hoist_test.ll -S > hoist_test_opt.ll