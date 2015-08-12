DBC_FILES = $(shell find . -maxdepth 1 -name "*.dbc")
HEADERS = $(DBC_FILES:.dbc=.h)

AWK= awk

all: $(HEADERS)

%.h: %.dbc
	@echo 'Creating $(@) from $(<)'
	$(AWK) -f ./dbc2c.awk $< > $@

clean:
	rm -fv $(HEADERS)
