default: TestRun

TestRun: Execute
	./Execute --TheoryType None
	./Execute --TheoryType All
	./Execute --TheoryType Jewel
	./Execute --TheoryType Pyquen
	./Execute --TheoryType Ivan 
	./Execute --TheoryType Hybrid

Jewel: Execute
	./Execute --TheoryType Jewel

Pyquen: Execute
	./Execute --TheoryType Pyquen

Hybrid:	Execute
	./Execute --TheoryType Hybrid

Ivan: Execute
	./Execute --TheoryType Ivan

Execute: MakePlotRAA.cpp
	g++ MakePlotRAA.cpp -o Execute `root-config --cflags --glibs`
