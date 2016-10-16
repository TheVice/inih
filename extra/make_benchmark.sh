g++ -g -O2 -o INIReaderTest ../cpp/INIReaderTest.cpp ../cpp/INIReader.cpp ../ini.c
g++ -g -O2 -DPROPOSED -o INIReaderTest_proposed ../cpp/INIReaderTest.cpp ../cpp/proposed/INIReader.cpp ../ini.c