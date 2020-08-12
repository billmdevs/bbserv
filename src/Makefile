IDIR =.
CC=g++
CFLAGS=-I$(IDIR) -Wall -pedantic -std=c++17

ODIR=obj
LDIR =../lib

LIBS=-lm -pthread

wDEPS = AcknowledgeQueue.h AutoLock.h BBServException.h BBServTimeout.h BroadcastCommit.h BroadcastPrecommit.h BroadcastUnsuccessful.h CmdAcknowledge.h CmdBuilder.h CmdCommit.h CmdPrecommit.h CmdQuit.h CmdRead.h CmdReplace.h CmdSuccessful.h CmdUnsuccessful.h CmdUser.h CmdWrite.h Config.h ConnectionQueue.h InConnection.h RWLock.h SessionResources.h ThreadPool.h UndoStore.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = AcknowledgeQueue.o BroadcastCommit.o BroadcastPrecommit.o BroadcastUnsuccessful.o CmdAcknowledge.o CmdBuilder.o CmdCommit.o CmdPrecommit.o CmdQuit.o CmdRead.o CmdReplace.o CmdSuccessful.o CmdUnsuccessful.o CmdUser.o CmdWrite.o Config.o ConnectionQueue.o InConnection.o main.o RWLock.o ThreadPool.o UndoStore.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

bbserv: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
	touch bbserv

