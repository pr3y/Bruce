/*
** Nofrendo (c) 1998-2000 Matthew Conte (matt@conte.com)
**
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of version 2 of the GNU Library General 
** Public License as published by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
**
**
** dis6502.c
**
** 6502 disassembler based on code from John Saeger
** $Id: dis6502.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include <stdio.h>

#include "../noftypes.h"
#include "nes6502.h"
#include "dis6502.h"

#ifdef NES6502_DEBUG

/* addressing modes */
enum
{
   _imp,
   _acc,
   _rel,
   _imm,
   _abs,
   _abs_x,
   _abs_y,
   _zero,
   _zero_x,
   _zero_y,
   _ind,
   _ind_x,
   _ind_y
};

/* keep a filthy local copy of PC to
** reduce the amount of parameter passing
*/
static uint32 pc_reg;

/* if we ever overrun this buffer, something will
** have gone very wrong anyway...
*/
static char disasm_buf[256];

static uint8 dis_op8(void)
{
   return (nes6502_getbyte(pc_reg + 1));
}

static uint16 dis_op16(void)
{
   return (nes6502_getbyte(pc_reg + 1) + (nes6502_getbyte(pc_reg + 2) << 8));
}

static int dis_show_ind(char *buf)
{
   return sprintf(buf, "(%04X)  ", dis_op16());
}

static int dis_show_ind_x(char *buf)
{
   return sprintf(buf, "(%02X,x)  ", dis_op8());
}

static int dis_show_ind_y(char *buf)
{
   return sprintf(buf, "(%02X),y  ", dis_op8());
}

static int dis_show_zero_x(char *buf)
{
   return sprintf(buf, " %02X,x   ", dis_op8());
}

static int dis_show_zero_y(char *buf)
{
   return sprintf(buf, " %02X,y   ", dis_op8());
}

static int dis_show_abs_y(char *buf)
{
   return sprintf(buf, " %04X,y ", dis_op16());
}

static int dis_show_abs_x(char *buf)
{
   return sprintf(buf, " %04X,x ", dis_op16());
}

static int dis_show_zero(char *buf)
{
   return sprintf(buf, " %02X     ", dis_op8());
}

static int dis_show_abs(char *buf)
{
   return sprintf(buf, " %04X   ", dis_op16());
}

static int dis_show_immediate(char *buf)
{
   return sprintf(buf, "#%02X     ", dis_op8());
}

static int dis_show_acc(char *buf)
{
   return sprintf(buf, " a      ");
}

static int dis_show_relative(char *buf)
{
   int target;

   target = (int8)dis_op8();
   target += (pc_reg + 2);
   return sprintf(buf, " %04X   ", target);
}

static int dis_show_code(char *buf, int optype)
{
   char *dest = buf + sprintf(buf, "%02X ", nes6502_getbyte(pc_reg));

   switch (optype)
   {
   case _imp:
   case _acc:
      dest += sprintf(dest, "      ");
      break;

   case _rel:
   case _imm:
   case _zero:
   case _zero_x:
   case _zero_y:
   case _ind_y:
   case _ind_x:
      dest += sprintf(dest, "%02X    ", nes6502_getbyte(pc_reg + 1));
      break;

   case _abs:
   case _abs_x:
   case _abs_y:
   case _ind:
      dest += sprintf(dest, "%02X %02X ", nes6502_getbyte(pc_reg + 1), nes6502_getbyte(pc_reg + 2));
      break;
   }

   return (int)(dest - buf);
}

static int dis_show_op(char *buf, char *opstr, int optype)
{
   char *dest = buf;

   dest += dis_show_code(dest, optype);
   dest += sprintf(dest, "%s ", opstr);

   switch (optype)
   {
   case _imp:
      dest += sprintf(dest, "        ");
      break;
   case _acc:
      dest += dis_show_acc(dest);
      break;
   case _rel:
      dest += dis_show_relative(dest);
      break;
   case _imm:
      dest += dis_show_immediate(dest);
      break;
   case _abs:
      dest += dis_show_abs(dest);
      break;
   case _abs_x:
      dest += dis_show_abs_x(dest);
      break;
   case _abs_y:
      dest += dis_show_abs_y(dest);
      break;
   case _zero:
      dest += dis_show_zero(dest);
      break;
   case _zero_x:
      dest += dis_show_zero_x(dest);
      break;
   case _zero_y:
      dest += dis_show_zero_y(dest);
      break;
   case _ind:
      dest += dis_show_ind(dest);
      break;
   case _ind_x:
      dest += dis_show_ind_x(dest);
      break;
   case _ind_y:
      dest += dis_show_ind_y(dest);
      break;
   }

   return (int)(dest - buf);
}

