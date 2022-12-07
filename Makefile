########################################################################################
# This is a simple Makefile that should be enough to attend the needs of your
# programming task. A work to improve this file to attend diffetent platforms is in 
# progsress, but for now you have to make the necessary changes manually.
#
# Before using this Makefile, you should change the variables INC, CPPLIB accordingly.
# Change /home/dellarrivalisa/gurobi1000 to the directory where Gurobi is installed. 
# The directory /linux64 varies depending on the platform you are using, and the name 
# of -lgurobi100 varies depending on the version you installed.
#
# Send corrections and suggestions to Elisa Dell'Arriva.
########################################################################################

########################################################################################
# Compile multiple source files with Gurobi
########################################################################################
.PHONY: build clean

INC = /home/matheuscasanova/gurobi952/linux64/include/
CPPLIB = -L/home/matheuscasanova/gurobi952/linux64/lib -lgurobi_c++ -lgurobi95
CPP = g++
CARGS = -m64 -Wall 
SOURCES = *.cpp
LAB = exe
OBJ = obj
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))

build: $(SOURCES) $(OBJ) $(OBJECTS)
	$(CPP) $(CARGS) $(OBJ) -o $(LAB) -I$(INC) $(CPPLIB) -lm

$(OBJ): *.cpp
	$(CPP) $(CARGS) -g -c $< -o $@ -I$(INC) $(CPPLIB) -lm

clean:
	rm -rf $(OBJ) $(LAB) *.o


########################################################################################
# Test against expected output
########################################################################################
.PHONY: testar 

testar:
	@set -e ; \
	if [ ! -d testes ] ; then \
		echo "\033[1;31mDiretório 'testes' não encontrado!\033[0m" ; \
		exit 1 ; \
	fi ; \
	for entrada in testes/*.in ; do \
		echo "\033[1;34mTestando \033[4;34m$$entrada\033[0m" ; \
		saida=$$(echo "$$entrada" | sed -re 's/\.in$$/.out/' ) ; \
		resposta=$$(echo "$$entrada" | sed -re 's/\.in$$/.res/' ) ; \
		diff=$$(echo "$$entrada" | sed -re 's/\.in$$/.dif/' ) ; \
		./$(LAB) < "$$entrada" > "$$saida" ; \
		if diff "$$resposta" "$$saida" > "$$diff" ;  then \
			echo "\033[1;32mResultado correto!\033[0m" ; \
		else \
			echo "\033[1;31mFalhou!\033[0m" ; \
			echo ""; \
		fi ; \
		echo ; \
	done

