# Kamil Breczko (280 990)

CXXFLAGS = -std=c++11 -Wall -Wextra

CXX = g++

main : main.o OutputFile.o Packet.h Receiver.o Sender.o Transport.o Sockwrap.o
	$(CXX) $(CXXFLAGS) -o transport main.o OutputFile.o Receiver.o Sender.o Transport.o Sockwrap.o

main.o : main.cpp
	$(CXX) -c $(CXXFLAGS) main.cpp -o  main.o

Transport.o : Transport.cpp Transport.h
	$(CXX) -c $(CXXFLAGS) Transport.cpp -o Transport.o

Receiver.o : Receiver.cpp Receiver.h Packet.h
	$(CXX) -c $(CXXFLAGS) Receiver.cpp -o  Receiver.o

Sender.o : Sender.cpp Sender.h
	$(CXX) -c $(CXXFLAGS) Sender.cpp -o  Sender.o

OutputFile.o : OutputFile.cpp OutputFile.h
	$(CXX) -c $(CXXFLAGS) OutputFile.cpp -o  OutputFile.o

Sockwrap.o : Sockwrap.cpp Sockwrap.h
	$(CXX) -c $(CXXFLAGS) Sockwrap.cpp -o  Sockwrap.o

clean:
	rm -rf *.o

distclean:
	rm -rf *.o router .*.swp