CC=gcc
SRC=src
ODIR=obj
OUT=bin

$(OUT)/a1shell: $(ODIR)/error_handling.o $(SRC)/a1shell.c
	$(CC) -c $(SRC)/a1shell.c -o $(ODIR)/a1shell.o
	$(CC) -o $(OUT)/a1shell $(ODIR)/a1shell.o $(ODIR)/error_handling.o

$(ODIR)/error_handling.o: $(SRC)/error_handling.c
	mkdir -p $(ODIR)
	$(CC) -c $(SRC)/error_handling.c -o $(ODIR)/error_handling.o

clean:
	rm -rf $(ODIR)

tar:
	tar -czvf assignment1.tar ./README.md ./src/ ./bin/ ./Makefile
