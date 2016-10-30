CXX = clang++

CXXFLAGS =  -nostdinc++ -I/usr/local/include/c++/v1 -I$(SRCDIR) -std=c++1z -MMD # -D DEBUG=1

LDFLAGS = -nostdlib -L/usr/lib -L/usr/local/lib -lc++ -lSystem

SRCDIR = src

TESTDIR = test

OBJDIR = obj

BINDIR = bin

GTESTDIR = ../googletest/googletest

GTESTLIB = $(GTESTDIR)/make/gtest_main.a


#TARGETS = $(addprefix $(BINDIR)/, )

#MAINS	= $(TARGETS:$(BINDIR)/%=$(SRCDIR)/%.cpp)

SOURCES = $(filter-out $(MAINS), $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*/*.cpp $(SRCDIR)/*/*/*.cpp))

OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGETS): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(@:$(BINDIR)/%=$(SRCDIR)/%.cpp) $(OBJECTS) -MF $(@:$(BINDIR)/%=$(OBJDIR)/%.d) -o $@


GTEST_TARGET = $(BINDIR)/test

GTEST_SOURCES = $(wildcard $(TESTDIR)/*.cpp $(TESTDIR)/*/*.cpp $(TESTDIR)/*/*/*.cpp $(TESTDIR)/*/*/*/*.cp)

GTEST_OBJECTS = $(GTEST_SOURCES:$(TESTDIR)/%.cpp=$(OBJDIR)/$(TESTDIR)/%.o)

$(OBJDIR)/$(TESTDIR)/%.o: $(TESTDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -I$(GTESTDIR)/include/ $(CXXFLAGS) -c $< -o $@

$(GTEST_TARGET): $(OBJECTS) $(GTEST_OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(GTEST_OBJECTS) $(GTESTLIB) -o $@


DEPENDENCIES = $(MAINS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.d) $(OBJECTS:%.o=%.d) $(GTEST_OBJECTS:%.o=%.d)

.PHONY: all
all: $(TARGETS) $(GTEST_TARGET)

.PHONY: clean
clean:
	@rm -rf $(OBJDIR)
	@rm -rf $(BINDIR)

.PHONY: test
test: $(TARGETS) $(GTEST_TARGET)
	$(GTEST_TARGET)

.PHONY: dump
dump:
	@echo $(TARGETS)
	@echo $(MAINS)
	@echo $(SOURCES)
	@echo $(OBJECTS)
	@echo $(GTEST_TARGET)
	@echo $(GTEST_SOURCES)
	@echo $(GTEST_OBJECTS)
	@echo $(DEPENDENCIES)

-include $(DEPENDENCIES)
