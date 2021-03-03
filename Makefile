GIT_HOOK := .git/hooks/applied
$(GIT_HOOK): scripts/install-git-hooks
	@$<
	@echo

.PHONY: all check clean example
all: $(GIT_HOOK) check
.DEFAULT_GOAL := all

include common.mk

CFLAGS = -I./include -I./private
CFLAGS += -std=c99 -pedantic -Wall -W -Werror

EXAMPLES = \
    insert-sort \
    quick-sort

EXAMPLES := $(addprefix examples/,$(EXAMPLES))

deps2 := $(EXAMPLES:%=%.o.d)

EXAMPLES_OK = $(EXAMPLES:=.ok)

example: $(EXAMPLES_OK)

$(EXAMPLES_OK): %.ok: %
	$(Q)$(PRINTF) "*** Validating $< ***\n"
	$(Q)./$< && $(PRINTF) "\t$(PASS_COLOR)[ Verified ]$(NO_COLOR)\n"
	@touch $@

TESTS = \
    containerof \
    list_entry \
    list_init-explicit \
    list_init-local \
    list_init-global \
    list_add \
    list_add_tail \
    list_del \
    list_del_init \
    list_first_entry \
    list_last_entry \
    list_is_singular \
    list_for_each_safe \
    list_for_each_entry_safe \
    list_for_each \
    list_for_each_entry \
    list_move \
    list_move_tail \
    list_splice \
    list_splice_tail \
    list_splice_init \
    list_splice_tail_init \
    list_cut_position

TESTS := $(addprefix tests/,$(TESTS))
# dependency of source files
deps := $(TESTS:%=%.o.d)

TESTS_OK = $(TESTS:=.ok)

check: $(TESTS_OK)

$(TESTS_OK): %.ok: %
	$(Q)$(PRINTF) "*** Validating $< ***\n"
	$(Q)./$< && $(PRINTF) "\t$(PASS_COLOR)[ Verified ]$(NO_COLOR)\n"
	@touch $@

# standard build rules
.SUFFIXES: .o .c
.c.o:
	$(VECHO) "  CC\t$@\n"
	$(Q)$(CC) -o $@ $(CFLAGS) -c -MMD -MF $@.d $<

$(TESTS): %: %.o
	$(VECHO) "  LD\t$@\n"
	$(Q)$(CC) -o $@ $^ $(LDFLAGS)

$(EXAMPLES): %: %.o
	$(VECHO) "  LD\t$@\n"
	$(Q)$(CC) -o $@ $^ $(LDFAGS)

clean:
	$(VECHO) "  Cleaning...\n"
	$(Q)$(RM) $(TESTS) $(TESTS_OK) $(TESTS:=.o) $(deps)
	$(Q)$(RM) $(EXAMPLES) $(EXAMPLES_OK) $(EXAMPLES:=.o) $(deps2)

-include $(deps)
-include $(deps2)
