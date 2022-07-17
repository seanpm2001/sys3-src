#
char AS3[] = "@(#)as3.c 1.11 79/05/30 15:10:02";	/* sccs ident */
#include <stdio.h>
#include "as.h"
#include "as.yh"

readonly int revtab[] = {
0x00,0x80,0x40,0xc0,0x20,0xa0,0x60,0xe0,0x10,0x90,0x50,0xd0,0x30,0xb0,0x70,0xf0,
0x08,0x88,0x48,0xc8,0x28,0xa8,0x68,0xe8,0x18,0x98,0x58,0xd8,0x38,0xb8,0x78,0xf8,
0x04,0x84,0x44,0xc4,0x24,0xa4,0x64,0xe4,0x14,0x94,0x54,0xd4,0x34,0xb4,0x74,0xf4,
0x0c,0x8c,0x4c,0xcc,0x2c,0xac,0x6c,0xec,0x1c,0x9c,0x5c,0xdc,0x3c,0xbc,0x7c,0xfc,
0x02,0x82,0x42,0xc2,0x22,0xa2,0x62,0xe2,0x12,0x92,0x52,0xd2,0x32,0xb2,0x72,0xf2,
0x0a,0x8a,0x4a,0xca,0x2a,0xaa,0x6a,0xea,0x1a,0x9a,0x5a,0xda,0x3a,0xba,0x7a,0xfa,
0x06,0x86,0x46,0xc6,0x26,0xa6,0x66,0xe6,0x16,0x96,0x56,0xd6,0x36,0xb6,0x76,0xf6,
0x0e,0x8e,0x4e,0xce,0x2e,0xae,0x6e,0xee,0x1e,0x9e,0x5e,0xde,0x3e,0xbe,0x7e,0xfe,
};

#define OP(a,b,c,d,e,f,g,h,i) {a,b,(c==0 ? INST0-256:INSTn-256),c,d,e,f,g,h,i}

readonly struct instab instab[] = {
{".space", 0, ISPACE-256},
{".byte", 0, IBYTE-256},
{".word", 0, IWORD-256},
{".long", 0, ILONG-256},
{".int", 0, IINT-256},
{".data", 0, IDATA-256},
{".globl", 0, IGLOBAL-256},
{".set", 0, ISET-256},
{".text", 0, ITEXT-256},
{".comm", 0, ICOMM-256},
{".lcomm", 0, ILCOMM-256},
{".lsym", 0, ILSYM-256},
{".align", 0, IALIGN-256},
{".float", 0, IFLOAT-256},
{".double", 0, IDOUBLE-256},
{".org", 0, IORG-256},
{".stab", 0, ISTAB-256},
{"r0",0,REG-256},
{"r1",1,REG-256},
{"r2",2,REG-256},
{"r3",3,REG-256},
{"r4",4,REG-256},
{"r5",5,REG-256},
{"r6",6,REG-256},
{"r7",7,REG-256},
{"r8",8,REG-256},
{"r9",9,REG-256},
{"r10",10,REG-256},
{"r11",11,REG-256},
{"r12",12,REG-256},
{"r13",13,REG-256},
{"r14",14,REG-256},
{"r15",15,REG-256},
{"ap",12,REG-256},
{"fp",13,REG-256},
{"sp",14,REG-256},
{"pc",15,REG-256},
{"jcc",0x1e,IJXXX-256},
{"jcs",0x1f,IJXXX-256},
{"jeql",0x13,IJXXX-256},
{"jeqlu",0x13,IJXXX-256},
{"jgeq",0x18,IJXXX-256},
{"jgequ",0x1e,IJXXX-256},
{"jgtr",0x14,IJXXX-256},
{"jgtru",0x1a,IJXXX-256},
{"jleq",0x15,IJXXX-256},
{"jlequ",0x1b,IJXXX-256},
{"jlss",0x19,IJXXX-256},
{"jlssu",0x1f,IJXXX-256},
{"jneq",0x12,IJXXX-256},
{"jnequ",0x12,IJXXX-256},
{"jvc",0x1c,IJXXX-256},
{"jvs",0x1d,IJXXX-256},
{"jbr",0x11,IJXXX-256},
{"jbc",0xe1,IJXXX-256},
{"jbs",0xe0,IJXXX-256},
{"jbcc",0xe5,IJXXX-256},
{"jbsc",0xe4,IJXXX-256},
{"jbcs",0xe3,IJXXX-256},
{"jbss",0xe2,IJXXX-256},
{"jlbc",0xe9,IJXXX-256},
{"jlbs",0xe8,IJXXX-256},
#include "instrs"
0
};
