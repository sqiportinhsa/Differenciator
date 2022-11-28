CPPFLAGS = -D _DEBUG -ggdb3 -std=c++2a -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -fsanitize=address,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,leak,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

DIFF = build/differenciator.exe

FOLDERS = obj build

.PHONY: all

all: folders $(DIFF)

clean: 
	find . -name "*.o" -delete

folders:
	mkdir -p $(FOLDERS)

$(DIFF): obj/diff.o obj/tree.o obj/file_reading.o obj/logging.o obj/main.o obj/descent.o obj/common.o
	g++ obj/main.o obj/diff.o obj/tree.o obj/file_reading.o obj/logging.o obj/common.o obj/descent.o -o $(DIFF) $(CPPFLAGS)


obj/main.o: main.cpp obj/diff.o
	g++ -c main.cpp -o obj/main.o $(CPPFLAGS)

obj/common.o: common.cpp common.h
	g++ -c common.cpp -o obj/common.o $(CPPFLAGS)

obj/diff.o: Diff/diff.cpp Diff/diff.h Tree/tree.cpp Tree/tree.h
	g++ -c Diff/diff.cpp -o obj/diff.o $(CPPFLAGS)


obj/tree.o: Tree/tree.cpp Tree/tree.h
	g++ -c Tree/tree.cpp -o obj/tree.o $(CPPFLAGS)


obj/descent.o: Rec_desc/descent.cpp Rec_desc/descent.h
	g++ -c Rec_desc/descent.cpp -o obj/descent.o $(CPPFLAGS)


obj/file_reading.o: Libs/file_reading.cpp Libs/file_reading.hpp
	g++ -c Libs/file_reading.cpp -o obj/file_reading.o $(CPPFLAGS)


obj/logging.o: Libs/logging.cpp Libs/logging.h
	g++ -c Libs/logging.cpp -o obj/logging.o $(CPPFLAGS)