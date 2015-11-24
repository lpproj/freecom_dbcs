CFG_DEPENDENCIES = criter.mak

TOP=..
!include "$(TOP)/config.mak"

all:  context.def criter criter1

context.def : context.x
	..\utils\mkctxt.exe context
	copy context.h_c ..
	copy context.inc ..

criter : criter.asm dmy_cbrk.asm ..\include\stuff.inc context.def
	$(NASM) $(NASMFLAGS) $(__TARGET) $(__DBCS) -f bin -o criter criter.asm

criter1 : criter.asm dmy_cbrk.asm ..\include\stuff.inc context.def
	$(NASM) $(NASMFLAGS) $(__TARGET) $(__DBCS) -DAUTO_FAIL -f bin -o criter1 criter.asm

