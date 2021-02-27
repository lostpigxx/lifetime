# makefiles

BUILD_DIR = ./build/
FLAG_O_FILE = -o $@

OBJECTS = benchmark.o art.o massnode.o masstree.o
OBJ_PATH = $(addprefix $(BUILD_DIR), $(OBJECTS))

# $@  表示目标文件
# $^  表示所有的依赖文件
index_bench : $(OBJ_PATH)
	g++ -std=c++11 -o $@ $^

$(BUILD_DIR)benchmark.o : util.h option.h kv_generator.h index_warpper.h
	g++ -c -std=c++11 benchmark.cpp -o $@

$(BUILD_DIR)art.o :
	g++ -c ./index/art.cpp -o $@

$(BUILD_DIR)massnode.o :
	g++ -c ./index/massnode.cpp -o $@

$(BUILD_DIR)masstree.o :
	g++ -c ./index/masstree.cpp -o $@


.PHONY : clean
clean :
	-rm -f index_bench $(OBJ_PATH) a.out