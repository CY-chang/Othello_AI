CXX			= g++
CXXFLAGS	= --std=c++14
SOURCES		= $(wildcard *.cpp)
ifeq ($(OS),Windows_NT)
EXE			= $(SOURCES:%.cpp=%.exe)
else
EXE			= $(SOURCES:%.cpp=%)
endif
OTHER		= action state gamelog.txt

.PHONY: all clean

all: $(EXE) baseline_permissions

ifeq ($(OS),Windows_NT)
$(EXE): %.exe : %.cpp
	$(CXX) -Wall -Wextra $(CXXFLAGS) -o $@ $<
else
$(EXE): % : %.cpp
	$(CXX) -Wall -Wextra $(CXXFLAGS) -o $@ $<
endif

baseline_permissions:
	chmod +x baseline/baseline1
	chmod +x baseline/baseline2
	chmod +x baseline/baseline3
	chmod +x baseline/baseline4
	chmod +x baseline/baseline5

clean:
ifeq ($(OS),Windows_NT)
	del /f $(EXE) $(OTHER)
else
	rm -f $(EXE) $(OTHER)
endif
