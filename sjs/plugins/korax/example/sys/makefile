#
# Copyright (C) 2005 Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com
#

COMPILER = g++
CCFLAGS  = -O3 -pthread
OUTPUT   = sys.exe

all : sys.cpp util_sys.o util_log.o util_sync.o util_thread.o util_par.o util_misc.o util_vector.o
	@echo "Linking ..."
	@ $(COMPILER) $(CCFLAGS) $? -o $(OUTPUT)
	@strip -s $(OUTPUT)
	@echo "Done"

util_misc.o: ../../util/util_misc.cpp ../../util/util_misc.h
	@echo "Compiling miscelaneous stuff ..."
	@ $(COMPILER) $(CCFLAGS) -c ../../util/util_misc.cpp

util_vector.o: ../../util/util_vector.cpp ../../util/util_vector.h
	@echo "Compiling vector stuff ..."
	@ $(COMPILER) $(CCFLAGS) -c ../../util/util_vector.cpp

util_sync.o: ../../util/util_sync.cpp ../../util/util_sync.h
	@echo "Compiling synchronization classes ..."
	@ $(COMPILER) $(CCFLAGS) -c ../../util/util_sync.cpp

util_thread.o: ../../util/util_thread.cpp ../../util/util_thread.h
	@echo "Compiling CThread class ..."
	@ $(COMPILER) $(CCFLAGS) -c ../../util/util_thread.cpp

util_sys.o: ../../util/util_sys.cpp ../../util/util_sys.h
	@echo "Compiling CSys class ..."
	@ $(COMPILER) $(CCFLAGS) -c ../../util/util_sys.cpp

util_log.o: ../../util/util_log.cpp ../../util/util_log.h
	@echo "Compiling CLog class ..."
	@ $(COMPILER) $(CCFLAGS) -c ../../util/util_log.cpp

util_par.o: ../../util/util_par.cpp ../../util/util_par.h
	@echo "Compiling CPar class ..."
	@ $(COMPILER) $(CCFLAGS) -c ../../util/util_par.cpp

clean:
	@echo "Deleting useless files ..."
	@ rm -rf *.o a.out $(OUTPUT)

re:
	@echo "Complete rebuild ..."
	@ make clean
	@echo "Updating timestamp ..."
	@ touch ../../util/*
	@ touch *
	@ make
