build:
	gcc src/main.c -o syntaxflowc

run:
	make build && ./syntaxflowc

dbgrun:
	make build && valgrind ./syntaxflowc
