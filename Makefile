include $(ROOTSYS)/etc/Makefile.arch


CXXFLAGS += -I.

ifeq ($(PLATFORM),macosx)
CXXFLAGS += -std=c++11
endif


# TARGET=libTreeAnalysis.so
# STARGET=libTreeAnalysis.a

TARGET=HepKDE.so


ASKDE  = src

SOURCE = $(wildcard $(ASKDE)/*.cc)
OBJ=$(join $(addsuffix ../obj/, $(dir $(SOURCE))), $(notdir $(SOURCE:.cc=.o))) 
DEPENDS=$(join $(addsuffix ../.dep/, $(dir $(SOURCE))), $(notdir $(SOURCE:.cc=.d)))

all:  $(TARGET)
	@true
clean:
	@-rm -f $(TARGET) $(OBJ) $(DEPENDS)

$(TARGET): $(OBJ)
	@echo "============="
	@echo "Linking the target $@"
	@echo "============="
ifeq ($(PLATFORM),macosx)
		$(LD) $(SOFLAGS)$@ $(LDFLAGS) $^ $(OutPutOpt) $@ $(EXPLLINKLIBS)
else
		$(LD) $(SOFLAGS) $(LDFLAGS) $^ $(OutPutOpt) $@ $(EXPLLINKLIBS) $(ROOTLIBS)
endif
	@echo -- Link finished --
        
%.o : %.cc
	@mkdir -p $(dir $@)
	@echo "============="
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $<  -o $@

$(ASKDE)/../obj/%.o : $(ASKDE)/%.cc
	@mkdir -p $(dir $@)
	@echo "============="
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $<  -o $@

-include $(DEPENDS)
