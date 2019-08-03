all: fmulib.a

clean:
	rm -f fmulib.a
	rm -f *.o

SHARED_SRCS = fmuinc/fmu_fun.c

CPP_SRCS = \
	fmuinc/parser/XmlElement.cpp \
	fmuinc/parser/XmlParser.cpp \
	fmuinc/parser/XmlParserCApi.cpp

# Dependencies shared between both fmusim_cs and fmusim_me
SHARED_DEPS = \
	fmuinc/fmi2.h \
	fmuinc/include/fmi2Functions.h \
	fmuinc/include/fmi2FunctionTypes.h \
	fmuinc/include/fmi2TypesPlatform.h \
	fmuinc/parser/fmu20/XmlElement.h \
	fmuinc/parser/fmu20/XmlParser.h \
	fmuinc/parser/fmu20/XmlParserException.h \
	fmuinc/parser/XmlParserCApi.h 

CXX=c++

fmulib.a: $(SHARED_DEPS)
	$(CC) $(CFLAGS) -c -g -Wall -DFMI_COSIMULATION \
		-DSTANDALONE_XML_PARSER -DLIBXML_STATIC \
		-Ifmuinc/include -Ifmuinc/parser -Ifmuinc \
		$(SHARED_SRCS)

	$(CXX) $(CFLAGS) -c -g -Wall -DFMI_COSIMULATION \
		-DSTANDALONE_XML_PARSER -DLIBXML_STATIC \
		-Ifmuinc/include -Ifmuinc/parser -Ifmuinc \
		$(CPP_SRCS)

	ar -r fmulib.a *.o

install:
	mv fmulib.a ./lib
	rm -f *.o
