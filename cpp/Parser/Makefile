#sudo apt install flex bison

CC = gcc 
CPP = g++
LEX = flex -I -t -i
YACC = bison -d
LIBS =  -ly -ll
FRAMEWORKS = -framework Foundation
YYLE = NscrParser
SOURCES = $(YYLE).tab.cpp $(YYLE).cpp

all: parser

parser : $(SOURCES)
	$(CPP) -o parser $(SOURCES)

clean :
	rm -rf $(YYLE).tab.* $(YYLE).cpp

$(YYLE).tab.cpp :  $(YYLE).y 
	$(YACC) $(YYLE).y -o $(YYLE).tab.cpp

$(YYLE).cpp : $(YYLE).l
	$(LEX) $(YYLE).l > $(YYLE).cpp

