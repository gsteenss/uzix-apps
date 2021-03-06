/*
 * t r . c	23.06.94
 *
 * �ணࠬ�� �८�ࠧ������ ᨬ�����
 *
 * TR ������� �⠭����� ���� �� �⠭����� �뢮� � ����饭��� ���
 * 㭨�⮦����� ��࠭��� ᨬ�����.
 *
 * FROM � TO - ��ப� ᨬ�����. ��䥪� TR ������ �� ����� ����� ��ப.
 *
 * �᫨ FROM � TO ���������� �����, ᨬ���� �� TO ������� ᨬ����
 * �� FROM. ���
 *
 *	TR abcd wxyz +start -end
 *
 * ������� �� a � w, b � x, � �.�. TR ��������� �㪢� �� ॣ���ࠬ.
 *
 * �᫨ TO - ����� ��ப�, �� ᨬ���� �� FROM 㭨�⮦�����.
 *
 * �᫨ TO �� ����, �� ����, 祬 FROM, �� ᨬ���� �� FROM, ��
 * ����騥 ���� � TO, ���� �������� �� ��᫥���� ᨬ��� �� TO,
 * �� �� ��᫥����⥫쭮��� �࠭᫨�㥬�� ⠪�� ��ࠧ�� ᨬ�����
 * �ᥪ����� �� ������ १������饣� ᨬ����. ���:
 *
 *	TR abcde12345 ABCDE-
 *
 * ������� "aAbB1234cCdD5678eEfF" �� "AABB-CCDD-678EEfF".
 *
 * �᫨ ᨬ��� ����砥��� � FROM ����� ������ ࠧ�, �ᯮ������
 * ᠬ�� ����� ������.  ���, ��� ��������� ��� ��ப �� '*' � ����
 * ��������, �ᯮ���� TR ** *.
 *
 * � ���������� � ����� ᨬ�����, FROM � TO ����� �������:
 *
 * �᪥���
 *	\x ���� ᠬ x, �� \b - ���ᯥ��, \f - ��䨤, \n - ����䨤,
 *	\r - ����, \t - ⠡����, \s - �஡�� � \DDD - ᨬ��� �
 *	���쬥��� ����� DDD (�ᯮ������ ⮫쪮 8 ���, ��� ����� ����
 *	� ����, � ���, �ᯮ�짮����� \0 ��뢠�� ���।᪠�㥬� १����).
 *	�㪢� b, f � �.�. ����� ���� � ���� ॣ�����.
 *
 * ���������
 *	a-e ���������⭮ abcde, � �.�.	�������, �� e-a �����⨬�,
 *	�� ���⮩ ��������.
 *
 * ������
 *	:z	���⮩ ��������
 *	:a	���������⭮ a-zA-Z
 *	:l	���������⭮ a-z
 *	:u	���������⭮ A-Z
 *	:m	���������⭮ �-�
 *	:b	���������⭮ �-�
 *	:r	���������⭮ �-�-�
 *	:d	���������⭮ 0-9
 *	:n	���������⭮ a-zA-Z0-9
 *	:s	�������� �� CTRL/A �� �஡��� (001-040)
 *	:.	�� ASCII ᨬ����, �஬� 0 (001-0377)
 *
 *	���: TR ":a:." "A-ZA-Z "
 *	�࠭᫨��� �� ��⨭᪨� �㪢� � ���孨� ॣ����, � ।�����
 *	�� �᫥����⥫쭮�� ��-�㪢 � ���� �஡��
 *
 *	�����䨪���� ����� ����� ���� � �� ॣ����.
 *
 * ���栭��
 *	�᫨ ���� ᨬ��� FROM ���� '^', �� ����砥� �� ᨬ���� ��
 *	�� FROM. � �⮬ ��砥 TO ������ ���� ���� ��� ⮫쪮 �� ������
 *	ᨬ����.
 *
 * '\' and ':' ����� ᢮� ᯥ槭�祭��, �᫨ ��� ��᫥���� ᨬ��� FROM
 * ��� TO; '-' - �᫨ �� ���� ᨬ���; '^' - �᫨ �� �� ���� ᨬ���
 * FROM; �� ᨬ��� ���� ᯥ槭�祭�� ��᫥ �᪥��� ('\').
 *
 * This program is licensed under GNU GPL license.
 *
 */
 
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define SSIZE	512	/* ࠧ��� ��ࠧ� */
#define ESCAPE	'\\'	/* ᨬ��� �᪥��� */
#define NOT	'^'	/* ᨬ��� ���栭�� */
#define PCLASS	':'	/* ��䨪� ����� */
#define RANGE	'-'	/* ᯥ�䨪��� ��������� */

