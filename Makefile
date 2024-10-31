prog=data_reader
src=data_reader.c
headers=util.h parsing.h array_t.h

gcc_flags=-g -g3 -Wall -Wextra -pedantic
test_dir=./tests


.PHONY: compile

run: $(prog)
	./$<

$(prog): $(src) $(headers)
	gcc $(gcc_flags) $< -o $@

debug: $(prog)
	gdb ./$(prog)

valgrind: $(prog)
	cat $(test_dir)/test_1.in | valgrind --track-origins=yes --leak-check=full ./data_reader
	cat $(test_dir)/128_dims.in | valgrind --track-origins=yes --leak-check=full ./data_reader


test_exe: $(src) $(headers)
	gcc -DFLOAT=0 $(gcc_flags) $< -o data_reader

test: test_exe
	./data_reader $(test_dir)/test_1.in | diff $(test_dir)/test_1.out - && echo Success
	./data_reader $(test_dir)/empty.in  | diff $(test_dir)/empty.out  - && echo Success

clean:
	rm -rf $(prog) test_exe vgcore*
