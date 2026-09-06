CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2
TARGET = solver
SRCS = main.cpp parser.cpp solvers.cpp dpll_solver.cpp cnf_generator.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)