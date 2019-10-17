TestRun: Execute
	./Execute --TheoryType None
	./Execute --TheoryType NoData
	./Execute --TheoryType All
	./Execute --TheoryType Jewel
	./Execute --TheoryType Pyquen
	./Execute --TheoryType Hybrid

Execute: MakePlotRRAA.cpp
	g++ MakePlotRRAA.cpp -o Execute `root-config --cflags --glibs`
