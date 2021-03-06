#-------------------------- Makefile ------------------------------------#

#-- dirs --#
local_dir= build modules

.PHONY: all
all:
	@dir_list='$(local_dir)';for d_name in $$dir_list;\
	do \
		if [ -d $$d_name ] && [ "build" == "$$d_name" ]; then \
			(cd $$d_name && cmake ..) \
		fi; \
		if [ -d $$d_name ] && [ -f $$d_name/Makefile ]; then \
			echo "<make>: $$d_name ..."; \
			(cd $$d_name && make) \
		fi; \
	done;

.PHONY: clean
clean:
	@dir_list='$(local_dir)';for d_name in $$dir_list;\
	do\
		if [ -d $$d_name ] && [ -f $$d_name/Makefile ]; then \
			echo "<make-clean>: $$d_name ..."; \
			(cd $$d_name && make clean) \
		fi \
	done;


