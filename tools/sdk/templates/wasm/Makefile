# needs clang with the wasm32 target; the result is {{id}}.wasm beside mod.txt
{{id}}.wasm: main.c lce.h
	clang --target=wasm32 -O2 -nostdlib -Wl,--no-entry -Wl,--export-dynamic -o $@ main.c
