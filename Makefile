CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = buyerseller
SOURCES = main.cpp
HEADERS = transaction.h item.h data_manager.h store_manager.h bank_manager.h 

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
	rm -f *.csv

.PHONY: clean