char *hlp[] = {
	"usage: tr FROM TO [+START] [-END] [filein [fileout]]",
	"\tSTART - starting position for transliteration",
	"\tEND - ending position for transliteration",
	"\tlen(FROM) == len(TO):",
	"\t\tchars in TO are substitutes for chars in FROM",
	"\tlen(TO) == 0:",
	"\t\tany chars in FROM are deleted",
	"\tlen(TO) < len(FROM):",
	"\t\tall chars in FROM beyond the last one to match",
	"\t\tup with one in TO translate into the last char",
	"\t\tin TO; BUT any stream of consecutive characters so",
	"\t\ttranslated is reduced to just one occurence of the",
	"\t\tresulting char.",
	"\tEscapes: \\r \\n \\t \\f \\b \\s \\ddd",
	"\tRanges: a-e is the same as abcde (e-a is empty range)",
	"\tClasses:",
	"\t\t:z empty range",
	"\t\t:a is the same as a-zA-Z",
	"\t\t:l is the same as a-z",
	"\t\t:u is the same as A-Z",
	"\t\t:m is the same as �-�",
	"\t\t:b is the same as �-�",
	"\t\t:r is the same as �-�-�",
	"\t\t:d is the same as 0-9",
	"\t\t:n is the same as a-zA-Z0-9",
	"\t\t:s is the range \\001-\\040",
	"\t\t:. is the range including all ASCII other then \\0",
	"\tNegation: if the first character of FROM is '^',",
	"\t\tany characters not in FROM match. In this case, TO",
	"\t\tmust be null or only a single char",
	0
};

void help(void) {
	register int i = 0;
	while (hlp[i])
		fprintf(stderr,"%s\n",hlp[i++]);
	exit(0);
}

void error(char *s1,char *s2) {
	fprintf(stderr, "tr: %s %s\n", s1, s2 ? s2 : "");
	exit(1);
}

int xindex(unsigned char *array, unsigned char c, int allbut, int lastto) {
	register unsigned char *index;

	for (index = array; *index; ++index)
		if (*index == c)
			break;
	if (allbut == 0)
		return(*index ? (index - array) : (-1));
	if (*index)
		return(-1);
	return(lastto+1);
}

/* ��⠢�� ᨬ����� � lo �� hi (�����⥫쭮) � ttb */
char *insrange(int lo, int hi, char *ttb) {
	for (lo &= 0377, hi &= 0377; lo <= hi; *ttb++ = lo++)
		;
	return ttb;
}

/* ��ࠡ�⪠ �᪥����
 *
 *	char esc(ppc)
 *	char **ppc;
 *
 * esc(ppc) �ᯮ����� � ��ࠡ��뢠�� escaped-ᨬ����.
 * �᫨ *ppc 㪠�뢠�� �� �� '\', esc() ���� ��୥� ᨬ���.
 * ����, �᫨ ᫥���騩 ᨬ��� �� ������⢠ {b,f,n,r,t,s}, �����頥���
 * ᮮ⢥�����騩 ���. �᫨ ᫥���騩 ᨬ��� - ���쬥�筠� ���,
 * ��ࠡ��뢠���� \ddd, �����頥��� ����祭��� ���祭��.
 *
 * �᫨ '\' - ��᫥���� ᨬ���, ᠬ '\' �����頥���.
 *
 * �᫨ '\' �।����� ��� ��㯮���⮬� x, x ᠬ �����頥���.
 *
 * �� ��� �����, *ppc 㪠�뢠�� �� ��᫥���� ᨬ���, "�ண��祭��" esc(),
 * ���ਬ�� 't' �᫨ ���㫨 <TAB> ��� '\t'.
 */
char esc(char **ppc) {
	char c, c1;
	char *pc = *ppc;

	if ((c = *pc) != ESCAPE || pc[1] == 0)
		return(c);
	else {
		c = *++pc;
		c1 = tolower(c);
		*ppc = pc;
		if (c1 == 'b')
			return('\b');
		if (c1 == 'f')
			return('\f');
		if (c1 == 'n')
			return('\n');
		if (c1 == 'r')
			return('\r');
		if (c1 == 't')
			return('\t');
		if (c1 == 's')
			return('\040');
		if (c1 >= '0' && c1 <= '7') {
			c -= '0';
			c1 = pc[1];
			if (c1 >= '0' && c1 <= '7') {
				c = (c << 3) + (c1 - '0');
				pc++;
				c1 = pc[1];
				if (c1 >= '0' && c1 <= '7') {
					c = (c << 3) + (c1 - '0');
					pc++;
				}
			}
			*ppc = pc;
			return (c & 0377);
		}
		return (c);
	}
}

/* ����७�� ��������� �� ��᫥����� ᨬ���� ttb �� ᫥���饣�
 * ᨬ���� � *ps (����� ���� "escaped").
 */
char *dorange(char **ps, char *ttb) {
	(*ps)++;				/* �ய�� RANGE */
	return insrange(ttb[-1]+1,esc(ps),ttb);
}

