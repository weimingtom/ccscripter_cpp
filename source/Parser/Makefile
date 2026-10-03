CC = cc 
LEX = flex -I -t -i
YACC = bison -d
LIBS =  -ly -ll
FRAMEWORKS = -framework Foundation
YYLE = NscrParser
SOURCES = $(YYLE).tab.m $(YYLE).m

parser : $(SOURCES)

clean :
	-rm $(YYLE).tab.* $(YYLE).m

$(YYLE).tab.m :  $(YYLE).y 
	$(YACC) $(YYLE).y
	mv $(YYLE).tab.c $(YYLE).tab.m

$(YYLE).m : $(YYLE).l
	$(LEX) $(YYLE).l > $(YYLE).m

