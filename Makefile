.PHONY: all clean

all:
	$(MAKE) -C src all

clean:
	-rm -rf bin/ src/*/*.o