char *nes6502_disasm(uint32 PC, uint8 P, uint8 A, uint8 X, uint8 Y, uint8 S)
{
   char *buf = disasm_buf;
   char *op;
   int type;

   pc_reg = PC;

   buf += sprintf(buf, "%04X: ", pc_reg);

   switch (nes6502_getbyte(pc_reg))
   {
   case 0x00:
      op = "brk";
      type = _imp;
      break;
   case 0x01:
      op = "ora";
      type = _ind_x;
      break;
   case 0x02:
      op = "jam";
      type = _imp;
      break;
   case 0x03:
      op = "slo";
      type = _ind_x;
      break;
   case 0x04:
      op = "nop";
      type = _zero;
      break;
   case 0x05:
      op = "ora";
      type = _zero;
      break;
   case 0x06:
      op = "asl";
      type = _zero;
      break;
   case 0x07:
      op = "slo";
      type = _zero;
      break;
   case 0x08:
      op = "php";
      type = _imp;
      break;
   case 0x09:
      op = "ora";
      type = _imm;
      break;
   case 0x0a:
      op = "asl";
      type = _acc;
      break;
   case 0x0b:
      op = "anc";
      type = _imm;
      break;
   case 0x0c:
      op = "nop";
      type = _abs;
      break;
   case 0x0d:
      op = "ora";
      type = _abs;
      break;
   case 0x0e:
      op = "asl";
      type = _abs;
      break;
   case 0x0f:
      op = "slo";
      type = _abs;
      break;

   case 0x10:
      op = "bpl";
      type = _rel;
      break;
   case 0x11:
      op = "ora";
      type = _ind_y;
      break;
   case 0x12:
      op = "jam";
      type = _imp;
      break;
   case 0x13:
      op = "slo";
      type = _ind_y;
      break;
   case 0x14:
      op = "nop";
      type = _zero_x;
      break;
   case 0x15:
      op = "ora";
      type = _zero_x;
      break;
   case 0x16:
      op = "asl";
      type = _zero_x;
      break;
   case 0x17:
      op = "slo";
      type = _zero_x;
      break;
   case 0x18:
      op = "clc";
      type = _imp;
      break;
   case 0x19:
      op = "ora";
      type = _abs_y;
      break;
   case 0x1a:
      op = "nop";
      type = _imp;
      break;
   case 0x1b:
      op = "slo";
      type = _abs_y;
      break;
   case 0x1c:
      op = "nop";
      type = _abs_x;
      break;
   case 0x1d:
      op = "ora";
      type = _abs_x;
      break;
   case 0x1e:
      op = "asl";
      type = _abs_x;
      break;
   case 0x1f:
      op = "slo";
      type = _abs_x;
      break;

   case 0x20:
      op = "jsr";
      type = _abs;
      break;
   case 0x21:
      op = "and";
      type = _ind_x;
      break;
   case 0x22:
      op = "jam";
      type = _imp;
      break;
   case 0x23:
      op = "rla";
      type = _ind_x;
      break;
   case 0x24:
      op = "bit";
      type = _zero;
      break;
   case 0x25:
      op = "and";
      type = _zero;
      break;
   case 0x26:
      op = "rol";
      type = _zero;
      break;
   case 0x27:
      op = "rla";
      type = _zero;
      break;
   case 0x28:
      op = "plp";
      type = _imp;
      break;
   case 0x29:
      op = "and";
      type = _imm;
      break;
   case 0x2a:
      op = "rol";
      type = _acc;
      break;
   case 0x2b:
      op = "anc";
      type = _imm;
      break;
   case 0x2c:
      op = "bit";
      type = _abs;
      break;
   case 0x2d:
      op = "and";
      type = _abs;
      break;
   case 0x2e:
      op = "rol";
      type = _abs;
      break;
   case 0x2f:
      op = "rla";
      type = _abs;
      break;

   case 0x30:
      op = "bmi";
      type = _rel;
      break;
   case 0x31:
      op = "and";
      type = _ind_y;
      break;
   case 0x32:
      op = "jam";
      type = _imp;
      break;
   case 0x33:
      op = "rla";
      type = _ind_y;
      break;
   case 0x34:
      op = "nop";
      type = _imp;
      break;
   case 0x35:
      op = "and";
      type = _zero_x;
      break;
   case 0x36:
      op = "rol";
      type = _zero_x;
      break;
   case 0x37:
      op = "rla";
      type = _zero_x;
      break;
   case 0x38:
      op = "sec";
      type = _imp;
      break;
   case 0x39:
      op = "and";
      type = _abs_y;
      break;
   case 0x3a:
      op = "nop";
      type = _imp;
      break;
   case 0x3b:
      op = "rla";
      type = _abs_y;
      break;
   case 0x3c:
      op = "nop";
      type = _abs_x;
      break;
   case 0x3d:
      op = "and";
      type = _abs_x;
      break;
   case 0x3e:
      op = "rol";
      type = _abs_x;
      break;
   case 0x3f:
      op = "rla";
      type = _abs_x;
      break;

   case 0x40:
      op = "rti";
      type = _imp;
      break;
   case 0x41:
      op = "eor";
      type = _ind_x;
      break;
   case 0x42:
      op = "jam";
      type = _imp;
      break;
   case 0x43:
      op = "sre";
      type = _ind_x;
      break;
   case 0x44:
      op = "nop";
      type = _zero;
      break;
   case 0x45:
      op = "eor";
      type = _zero;
      break;
   case 0x46:
      op = "lsr";
      type = _zero;
      break;
   case 0x47:
      op = "sre";
      type = _zero;
      break;
   case 0x48:
      op = "pha";
      type = _imp;
      break;
   case 0x49:
      op = "eor";
      type = _imm;
      break;
   case 0x4a:
      op = "lsr";
      type = _acc;
      break;
   case 0x4b:
      op = "asr";
      type = _imm;
      break;
   case 0x4c:
      op = "jmp";
      type = _abs;
      break;
   case 0x4d:
      op = "eor";
      type = _abs;
      break;
   case 0x4e:
      op = "lsr";
      type = _abs;
      break;
   case 0x4f:
      op = "sre";
      type = _abs;
      break;

   case 0x50:
      op = "bvc";
      type = _rel;
      break;
   case 0x51:
      op = "eor";
      type = _ind_y;
      break;
   case 0x52:
      op = "jam";
      type = _imp;
      break;
   case 0x53:
      op = "sre";
      type = _ind_y;
      break;
   case 0x54:
      op = "nop";
      type = _zero_x;
      break;
   case 0x55:
      op = "eor";
      type = _zero_x;
      break;
   case 0x56:
      op = "lsr";
      type = _zero_x;
      break;
   case 0x57:
      op = "sre";
      type = _zero_x;
      break;
   case 0x58:
      op = "cli";
      type = _imp;
      break;
   case 0x59:
      op = "eor";
      type = _abs_y;
      break;
   case 0x5a:
      op = "nop";
      type = _imp;
      break;
   case 0x5b:
      op = "sre";
      type = _abs_y;
      break;
   case 0x5c:
      op = "nop";
      type = _abs_x;
      break;
   case 0x5d:
      op = "eor";
      type = _abs_x;
      break;
   case 0x5e:
      op = "lsr";
      type = _abs_x;
      break;
   case 0x5f:
      op = "sre";
      type = _abs_x;
      break;

   case 0x60:
      op = "rts";
      type = _imp;
      break;
   case 0x61:
      op = "adc";
      type = _ind_x;
      break;
   case 0x62:
      op = "jam";
      type = _imp;
      break;
   case 0x63:
      op = "rra";
      type = _ind_x;
      break;
   case 0x64:
      op = "nop";
      type = _zero;
      break;
   case 0x65:
      op = "adc";
      type = _zero;
      break;
   case 0x66:
      op = "ror";
      type = _zero;
      break;
   case 0x67:
      op = "rra";
      type = _zero;
      break;
   case 0x68:
      op = "pla";
      type = _imp;
      break;
   case 0x69:
      op = "adc";
      type = _imm;
      break;
   case 0x6a:
      op = "ror";
      type = _acc;
      break;
   case 0x6b:
      op = "arr";
      type = _imm;
      break;
   case 0x6c:
      op = "jmp";
      type = _ind;
      break;
   case 0x6d:
      op = "adc";
      type = _abs;
      break;
   case 0x6e:
      op = "ror";
      type = _abs;
      break;
   case 0x6f:
      op = "rra";
      type = _abs;
      break;

   case 0x70:
      op = "bvs";
      type = _rel;
      break;
   case 0x71:
      op = "adc";
      type = _ind_y;
      break;
   case 0x72:
      op = "jam";
      type = _imp;
      break;
   case 0x73:
      op = "rra";
      type = _ind_y;
      break;
   case 0x74:
      op = "nop";
      type = _zero_x;
      break;
   case 0x75:
      op = "adc";
      type = _zero_x;
      break;
   case 0x76:
      op = "ror";
      type = _zero_x;
      break;
   case 0x77:
      op = "rra";
      type = _zero_x;
      break;
   case 0x78:
      op = "sei";
      type = _imp;
      break;
   case 0x79:
      op = "adc";
      type = _abs_y;
      break;
   case 0x7a:
      op = "nop";
      type = _imp;
      break;
   case 0x7b:
      op = "rra";
      type = _abs_y;
      break;
   case 0x7c:
      op = "nop";
      type = _abs_x;
      break;
   case 0x7d:
      op = "adc";
      type = _abs_x;
      break;
   case 0x7e:
      op = "ror";
      type = _abs_x;
      break;
   case 0x7f:
      op = "rra";
      type = _abs_x;
      break;

   case 0x80:
      op = "nop";
      type = _imm;
      break;
   case 0x81:
      op = "sta";
      type = _ind_x;
      break;
   case 0x82:
      op = "nop";
      type = _imm;
      break;
   case 0x83:
      op = "sax";
      type = _ind_x;
      break;
   case 0x84:
      op = "sty";
      type = _zero;
      break;
   case 0x85:
      op = "sta";
      type = _zero;
      break;
   case 0x86:
      op = "stx";
      type = _zero;
      break;
   case 0x87:
      op = "sax";
      type = _zero;
      break;
   case 0x88:
      op = "dey";
      type = _imp;
      break;
   case 0x89:
      op = "nop";
      type = _imm;
      break;
   case 0x8a:
      op = "txa";
      type = _imp;
      break;
   case 0x8b:
      op = "ane";
      type = _imm;
      break;
   case 0x8c:
      op = "sty";
      type = _abs;
      break;
   case 0x8d:
      op = "sta";
      type = _abs;
      break;
   case 0x8e:
      op = "stx";
      type = _abs;
      break;
   case 0x8f:
      op = "sax";
      type = _abs;
      break;

   case 0x90:
      op = "bcc";
      type = _rel;
      break;
   case 0x91:
      op = "sta";
      type = _ind_y;
      break;
   case 0x92:
      op = "jam";
      type = _imp;
      break;
   case 0x93:
      op = "sha";
      type = _ind_y;
      break;
   case 0x94:
      op = "sty";
      type = _zero_x;
      break;
   case 0x95:
      op = "sta";
      type = _zero_x;
      break;
   case 0x96:
      op = "stx";
      type = _zero_y;
      break;
   case 0x97:
      op = "sax";
      type = _zero_y;
      break;
   case 0x98:
      op = "tya";
      type = _imp;
      break;
   case 0x99:
      op = "sta";
      type = _abs_y;
      break;
   case 0x9a:
      op = "txs";
      type = _imp;
      break;
   case 0x9b:
      op = "shs";
      type = _abs_y;
      break;
   case 0x9c:
      op = "shy";
      type = _abs_x;
      break;
   case 0x9d:
      op = "sta";
      type = _abs_x;
      break;
   case 0x9e:
      op = "shx";
      type = _abs_y;
      break;
   case 0x9f:
      op = "sha";
      type = _abs_y;
      break;

   case 0xa0:
      op = "ldy";
      type = _imm;
      break;
   case 0xa1:
      op = "lda";
      type = _ind_x;
      break;
   case 0xa2:
      op = "ldx";
      type = _imm;
      break;
   case 0xa3:
      op = "lax";
      type = _ind_x;
      break;
   case 0xa4:
      op = "ldy";
      type = _zero;
      break;
   case 0xa5:
      op = "lda";
      type = _zero;
      break;
   case 0xa6:
      op = "ldx";
      type = _zero;
      break;
   case 0xa7:
      op = "lax";
      type = _zero;
      break;
   case 0xa8:
      op = "tay";
      type = _imp;
      break;
   case 0xa9:
      op = "lda";
      type = _imm;
      break;
   case 0xaa:
      op = "tax";
      type = _imp;
      break;
   case 0xab:
      op = "lxa";
      type = _imm;
      break;
   case 0xac:
      op = "ldy";
      type = _abs;
      break;
   case 0xad:
      op = "lda";
      type = _abs;
      break;
   case 0xae:
      op = "ldx";
      type = _abs;
      break;
   case 0xaf:
      op = "lax";
      type = _abs;
      break;

   case 0xb0:
      op = "bcs";
      type = _rel;
      break;
   case 0xb1:
      op = "lda";
      type = _ind_y;
      break;
   case 0xb2:
      op = "jam";
      type = _imp;
      break;
   case 0xb3:
      op = "lax";
      type = _ind_y;
      break;
   case 0xb4:
      op = "ldy";
      type = _zero_x;
      break;
   case 0xb5:
      op = "lda";
      type = _zero_x;
      break;
   case 0xb6:
      op = "ldx";
      type = _zero_y;
      break;
   case 0xb7:
      op = "lax";
      type = _zero_y;
      break;
   case 0xb8:
      op = "clv";
      type = _imp;
      break;
   case 0xb9:
      op = "lda";
      type = _abs_y;
      break;
   case 0xba:
      op = "tsx";
      type = _imp;
      break;
   case 0xbb:
      op = "las";
      type = _abs_y;
      break;
   case 0xbc:
      op = "ldy";
      type = _abs_x;
      break;
   case 0xbd:
      op = "lda";
      type = _abs_x;
      break;
   case 0xbe:
      op = "ldx";
      type = _abs_y;
      break;
   case 0xbf:
      op = "lax";
      type = _abs_y;
      break;

   case 0xc0:
      op = "cpy";
      type = _imm;
      break;
   case 0xc1:
      op = "cmp";
      type = _ind_x;
      break;
   case 0xc2:
      op = "nop";
      type = _imm;
      break;
   case 0xc3:
      op = "dcp";
      type = _ind_x;
      break;
   case 0xc4:
      op = "cpy";
      type = _zero;
      break;
   case 0xc5:
      op = "cmp";
      type = _zero;
      break;
   case 0xc6:
      op = "dec";
      type = _zero;
      break;
   case 0xc7:
      op = "dcp";
      type = _zero;
      break;
   case 0xc8:
      op = "iny";
      type = _imp;
      break;
   case 0xc9:
      op = "cmp";
      type = _imm;
      break;
   case 0xca:
      op = "dex";
      type = _imp;
      break;
   case 0xcb:
      op = "sbx";
      type = _imm;
      break;
   case 0xcc:
      op = "cpy";
      type = _abs;
      break;
   case 0xcd:
      op = "cmp";
      type = _abs;
      break;
   case 0xce:
      op = "dec";
      type = _abs;
      break;
   case 0xcf:
      op = "dcp";
      type = _abs;
      break;

   case 0xd0:
      op = "bne";
      type = _rel;
      break;
   case 0xd1:
      op = "cmp";
      type = _ind_y;
      break;
   case 0xd2:
      op = "jam";
      type = _imp;
      break;
   case 0xd3:
      op = "dcp";
      type = _ind_y;
      break;
   case 0xd4:
      op = "nop";
      type = _zero_x;
      break;
   case 0xd5:
      op = "cmp";
      type = _zero_x;
      break;
   case 0xd6:
      op = "dec";
      type = _zero_x;
      break;
   case 0xd7:
      op = "dcp";
      type = _zero_x;
      break;
   case 0xd8:
      op = "cld";
      type = _imp;
      break;
   case 0xd9:
      op = "cmp";
      type = _abs_y;
      break;
   case 0xda:
      op = "nop";
      type = _imp;
      break;
   case 0xdb:
      op = "dcp";
      type = _abs_y;
      break;
   case 0xdc:
      op = "nop";
      type = _abs_x;
      break;
   case 0xdd:
      op = "cmp";
      type = _abs_x;
      break;
   case 0xde:
      op = "dec";
      type = _abs_x;
      break;
   case 0xdf:
      op = "dcp";
      type = _abs_x;
      break;

   case 0xe0:
      op = "cpx";
      type = _imm;
      break;
   case 0xe1:
      op = "sbc";
      type = _ind_x;
      break;
   case 0xe2:
      op = "nop";
      type = _imm;
      break;
   case 0xe3:
      op = "isb";
      type = _ind_x;
      break;
   case 0xe4:
      op = "cpx";
      type = _zero;
      break;
   case 0xe5:
      op = "sbc";
      type = _zero;
      break;
   case 0xe6:
      op = "inc";
      type = _zero;
      break;
   case 0xe7:
      op = "isb";
      type = _zero;
      break;
   case 0xe8:
      op = "inx";
      type = _imp;
      break;
   case 0xe9:
      op = "sbc";
      type = _imm;
      break;
   case 0xea:
      op = "nop";
      type = _imp;
      break;
   case 0xeb:
      op = "sbc";
      type = _imm;
      break;
   case 0xec:
      op = "cpx";
      type = _abs;
      break;
   case 0xed:
      op = "sbc";
      type = _abs;
      break;
   case 0xee:
      op = "inc";
      type = _abs;
      break;
   case 0xef:
      op = "isb";
      type = _abs;
      break;

   case 0xf0:
      op = "beq";
      type = _rel;
      break;
   case 0xf1:
      op = "sbc";
      type = _ind_y;
      break;
   case 0xf2:
      op = "jam";
      type = _imp;
      break;
   case 0xf3:
      op = "isb";
      type = _ind_y;
      break;
   case 0xf4:
      op = "nop";
      type = _zero_x;
      break;
   case 0xf5:
      op = "sbc";
      type = _zero_x;
      break;
   case 0xf6:
      op = "inc";
      type = _zero_x;
      break;
   case 0xf7:
      op = "isb";
      type = _zero_x;
      break;
   case 0xf8:
      op = "sed";
      type = _imp;
      break;
   case 0xf9:
      op = "sbc";
      type = _abs_y;
      break;
   case 0xfa:
      op = "nop";
      type = _imp;
      break;
   case 0xfb:
      op = "isb";
      type = _abs_y;
      break;
   case 0xfc:
      op = "nop";
      type = _abs_x;
      break;
   case 0xfd:
      op = "sbc";
      type = _abs_x;
      break;
   case 0xfe:
      op = "inc";
      type = _abs_x;
      break;
   case 0xff:
      op = "isb";
      type = _abs_x;
      break;
   }

   buf += dis_show_op(buf, op, type);

   buf += sprintf(buf, "%c%c1%c%c%c%c%c %02X %02X %02X %02X\n",
                  (P & N_FLAG) ? 'N' : '.',
                  (P & V_FLAG) ? 'V' : '.',
                  (P & B_FLAG) ? 'B' : '.',
                  (P & D_FLAG) ? 'D' : '.',
                  (P & I_FLAG) ? 'I' : '.',
                  (P & Z_FLAG) ? 'Z' : '.',
                  (P & C_FLAG) ? 'C' : '.',
                  A, X, Y, S);

   return disasm_buf;
}

#endif /* NES6502_DEBUG */

/*
** $Log: dis6502.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1  2001/04/27 12:54:39  neil
** blah
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.11  2000/08/30 14:49:25  matt
** changed output of flags to be more readable
**
** Revision 1.10  2000/08/28 12:53:44  matt
** fixes for disassembler
**
** Revision 1.9  2000/07/25 10:49:38  matt
** fixed zero page Y output
**
** Revision 1.8  2000/07/17 01:52:28  matt
** made sure last line of all source files is a newline
**
** Revision 1.7  2000/07/11 04:26:23  matt
** rewrote to fill up a static buffer, rather than use log_printf
**
** Revision 1.6  2000/07/10 05:15:58  matt
** fixed a bug in indirect x/y disassembly
**
** Revision 1.5  2000/07/05 12:06:23  matt
** cosmetic changes
**
** Revision 1.4  2000/06/09 15:12:25  matt
** initial revision
**
*/
