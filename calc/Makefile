BUILD_DIR=../build

LIB_SRC := add.c mul.c div.c sub.c 
LIB_SO := add.o mul.o div.o sub.o
app:main.c libcalc.so
	gcc $^ -Iinclude -L./ -lcalc -Wl,-R. -o $@

libcalc.so:$(LIB_SO)
	gcc -shared $^ -o $@

$(LIB_SO):$(LIB_SRC)
	gcc -Iinclude -c -fpic $^

.PHONY:clean
clean:
	-@echo cleaning...
	-rm $(LIB_SO) app libcalc.so
# -rm -rf *.o
# -rm -rf *.a
# -rm -rf *.so
# -rm -rf app
# -rm -rf $(BUILD_DIR)/*
