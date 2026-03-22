# 银行综合管理系统 Makefile

CXX = g++
CXXFLAGS = -std=c++11 -Wall -I./include

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

TARGET = $(BINDIR)/bank_system
TEST_TARGET = $(BINDIR)/test_system

.PHONY: all clean test directories

all: directories $(TARGET)

directories:
	@mkdir -p $(OBJDIR) $(BINDIR)

$(TARGET): $(filter-out $(OBJDIR)/test.o, $(OBJECTS))
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_TARGET): $(filter-out $(OBJDIR)/main.o, $(OBJECTS))
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: directories $(TEST_TARGET)
	./$(TEST_TARGET)

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJDIR) $(BINDIR) *.txt *.huf

# 安装依赖（如果需要）
install:
	@echo "本系统使用C++标准库，无需安装第三方依赖"

# 帮助信息
help:
	@echo "银行综合管理系统构建工具"
	@echo ""
	@echo "可用目标:"
	@echo "  make all    - 编译主程序"
	@echo "  make test   - 编译并运行测试"
	@echo "  make run    - 编译并运行主程序"
	@echo "  make clean  - 清理编译产物"
	@echo "  make help   - 显示此帮助信息"
