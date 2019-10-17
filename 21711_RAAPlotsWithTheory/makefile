default: TestRun

TestRun: Execute
	./Execute --TheoryType None
	./Execute --TheoryType NoData
	./Execute --TheoryType All
	./Execute --TheoryType Jewel
	./Execute --TheoryType Pyquen
	./Execute --TheoryType Ivan 

Execute: MakePlotRAA.cpp
	g++ MakePlotRAA.cpp -o Execute `root-config --cflags --glibs`
