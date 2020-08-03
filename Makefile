SUBDIRS := src

.PHONY: subdirs $(SUBDIRS) clean all

subdirs: $(SUBDIRS) 

$(SUBDIRS):
	$(MAKE) -C '$@' $(MAKECMDGOALS)
	mv -f '$@/bbserv' .

clean: $(SUBDIRS)
	rm -rf dist
	rm -f bbserv

dist: $(SUBDIRS) dist/.build_marker

dist/.build_marker: 
	mkdir -p dist
	for d in $(SUBDIRS) ; do cp $$d/dist/* dist ; done
	touch dist/.build_marker


