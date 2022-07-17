LIBNAME = ../lib0
FRC =

FILES =\
	$(LIBNAME)(dump_ht.o)\
	$(LIBNAME)(dump_tm.o)\
	$(LIBNAME)(dump_rk.o)\
	$(LIBNAME)(dump_hp.o)\
	$(LIBNAME)(dump_rl.o)\
	$(LIBNAME)(dump_rp.o)

all:	machines $(LIBNAME)

.PRECIOUS:	$(LIBNAME)

$(LIBNAME):	$(FILES) $(FRC)

machines:
	cd mch_id; make -f mch_id.mk FRC=$(FRC)
	cd mch_i; make -f mch_i.mk FRC=$(FRC)

clean:
	-rm -f *.o
	cd mch_id; make -f mch_id.mk clean
	cd mch_i; make -f mch_i.mk clean

clobber:	clean
	-rm -f $(LIBNAME)
	cd mch_id; make -f mch_id.mk clobber
	cd mch_i; make -f mch_i.mk clobber

FRC:

.s.a:
	$(AS) $(ASFLAG) -o $*.o $*.s
	ar rv $@ $*.o
	-rm -f $*.o
