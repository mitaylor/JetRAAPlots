TestRun: Execute
	./Execute --TheoryType None --Data true
	./Execute --TheoryType All --Data true
	./Execute --TheoryType All --Data false
	./Execute --TheoryType Jewel --Data true
	./Execute --TheoryType Jewel --Data false
	./Execute --TheoryType Pyquen --Data true
	./Execute --TheoryType Pyquen --Data false
	./Execute --TheoryType Hybrid --Data true
	./Execute --TheoryType Hybrid --Data false

Execute: MakePlotRRAA.cpp
	g++ MakePlotRRAA.cpp -o Execute `root-config --cflags --glibs`
