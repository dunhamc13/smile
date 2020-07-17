# Makefile
# This is an example Makefile to help you get familiar with GNU Make
# and get started with using it to automate your project build tasks.
# For the full GNU Make manual, see:
# https://www.gnu.org/software/make/manual/make.html
#
# A Makefile consists of rules, which take following form:
# target … : prerequisites …
#        recipe
#        …
#        …
#
# Multiple targets and prerequisites are separated by spaces;
# multiple recipes are separated by newlines.
#
# If the target file exists, and the file has not been modified since any
# of its prerequisite files were last modified, the rule will be skipped.
# This means that it won't redo any work that it doesn't need to!
#
# Some things to note:
# - Indentation must be tabs--Make will error if you use spaces.
# - To break a long line, use a backslash: \
# - If you just type "make" at the command line in the directory
# where the Makefile is, it will run the first recipe
# (after running its prerequisites).
# - Other than that, the ordering of the rules in the file doesn't matter.
# Make builds a dependency graph and performs a topological sort to determine
# the correct rule execution order.
# - If you just type "make" it will run the first rule in the file.
# - If you type "make {rule name}" it will run that specific rule name
# (again, after running its prerequisites).
# - If you type "make -n" it will tell you what it's going to run without
# actually running it (a dry run).
#
# Rules can either be file names (a rule for producing a file)
# or just task names that aren't files, called "phony rules."
# List any phony rules here so that Make knows not to look for files
# by these names:
.PHONY : all test zip
CC = g++
STD = -std=c++11
CFLAGS = $(STD) 
PT = -lpthread
# CFLAGS = -g -O0 -Wall -Wextra -lpthread $(STD) \
#	-fsanitize=address -fsanitize=leak -fsanitize=undefined -fno-sanitize-recover

# Because "make" runs the first rule in the file, make the first rule
# inclusive of the other rules in your Makefile:
all : shop.out testShop.out

# A rule to make the main executable:
shop.out : driver.o shop.o
	$(CC) $(CFLAGS) driver.o shop.o -o $@ $(PT)

# A rule to make the main executable:
driver.o : driver.cpp shop.o
	$(CC) $(CFLAGS) $< -c -o $@

# A rule to compile the main file for use by the tests.
# Uses a #define macro to replace `main` with `notmain` because
# a C++ program can only have one `main` function.
# after $(CFLAGS) -Dmain=notmain
shop.o : Shop.cpp Shop.h
	$(CC) $(CFLAGS) $< -c -o $@

# A rule to compile the test executable
testShop.out : test_shop.cpp Shop.cpp Shop.h shop.o test.o
	$(CC) $(CFLAGS) test_shop.cpp shop.o test.o -o $@

# A rule to run your tests:
test : test_shop.out
	./$< -s

# A rule to run your leak tests with valgrind:
leak : shop.out
	valgrind --leak-check=full ./$< 3 3

# A rule to compile the "dummy" main for Catch2
test.o : test.cpp catch.hpp
	$(CC) $(STD) $< -c -o $@

# A rule to zip the source files
zip : shop.zip

# A rule to make the zip file
convex_mp.zip : $(wildcard *.cpp) $(wildcard *.h) $(wildcard *.hpp) Makefile
	zip $@ $^

# A rule to delete all compiled files and leave only source code behind
clean :
#	rm -r convex.out_mp.dSYM
#	rm -r test_convex_mp.out.dSYM
	rm *.o
	rm *.out
#	rm *.zip
