PROJECTNAME = ceedling

.PHONY: all

help:
	@echo "This Makefile has no default rule. Use one of the following:"
	@echo "make ceedling ..... to setup ceedling tests"
	@echo "make test ......... to run tests"
	@echo "make clean ........ to delete the testing folder (be careful not to lose any code)"

all: ceedling test


.PHONY: ceedling test clean


ceedling:
	ceedling new $(PROJECTNAME)
	cd $(PROJECTNAME) ; cp ../project.yml . ; rake module:create[chatstorage]
	cp ../src/chatstorage.[ch] $(PROJECTNAME)/src
	cp test_chatstorage.c $(PROJECTNAME)/test

test:
	cd $(PROJECTNAME); rake test:all

clean:
	clear
	$(RM) -r $(PROJECTNAME)


# $@ reffers to named before :
# # $^ reffers to all named after :
# # $< reffers to first after :
#
