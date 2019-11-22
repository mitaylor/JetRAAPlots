default: TestRun

TestRun: Execute
	./Execute

Files: GetValues
	./GetValues

Execute: MakePlot.cpp
	g++ -rpath ${ROOTSYS}/lib MakePlot.cpp -o Execute `root-config --cflags --glibs`

GetValues: GetValues.cpp
	g++ -rpath ${ROOTSYS}/lib GetValues.cpp -o GetValues `root-config --cflags --glibs`
