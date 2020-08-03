IDIR =.
CC=g++
CFLAGS=-I$(IDIR) -Wall -pedantic -std=c++17

ODIR=obj
LDIR =../lib

LIBS=-lm -pthread

_DEPS = AcknowledgeQueue.h AutoLock.h BBServException.h BBServTimeout.h BroadcastCommit.h BroadcastPrecommit.h CmdAcknowledge.h CmdBuilder.h CmdCommit.h CmdPrecommit.h CmdQuit.h CmdRead.h CmdReplace.h CmdSuccessful.h CmdUser.h CmdWrite.h Config.h ConnectionQueue.h InConnection.h SessionResources.h ThreadPool.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = AcknowledgeQueue.o BroadcastCommit.o BroadcastPrecommit.o CmdAcknowledge.o CmdBuilder.o CmdCommit.o CmdPrecommit.o CmdQuit.o CmdRead.o CmdReplace.o CmdSuccessful.o CmdUser.o CmdWrite.o Config.o ConnectionQueue.o InConnection.o main.o ThreadPool.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

bbserv: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
	touch bbserv

