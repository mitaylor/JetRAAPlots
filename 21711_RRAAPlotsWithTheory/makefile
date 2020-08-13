default: TestRun

TestRun: Execute
	./Execute --TheoryType None --Data true --Pres true --Prelim false
	./Execute --TheoryType All --Data true --Pres true --Prelim false
	./Execute --TheoryType Generators --Data true --Pres true --Prelim false
	./Execute --TheoryType MC --Data true --Pres true --Prelim false
	./Execute --TheoryType Calculations --Data true --Pres true --Prelim false
	./Execute --TheoryType 10to30 --Data true --Pres true --Prelim false
	./Execute --TheoryType 30to50 --Data true --Pres true --Prelim false
	./Execute --TheoryType 50to90 --Data true --Pres true --Prelim false

	./Execute --TheoryType All --Data false --Pres true --Prelim false
	./Execute --TheoryType Calculations --Data false --Pres true --Prelim false

	./Execute --TheoryType None --Data true --Pres false --Prelim true
	./Execute --TheoryType All --Data true --Pres false --Prelim true
	./Execute --TheoryType Generators --Data true --Pres false --Prelim true
	./Execute --TheoryType MC --Data true --Pres false --Prelim true
	./Execute --TheoryType Calculations --Data true --Pres false --Prelim true
	./Execute --TheoryType 10to30 --Data true --Pres false --Prelim true
	./Execute --TheoryType 30to50 --Data true --Pres false --Prelim true
	./Execute --TheoryType 50to90 --Data true --Pres false --Prelim true

	./Execute --TheoryType None --Data true --Pres false --Prelim false
	./Execute --TheoryType All --Data true --Pres false --Prelim false
	./Execute --TheoryType Generators --Data true --Pres false --Prelim false
	./Execute --TheoryType MC --Data true --Pres false --Prelim false
	./Execute --TheoryType Calculations --Data true --Pres false --Prelim false
	./Execute --TheoryType 10to30 --Data true --Pres false --Prelim false
	./Execute --TheoryType 30to50 --Data true --Pres false --Prelim false
	./Execute --TheoryType 50to90 --Data true --Pres false --Prelim false

Execute: MakePlotRRAA.cpp
	g++ -rpath ${ROOTSYS}/lib MakePlotRRAA.cpp -o Execute `root-config --cflags --glibs`