/* ����஥��� ttb (⠡���� �࠭��樨) �� array. */
char *makttb(char *a, char *ttb, char *xxx) {
	int c;
	char *t = ttb;

	for (; (c = *a) != 0; ++a) {
		if ((ttb - t) >= SSIZE)
			error("Expansion to long for",xxx);
		if (c == ESCAPE)
			*ttb++ = esc(&a);
		else if (c == PCLASS && a[1]) {
			switch (c = tolower(*++a)) {
			case 'z':
				break;
			case 'n':
				ttb = insrange('0','9',ttb);
			case 'a':
				ttb = insrange('A','Z',ttb);
			case 'l':
				ttb = insrange('a','z',ttb);
				break;
			case 'u':
				ttb  = insrange('A','Z',ttb);
				break;
			case 'r':
				ttb = insrange('�','�',ttb);
			case 'm':
				ttb = insrange('�','�',ttb);
				ttb = insrange('�','�',ttb);
				break;
			case 'b':
				ttb = insrange('�','�',ttb);
				break;
			case 'd':
				ttb = insrange('0','9',ttb);
				break;
			case 's':
				ttb = insrange(1,' ',ttb);
				break;
			case '.':
				ttb = insrange(1,0377,ttb);
				break;
			default:
				error("Unknown class type",a-1);
			}
		}
		else if (*a == RANGE && ttb != t && a[1])
			ttb = dorange(&a,ttb);
		else	*ttb++ = *a;
	}
	*ttb = 0;
	return t;
}

char	*froms,
	*tos;
int	allbut,
	collapse,
	expander,
	lastto;
int	bp = 0,
	ep = 9999;
FILE	*fin = stdin,
	*fout = stdout;

void process(void) {
	int pos = 0;
	int i, c;

	while ((c = getc(fin)) != EOF) {
		pos = (c == '\t') ? (pos+8-(pos&7)) : pos+1;
		if (pos-1 < bp || (ep > 0 && pos-1 > ep)) {
			putc(c,fout);
			goto Cont;
		}
		i = xindex(froms,c,allbut,lastto);
		if (collapse && i >= lastto && lastto >= 0) { /* ������� */
			putc(tos[lastto],fout);
			do {
				i = xindex(froms,c = getc(fin),allbut,lastto);
				pos = (c == '\t') ? (pos+8-(pos&7)) : pos+1;
			} while (c != EOF && i >= lastto);
			if (c == EOF)
				break;
		}
		if (i >= 0) {
			if (lastto >= 0)
				putc(tos[i],fout);	/* �࠭���� */
		}					/* ���� - 稪��� */
		else	putc(c,fout);			/* �����㥬 */

Cont:		if (c == '\n')
			pos = 0;
	}
}

void main(int argc, char *argv[]) {
	int i, c;
	char *nfin = NULL, *nfout = NULL;
	char *from, *to;

	if (argc < 3)
		help();
	froms = argv[1];
	tos = argv[2];

	for (i = 3; i < argc; i++) {
		to = argv[i];
		c = *to++;
		if (c  == '+')
			bp = atoi(to);
		else if (c == '-')
			ep = atoi(to);
		else if (nfin == NULL)
			nfin = argv[i];
		else if (nfout == NULL)
			nfout = argv[i];
		else	error("Bad option",argv[i]);
	}
	if (bp >= ep)
		error("Illegal +START/-END parameters",0);
	if (nfin != NULL && (fin = fopen(nfin,"rb")) == NULL)
		error("Can't open file",nfin);
	if (nfout != NULL && (fout = fopen(nfout,"wb")) == NULL)
		error("Can't create file",nfout);
	setvbuf(fin,NULL,_IOFBF,0x7000);
	setvbuf(fout,NULL,_IOFBF,0x7000);
	if ((allbut = (*froms == NOT)) != 0)
		froms++;
	from = malloc(SSIZE+1);
	to = malloc(SSIZE+1);
	if (!from || !to)
		error("Not enough memory",0);
	froms = makttb(froms, from, "FROM");
	if (!tos)
		*to = 0;
	else	makttb(tos, to, "TO");
	tos = to;
	lastto = strlen(tos) - 1;
	collapse = (strlen(froms)-1 > lastto || allbut);
	expander = (strlen(froms)-1 < lastto);

#ifdef DEBUG
	fprintf(stderr,"%s%s, allbut %d, lastto %d\n",
		collapse?"Collapse":"",
		expander?"Expander":"",
		allbut,lastto);
	fprintf(stderr,"to: %s\n",tos);
	fprintf(stderr,"from: %s\n",froms);
	fprintf(stderr,"bp = %d, ep = %d\n",bp,ep);
#endif
	process();
	exit(0);
}
