CXXFLAGS = -g -Wall -Werror -std=c++11

ncc: driver.o machine_code_gen.o parser.o lexical_analyzer.o buffered_input.o storage.o symbol_table.o node.o error.o disasm.o
	g++ $(CXXFLAGS) -o ncc driver.o machine_code_gen.o parser.o lexical_analyzer.o buffered_input.o storage.o symbol_table.o node.o error.o disasm.o

driver.o: driver.cpp machine_code_gen.h parser.h lexical_analyzer.h buffered_input.h symbol_table.o node.h error.h
	g++ $(CXXFLAGS) -c driver.cpp

machine_code_gen.o: machine_code_gen.cpp machine_code_gen.h symbol_table.h storage.h node.h disasm.h
	g++ $(CXXFLAGS) -c machine_code_gen.cpp

parser.o: parser.cpp parser.h storage.h symbol_table.h node.h error.h
	g++ $(CXXFLAGS) -c parser.cpp

lexical_analyzer.o: lexical_analyzer.cpp lexical_analyzer.h buffered_input.h error.h
	g++ $(CXXFLAGS) -c lexical_analyzer.cpp

buffered_input.o: buffered_input.cpp buffered_input.h error.h
	g++ $(CXXFLAGS) -c buffered_input.cpp

storage.o: storage.cpp storage.h
	g++ $(CXXFLAGS) -c storage.cpp

symbol_table.o: symbol_table.cpp symbol_table.h storage.h
	g++ $(CXXFLAGS) -c symbol_table.cpp

node.o: node.cpp node.h
	g++ $(CXXFLAGS) -c node.cpp

error.o: error.cpp error.h buffered_input.h
	g++ $(CXXFLAGS) -c error.cpp

disasm.o: disasm.cc disasm.h
	g++ $(CXXFLAGS) -c disasm.cc

clean:
	rm -f *.o ncc
