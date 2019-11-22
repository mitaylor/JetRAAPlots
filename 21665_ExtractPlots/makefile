
default: TidyAll

TidyAll: Tidy
	bash TidyAll.sh

Tidy: Tidy.cpp
	g++ -rpath ${ROOTSYS}/lib Tidy.cpp -o Tidy `root-config --cflags --glibs`

TranscribeAll:
	bash TranscribeAll.sh

