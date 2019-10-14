/*
 * SW compiler program implemented in C
 *
 * The program has been tested on CodeBlocks  16.01 rev 10702
 *
 * Student: Tao Wei ����
 * Student Number: 10142130225
 *
 * ʹ�÷�����
 * ���к�����SWԴ�����ļ���
 * �ش��Ƿ�������������
 * �ش��Ƿ�������ű�
 * fcode.txt������������
 * foutput.txt���Դ�ļ�������ʾ�⣨���д������ж�Ӧ�����ɴ����׵�ַ�����޴�
 * fresult.txt������н��
 * ftable.txt������ű�
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define bool int            /* ��ΪC����û��bool���� */
#define true 1              /* ��������3�д��� */
#define false 0

#define norw 29             /* �����ָ��� 10+19 */
#define txmax 100           /* ���ű����� */
#define nmax 14             /* ���ֵ����λ�� */
#define al 10               /* ��ʶ������󳤶� 10?*/

#define maxerr 30           /* ������������� */
#define amax 2048           /* ��ַ�Ͻ�*/
#define cxmax 200           /* ��������������� */
#define stacksize 500       /* ����ʱ����ջԪ�����Ϊ500�� */
#define levmax 1            /* ����������Ƕ����������*/


/* ���� */
enum symbol {
    nul,                    /* δ���� */
    ident,                  /* ��ʶ�� */
    number,                 /*  ����  */
    plus,   minus,          /*  +  -  */
    times,  slash,          /*  *  /  */
    eql,                    /*   ==   */
    neq,                    /*   !=   */
    lss,    leq,            /*  <  <= */
    gtr,    geq,            /*  >  >= */
    lparen, rparen,         /*  (  )  */
    lbrace, rbrace,         /*  {  }  */
    semicolon,              /*   ;    */
    threepoints,            /*  ...   */
    becomes,                /*   =    */
    plusone,    minusone,   /* ++, -- */
    mod,                    /*   %    */
    colon,                  /*   :    */
    comma,                  /*   ,    */
    lbrack, rbrack,         /*  [  ]  */
    ifsym,     elsesym,   whilesym,   forsym,    insym,
    readsym,   printsym,  callsym,    varsym,    funcsym,
    boolsym,   constsym,  floatsym,   oddsym,    switchsym,
    casesym,   breaksym,  repeatsym,  notsym,    defaultsym,
    andsym,    orsym,     exitsym,    tointsym,  tofloatsym,
    truesym,   falsesym,  returnsym,  eofsym,    continuesym,
};
#define symnum 57           // 27+29+1


/* ���ű��е����� */
enum object {
    variable,               /*  ����  */
    function,               /*  ����  */
    boolean,                /*  ����  */
    constant,               /*  ����  */
    floatnum,               /*  ʵ��  */
};


/* ���������ָ�� */
enum fct {
    lit,                    /* lit 0, a ����a����ջ�� */
    opr,                    /* opr 0, a һ��������ϵ����� ���ص��ó��� */
    lod,                    /* lod 1, a ��1,a�γɵ�ջ��ַ����ֵ����ջ�� */
    sto,                    /* sto 1, a ��ջ��ֵ�浽��1,a�γɵ�ջ��ַ���� */
    cal,                    /* cal 1, a �����ӳ��� */
    ini,                    /* int 0, a Ԥ��a���洢λ�� (intд��ini������C�����е�INT����) */
    jmp,                    /* jmp 0, a ������ת�� */
    jpc,                    /* jpc 0, a ����ת�� (��s(t)==0ʱ) */
};
#define fctnum 8


/* ���������ṹ */
struct instruction
{
    enum fct f;             /* ���������ָ�� */
    int l;                  /* ���ò���������Ĳ�β� */
    float a;                /* ����f�Ĳ�ͬ����ͬ */
};

struct parameter
{
    char name[al];
    enum object type;
};

struct parameter p[10];     /* �����ڿɴ������10������ */

/* ȫ�ֱ��� (�м����ʹ��) */
bool listswitch ;           /* ��ʾ������������ */
bool tableswitch ;          /* ��ʾ���ű���� */
struct instruction code[cxmax]; /* ����������������� */
char mnemonic[fctnum][5];   /* ���������ָ������ */
bool declbegsys[symnum];    /* ��ʾ������ʼ�ķ��ż��� */
bool statbegsys[symnum];    /* ��ʾ��俪ʼ�ķ��ż��� */
bool facbegsys[symnum];     /* ��ʾ���ӿ�ʼ�ķ��ż��� */

char ch;                    /* ��ŵ�ǰ��ȡ���ַ���getch ʹ�� */
enum symbol sym;            /* ��ǰ�ķ��� */
char id[al+1];              /* ��ǰident�������һ���ֽ����ڴ��0 */
float num;                  /* ��ǰnumber */
int cc, ll;                 /* getchʹ�õļ�������cc��ʾ��ǰ�ַ�(ch)��λ�� */
int cx;                     /* ���������ָ��, ȡֵ��Χ[0, cxmax-1]*/
char line[81];              /* ��ȡ�л����� */
char a[al+1];               /* ��ʱ�ַ����������һ���ֽ����ڴ��0 */
char word[norw][al];        /* ������ */
enum symbol wsym[norw];     /* �����ֶ�Ӧ�ķ���ֵ */
enum symbol ssym[256];      /* ���ַ��ķ���ֵ */

bool isend = false;         /* �ļ��Ƿ���� */
bool isjmp[10];             /* statement_list����пɷ���continue��break��� */
bool isfor = false;         /* ��ǰִ������Ƿ���for-in�����(ѭ�����ڳ���)�� */
bool isfloat = false;       /* ��ʶ���������������Ƿ���С�� */
int circlenum = 0;          /* ��ǰλ�ڼ���ѭ������ */
int continue_cx[10][100];   /* continue���λ�� */
int continue_n[10];         /* continue������ */
int break_cx[10][100];      /* break���λ�� */
int break_n[10];            /* break������ */
int exit_cx[100];           /* exit���λ��(���100��) */
int exit_n = 0;             /* exit������ */
int parameter_n = 0;        /* ����()�ڲ������� */
bool is_return = false;     /* �����Ƿ��з���ֵ */

char current[10];           /* ��ǰ���ں����� */
int varmax = 0;             /* ȫ�ֱ��������λ�� */

/* ���ű�ṹ */
struct tablestruct
{
    char name[al];          /* ���� */
    enum object kind;       /* ���� */
    int val;                /* ��ֵ����constʹ�� */
    int level;              /* ������, ��const��ʹ�� */
    int adr;                /* (��ʼ)��ַ, ��const��ʹ�� */
    int size;               /* ��Ҫ������������ռ�, ��function, arrayʹ�� */
    int para_n;             /* ������������, ��functionʹ�� */
    bool isreturn;          /* �Ƿ��з���ֵ, ��functionʹ�� */
};
struct tablestruct table[txmax]; /* ���ű� */


/* ȫ�ֱ��� (����������) */
FILE* fin;                  /* ����Դ�ļ� */
FILE* foutput;              /* ����ļ�������ʾ�⣨���д������ж�Ӧ�����ɴ����׵�ַ�����޴� */
FILE* ftable;               /* ������ű� */
FILE* fcode;                /* ������������ */
FILE* fresult;              /* ���ִ�н�� */
char fname[al];             /* �����ַ��� (�ļ���, �Ƿ�ѡ��) */
int err;                    /* ��������� */


/* �������� */
void error(int n);
void getsym();
void getch();
void init();

void program(int tx, bool* fsys, int lev);
void var_declaration_list(int* ptx, int* pdx, int lev);
void statement_list(int* ptx, bool* fsys, int lev);
void expression(int* ptx, bool* fsys, int lev);
void condition(int* ptx, bool* fsys, int lev);
void term(int* ptx, bool* fsys, int lev);
void factor(int* ptx, bool* fsys, int lev);

int position(char* idt, int tx);
void enter(enum object k, int* ptx, int* pdx, int lev);

void gen(enum fct x, int y, float z);
void test(bool* s1, bool* s2, int n);
int inset(int e, bool* s);
int addset(bool* sr, bool* s1, bool* s2, int n);
int subset(bool* sr, bool* s1, bool* s2, int n);
int mulset(bool* sr, bool* s1, bool* s2, int n);
void interpret();
void listcode(int cx0);
void listall();
int base(int l, float* s, int b);


/* ������ */
int main()
{
    bool nxtlev[symnum];                    /* ��̷��ż��� */

    printf("Input SW file?   ");
    scanf("%s", fname);                     /* �����ļ��� */

    if ((fin = fopen(fname, "r")) == NULL)  /* �ļ���ʧ�� */
    {
        printf("Can't open the input file:(\n");
        exit(1);
    }

    ch = fgetc(fin);
    if (ch == EOF)                          /*  �ļ�Ϊ��  */
    {
        printf("The input file is empty:(\n");
        fclose(fin);
        exit(1);
    }
    rewind(fin);                            /* ���ļ��ڲ���λ��ָ������ָ��һ������������/�ļ����Ŀ�ͷ */

    if ((foutput = fopen("foutput.txt", "w")) == NULL)
    {
        printf("Can't open the output file:(\n");
        exit(1);
    }

    if ((ftable = fopen("ftable.txt", "w")) == NULL)
    {
        printf("Can't open ftable.txt file!:(\n");
        exit(1);
    }

    printf("List object codes?(Y/N)");      /* �Ƿ������������� */
    scanf("%s", fname);
    listswitch = (fname[0]=='y' || fname[0]=='Y');

    printf("List symbol table?(Y/N)");      /* �Ƿ�������ű� */
    scanf("%s", fname);
    tableswitch = (fname[0]=='y' || fname[0]=='Y');

    init();                                 /* ��ʼ�� */

    addset(nxtlev, declbegsys, statbegsys, symnum);
    program(0, nxtlev, 0);                  /* ����ֳ��� */

    if (sym != eofsym)
    {
        printf("We cannot read the file over:(\n");
        exit(1);
    }

    if (err == 0)
    {
        printf("\n===Parsing success!===\n");
        fprintf(foutput,"\n===Parsing success!===\n");

        if ((fcode = fopen("fcode.txt", "w")) == NULL)
        {
            printf("Can't open fcode.txt file!\n");
            exit(1);
        }

        if ((fresult = fopen("fresult.txt", "w")) == NULL)
        {
            printf("Can't open fresult.txt file!\n");
            exit(1);
        }

        listall();                          /* ������д��� */
        fclose(fcode);

        interpret();                        /* ���ý���ִ�г��� */
        fclose(fresult);
      }
    else
    {
        printf("\n%d errors in SW program!\n",err);
        fprintf(foutput,"\n%d errors in SW program!\n",err);
    }

    fclose(ftable);
    fclose(foutput);
    fclose(fin);

    return 0;
}


/* ��ʼ�� */
void init()
{
    int i;

    /* ���õ��ַ����� */
    for (i = 0; i < 256; i++)
    {
        ssym[i] = nul;
    }
    ssym['+'] = plus;
    ssym['-'] = minus;
    ssym['*'] = times;
    ssym['/'] = slash;
    ssym['('] = lparen;
    ssym[')'] = rparen;
    ssym['{'] = lbrace;
    ssym['}'] = rbrace;
    ssym['='] = becomes;
    ssym[';'] = semicolon;
    ssym['%'] = mod;
    ssym[':'] = colon;
    ssym[','] = comma;
    ssym['['] = lbrack;
    ssym[']'] = rbrack;

    /* ���ñ���������,������ĸ˳�򣬱��ڶ��ֲ��� */
    strcpy(&(word[0][0]), "and");
    strcpy(&(word[1][0]), "bool");
    strcpy(&(word[2][0]), "break");
    strcpy(&(word[3][0]), "call");
    strcpy(&(word[4][0]), "case");
    strcpy(&(word[5][0]), "const");
    strcpy(&(word[6][0]), "continue");
    strcpy(&(word[7][0]), "default");
    strcpy(&(word[8][0]), "else");
    strcpy(&(word[9][0]), "exit");
    strcpy(&(word[10][0]), "false");
    strcpy(&(word[11][0]), "float");
    strcpy(&(word[12][0]), "for");
    strcpy(&(word[13][0]), "func");
    strcpy(&(word[14][0]), "if");
    strcpy(&(word[15][0]), "in");
    strcpy(&(word[16][0]), "not");
    strcpy(&(word[17][0]), "odd");
    strcpy(&(word[18][0]), "or");
    strcpy(&(word[19][0]), "print");
    strcpy(&(word[20][0]), "read");
    strcpy(&(word[21][0]), "repeat");
    strcpy(&(word[22][0]), "return");
    strcpy(&(word[23][0]), "switch");
    strcpy(&(word[24][0]), "tofloat");
    strcpy(&(word[25][0]), "toint");
    strcpy(&(word[26][0]), "true");
    strcpy(&(word[27][0]), "var");
    strcpy(&(word[28][0]), "while");

    /* ���ñ����ַ��� */
    wsym[0] = andsym;
    wsym[1] = boolsym;
    wsym[2] = breaksym;
    wsym[3] = callsym;
    wsym[4] = casesym;
    wsym[5] = constsym;
    wsym[6] = continuesym;
    wsym[7] = defaultsym;
    wsym[8] = elsesym;
    wsym[9] = exitsym;
    wsym[10] = falsesym;
    wsym[11] = floatsym;
    wsym[12] = forsym;
    wsym[13] = funcsym;
    wsym[14] = ifsym;
    wsym[15] = insym;
    wsym[16] = notsym;
    wsym[17] = oddsym;
    wsym[18] = orsym;
    wsym[19] = printsym;
    wsym[20] = readsym;
    wsym[21] = repeatsym;
    wsym[22] = returnsym;
    wsym[23] = switchsym;
    wsym[24] = tofloatsym;
    wsym[25] = tointsym;
    wsym[26] = truesym;
    wsym[27] = varsym;
    wsym[28] = whilesym;

    /* ����ָ������ */
    strcpy(&(mnemonic[lit][0]), "lit");
    strcpy(&(mnemonic[opr][0]), "opr");
    strcpy(&(mnemonic[lod][0]), "lod");
    strcpy(&(mnemonic[sto][0]), "sto");
    strcpy(&(mnemonic[cal][0]), "cal");
    strcpy(&(mnemonic[ini][0]), "int");
    strcpy(&(mnemonic[jmp][0]), "jmp");
    strcpy(&(mnemonic[jpc][0]), "jpc");

    /* ���÷��ż� */
    for (i = 0; i < symnum; i++)
    {
        declbegsys[i] = false;
        statbegsys[i] = false;
        facbegsys[i] = false;
    }

    /* ����������ʼ���ż� */
    declbegsys[varsym] = true;
    declbegsys[funcsym] = true;
    declbegsys[boolsym] = true;
    declbegsys[constsym] = true;
    declbegsys[floatsym] = true;

    /* ������俪ʼ���ż� */
    statbegsys[ifsym] = true;
    statbegsys[whilesym] = true;
    statbegsys[readsym] = true;
    statbegsys[printsym] = true;
    statbegsys[ident] = true;
    statbegsys[tointsym] = true;
    statbegsys[tofloatsym] = true;
    statbegsys[forsym] = true;
    statbegsys[callsym] = true;
    statbegsys[switchsym] = true;
    statbegsys[repeatsym] = true;
    statbegsys[exitsym] = true;

    /* �������ӿ�ʼ���ż� */
    facbegsys[ident] = true;
    facbegsys[number] = true;
    facbegsys[lparen] = true;
    facbegsys[tointsym] = true;
    facbegsys[tofloatsym] = true;
    facbegsys[truesym] = true;
    facbegsys[falsesym] = true;

    /* ��ʼ��break, continue��¼����Ϊ��,
     * ��ʼ�����Ϊ����ѭ������ */
    for(i = 0; i < 10; i++)
    {
        break_n[i] = 0;
        isjmp[i] = false;
        continue_n[i] = 0;
    }

    err = 0;
    cc = ll = cx = 0;
    ch = ' ';
}


/* ������ʵ�ּ��ϵļ������� */

/* �Ƿ��ڼ����е�������� */
int inset(int e, bool* s)
{
    return s[e];
}

/* ������ s1Us2 */
int addset(bool* sr, bool* s1, bool* s2, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        sr[i] = s1[i] || s2[i];
    }
    return 0;
}

/* ������ s1-s2 */
int subset(bool* sr, bool* s1, bool* s2, int n)
{
    int i;
    for (i=0; i<n; i++)
    {
        sr[i] = s1[i] && (!s2[i]);
    }
    return 0;
}

/* ������ s1ns2*/
int mulset(bool* sr, bool* s1, bool* s2, int n)
{
    int i;
    for (i=0; i<n; i++)
    {
        sr[i] = s1[i] && s2[i];
    }
    return 0;
}


/* ��������ӡ����λ�úʹ������ */
void error(int n)
{
    char space[81];
    memset(space,32,81);    /* 32Ϊasii���Ŀո� ��ʼ���ַ���Ϊȫ�ո�*/
    space[cc-1]=0;          /* ����ʱ��ǰ�����Ѿ����꣬����cc-1 */

    printf("**%s^%d\n", space, n);
    fprintf(foutput,"**%s^%d\n", space, n);

    err = err + 1;
    if (err > maxerr)
    {
        printf("There are too many errors to handle!:(\n");
        exit(1);
    }
}


/*
 * ���˿ո�����ֱ�ʾ������ע�ͣ���ȡһ���ַ�
 * ÿ�ζ�һ�У�����line��������line��getsymȡ�պ��ٶ�һ��
 * ������getsym����
 */
void getch()
{
    if (cc == ll)           /* �жϻ��������Ƿ����ַ��������ַ����������һ���ַ����������� */
    {                       /* �޻������ */
        if (feof(fin))      /* feof(fin)�������ļ����� */
        {
            isend = true;
        }

        printf("%d ", cx);  /* ������������ָ�� */
        fprintf(foutput,"%d ", cx);

        ll = 0;             /* �ļ�û�н��� */
        cc = 0;

        ch = ' ';
        while (ch != 10)   /* 10Ϊasii���Ļ��м� */
        {
            char pre = ch;
            if (EOF == fscanf(fin,"%c", &ch) || (pre == '/' && ch == '/' ))   /* �����ȡ���ļ�����/��ע�� */
            {
                if(ll != 0)                         /* ����ͷ�����ַ�����"//" */
                {
                    line[ll] = 0;                   /* �ַ�������\0, �����л������ */
                }
                if(pre == '/' && ch == '/')
                {
                    while(EOF != fscanf(fin, "%c", &ch) && ch != 10)          /* ���Ե�"//"���������ֱ������'\n' */
                    {
                        ;
                    }
                    if(ch !=10)
                        isend = true;

                    if(ll == 0)                     /* ����ͷ�����ַ�����"//" */
                    {
                        ch = ' ';
                        continue;
                    }

                    printf("\n");
                    fprintf(foutput,"\n");
                }
                else
                {
                    isend = true;
                }
                break;
            }
            if(pre == '/' && ch == '*')
            {
                line[ll] = 0;
                bool isenter = false;               /* ��ע�����Ƿ��л��з�, �������һ������ */
                fscanf(fin, "%c", &pre);
                fscanf(fin, "%c", &ch);
                if(pre == '\n')
                    isenter = true;
                while(pre != '*' || ch != '/')
                {
                    pre = ch;
                    if(pre == '\n')
                        isenter = true;
                    if (EOF == fscanf(fin,"%c", &ch))
                    {
                        error(29);                  // ȱ��"*/"
                    }
                }
                if(ll == 0)                         // ����ͷ�����ַ�����"/*"
                {
                    ch = ' ';
                    continue;
                }
                if(isenter == true)
                {
                    printf("\n");
                    fprintf(foutput,"\n");
                }
                break;
            }
            if(pre == '/')
            {
                printf("%c", pre);                   /* ͨ�������ж�δ����ѭ����'/'��ע�����ÿ������ */
                fprintf(foutput, "%c", pre);
                line[ll] = pre;
                ll++;
            }
            if(ch != '/')
            {
                printf("%c", ch);                   /* ��ǰ�ַ�Ϊ'/', �Ƿ�����ע���д���һ���ж� */
                fprintf(foutput, "%c", ch);
                line[ll] = ch;
                ll++;
            }
        }
    }

    if(isend == true && ll == 0)
        ch = 0;
    else
    {
        ch = line[cc];          /* �л������ */
        cc++;
    }
}


/* �ʷ���������ȡһ������ */
void getsym()
{
    int i, j, k;
    if(ch == 0)
        sym = eofsym;
    else
    {
        while (ch == ' ' || ch == 10 || ch == 9)    /* ���˿ո񡢻��к��Ʊ�� */
        {
            getch();
        }

        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) /* ��ǰ�����Ǳ�ʶ�����Ǳ����� */
        {
            /* ���ʱ�����ַ���a�� */
            k = 0;
            do {
                if(k < al)
                {
                    a[k] = ch;
                    k++;
                }
                getch();
            }while ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'));
            a[k] = 0;

            strcpy(id, a);

            /* ������ǰ�����Ƿ�Ϊ�����֣�ʹ�ö��ַ����� */
            i = 0;
            j = norw - 1;
            do {
                k = (i + j) / 2;
                if (strcmp(id, word[k]) <= 0)
                {
                    j = k - 1;
                }
                if (strcmp(id, word[k]) >= 0)
                {
                    i = k + 1;
                }
            } while (i <= j);
            if (i-1 > j)        /* ��ǰ����Ϊ������ */
            {
                sym = wsym[k];
                if(sym == wsym[10])
                {
                    sym = number;
                    num = 0; // false
                }
                else if(sym == wsym[26])
                {
                    sym = number;
                    num = 1; // true
                }
            }
            else                /* ��ǰ����Ϊ��ʶ�� */
            {
                sym = ident;
            }
        }
        else if (ch >= '0' && ch <= '9') /* ��ǰ�ĵ��������� */
        {
            sym = number;

            /* ��ȡ���ֵ�ֵ */
            num = 0;
            k = 0;
            do {
                num = 10 * num + ch - '0';
                k++;
                getch();
            } while (ch >= '0' && ch <= '9');

            float weight = 0.1;
            if(isfor == false && ch == '.')
            {
                getch();
                do{
                    num += weight * (ch - '0');
                    weight *= 0.1;
                    k++;
                    getch();
                } while(ch >= '0' && ch <= '9');
            }
            k--;
            if (k > nmax)       /* ����λ��̫�� */
            {
                error(30);
            }
        }
        else if(ch == '!')
        {
            getch();
            if(ch == '=')
            {
                sym = neq;
                getch();
            }
            else
            {
                sym = nul;      /* ����ʶ��ķ��� */
            }
        }
        else if(ch == '.')
        {
            getch();
            if(ch == '.')
            {
                getch();
                if(ch == '.')
                {
                    sym = threepoints;
                    getch();
                }
                else
                {
                    sym = nul;  /* ����ʶ��ķ��� */
                }
            }
            else
            {
                sym = nul;      /* ����ʶ��ķ��� */
            }
        }
        else if (ch == '<')     /* ���С�ڻ�С�ڵ��ڷ��� */
        {
            getch();
            if (ch == '=')
            {
               sym = leq;
               getch();
            }
            else
            {
               sym = lss;
            }
        }
        else if (ch == '>')     /* �����ڻ���ڵ��ڷ��� */
        {
            getch();
            if (ch == '=')
            {
                sym = geq;
                getch();
            }
            else
            {
                sym = gtr;
            }
        }
        else if (ch == '=')     /* �����ڷ��� */
        {
            getch();
            if (ch == '=')
            {
                sym = eql;
                getch();
            }
            else
            {
                sym = becomes;  /* ��ֵ���� */
            }
        }
        else if (ch == '+')
        {
            getch();
            if (ch == '+')
            {
                sym = plusone;
                getch();
            }
            else
            {
                sym = plus;
            }
        }
        else if (ch == '-')
        {
            getch();
            if (ch == '-')
            {
                sym = minusone;
                getch();
            }
            else
            {
                sym = minus;
            }
        }
        else if (ch == 0)
        {
            sym = eofsym;
        }
        else
        {
            sym = ssym[ch];     /* �����Ų�������������ʱ��ȫ�����յ��ַ����Ŵ��� */
            getch();
        }
    }
}


/*
 * �������������
 *
 * x: instruction.f;
 * y: instruction.l;
 * z: instruction.a;
 */
void gen(enum fct x, int y, float z)
{
    if (cx >= cxmax)
    {
        printf("Program is too long!X(\n");               /* ���ɵ���������������� */
        exit(1);
    }
    if ( z >= amax)
    {
        printf("Displacement address is too big!8(\n");   /* ��ַƫ��Խ�� */
        exit(1);
    }
    code[cx].f = x;
    code[cx].l = y;
    code[cx].a = z;
    cx++;
}


/*
 * ���Ե�ǰ�����Ƿ�Ϸ�
 *
 * ���﷨�����������ںͳ��ڴ����ò��Ժ���test��
 * ��鵱ǰ���ʽ�����˳����﷨��λ�ĺϷ���
 *
 * s1:  ��Ҫ�ĵ��ʼ���
 * s2:  ���������Ҫ�ĵ��ʣ���ĳһ����״̬ʱ��
 *      �ɻָ��﷨�����������������Ĳ��䵥�ʷ��ż���
 * n:   ������
 */
void test(bool* s1, bool* s2, int n)
{
    if (!inset(sym, s1))
    {
        error(n);
        /* ����ⲻͨ��ʱ����ͣ��ȡ���ţ�ֱ����������Ҫ�ļ��ϻ򲹾ȵļ��� */
        while ((!inset(sym,s1)) && (!inset(sym,s2)))
        {
            getsym();
        }
    }
}


/*
 * �����������
 *
 * tx:     ���ű�ǰβָ��
 * fsys:   ��ǰģ���̷��ż���
 * lev:    ��ǰ�ֳ������ڲ�
 */
void program(int tx, bool* fsys, int lev)
{
    int i;
    int dx;                 /* ��¼���ݷ������Ե�ַ */
    int tx0;                /* ������ʼtx */
    int cx0;                /* ������ʼcx */
    bool nxtlev[symnum];    /* ���¼������Ĳ����У����ż��Ͼ�Ϊֵ�Σ�������ʹ������ʵ�֣�
                               ���ݽ�������ָ�룬Ϊ��ֹ�¼������ı��ϼ������ļ��ϣ�����
                               �µĿռ䴫�ݸ��¼�����*/

    dx = 3;                 /* �����ռ����ڴ�ž�̬��SL����̬��DL�ͷ��ص�ַRA  */
    tx0 = tx;               /* ��¼�����ʶ���ĳ�ʼλ�� */
    table[tx0].adr = cx;    /* ��¼��ǰ�����Ŀ�ʼλ�� */
    gen(jmp, 0, 0);         /* ������תָ���תλ��δ֪��ʱ��0 */

    if (lev > levmax)       /* Ƕ�ײ������� */
    {
        error(32);
    }

    getsym();

    var_declaration_list(&tx, &dx, lev);
    varmax = tx;

    while (sym == funcsym)  /* ���������������ţ���ʼ����������� */
    {
        getsym();
        strcpy(current, id);
        printf("id:%s\n", id);
        if (sym == ident)
        {
            enter(function, &tx, &dx, lev);   /* ��д���ű� */
            getsym();
        }
        else
        {
            error(41);      /* function��ӦΪ��ʶ�� */
        }

        if (sym == lparen)
        {
            getsym();
        }
        else
        {
            error(17);     /* ������������"(" */
        }

        parameter_n = 0;
        if (sym == varsym || sym == boolsym || sym == floatsym)
        {
            enum object type;
            if(sym == varsym)
            {
                type = variable;
            }
            else if(sym == boolsym)
            {
                type = boolean;
            }
            else
            {
                type = floatnum;
            }
            getsym();
            if (sym == ident)
            {
                strcpy(p[parameter_n].name, id);
                p[parameter_n++].type = type;
                getsym();
            }
            else
            {
                error(43);
            }

            while(sym == comma)
            {
                getsym();
                if (sym == varsym || sym == boolsym || sym == floatsym)
                {
                    enum object type;
                    if(sym == varsym)
                    {
                        type = variable;
                    }
                    else if(sym == boolsym)
                    {
                        type = boolean;
                    }
                    else
                    {
                        type = floatnum;
                    }
                    getsym();
                    if (sym == ident)
                    {
                        strcpy(p[parameter_n].name, id);
                        p[parameter_n++].type = type;
                        getsym();
                    }
                    else
                    {
                        error(43);
                    }
                }
                else
                {
                    error(44);
                    break;
                }
            }
        }

        if (sym == rparen)
        {
            getsym();
        }
        else
        {
            error(18);      /* ������������")" */
        }
        if (sym == lbrace)
        {
            //getsym();     program�ڿ�ͷ��getsym()�ʲ���Ҫ�˴���getsym()
        }
        else
        {
            error(19);     /* ������������"{" */
        }

        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
        nxtlev[rbrace] = true;

        // start: program(tx, nxtlev, lev + 1);   /* Ƕ�׵���, ����Ϊlevmax=1, ������һ�㲻������func����*/
        int ii,idx,itx0,icx0;
        bool inxtlev[symnum];
        idx = 3;
        itx0 = tx;
        table[itx0].adr = cx;
        gen(jmp, 0, 0);
        if(lev+1 > levmax)
        {
            error(32);
        }
        for(ii = 0;ii< parameter_n;ii++)
        {
            strcpy(id, p[ii].name);
            enter(p[ii].type, &tx, &idx, lev+1);
        }
        getsym();
        var_declaration_list(&tx, &idx, lev+1);
        memcpy(inxtlev, statbegsys, sizeof(bool)*symnum);
        test(inxtlev, declbegsys, 7);
        code[table[itx0].adr].a = cx;
        table[itx0].adr = cx;
        table[itx0].size = idx;
        table[itx0].para_n = parameter_n;
        icx0 = cx;
        gen(ini, 0, idx);
        if(tableswitch)                     /* ������ű� */
        {
            for (ii = 1; ii <= tx; ii++)
            {
                switch(table[ii].kind)
                {
                    case variable:
                        printf("    %d var   %s ", ii, table[ii].name);
                        printf("lev=%d addr=%d\n", table[ii].level, table[ii].adr);
                        fprintf(ftable, "    %d var   %s ", ii, table[ii].name);
                        fprintf(ftable, "lev=%d addr=%d\n", table[ii].level, table[ii].adr);
                        break;
                    case boolean:
                        printf("    %d bool  %s ", ii, table[ii].name);
                        printf("lev=%d addr=%d\n", table[ii].level, table[ii].adr);
                        fprintf(ftable, "    %d bool  %s ", ii, table[ii].name);
                        fprintf(ftable, "lev=%d addr=%d\n", table[ii].level, table[ii].adr);
                        break;
                    case constant:
                        printf("    %d const %s ", ii, table[ii].name);
                        printf("val=%d\n", table[ii].val);
                        fprintf(ftable, "    %d const %s ", ii, table[ii].name);
                        fprintf(ftable, "val=%d\n", table[ii].val);
                        break;
                    case floatnum:
                        printf("    %d float %s ", ii, table[ii].name);
                        printf("lev=%d addr=%d\n", table[ii].level, table[ii].adr);
                        fprintf(ftable, "    %d float %s ", ii, table[ii].name);
                        fprintf(ftable, "lev=%d addr=%d\n", table[ii].level, table[ii].adr);
                        break;
                    case function:
                        printf("    %d func  %s ", ii, table[ii].name);
                        printf("lev=%d addr=%d size=%d para_n=%d isreturn=%d\n", table[ii].level, table[ii].adr, table[ii].size, table[ii].para_n, table[ii].isreturn);
                        fprintf(ftable, "    %d func  %s ", ii, table[ii].name);
                        fprintf(ftable, "lev=%d addr=%d size=%d para_n=%d isreturn=%d\n", table[ii].level, table[ii].adr, table[ii].size, table[ii].para_n, table[ii].isreturn);
                        break;
                }
            }
            printf("\n");
            fprintf(ftable, "\n");
        }

        memcpy(inxtlev, fsys, sizeof(bool) * symnum);  /* ÿ����̷��ż��϶������ϲ��̷��ż��ϣ��Ա㲹�� */
        inxtlev[rbrace] = true;
        statement_list(&tx, inxtlev, lev+1);

        is_return = false;
        if (sym == returnsym)
        {
            getsym();
            memcpy(inxtlev, fsys, sizeof(bool) * symnum);
            inxtlev[semicolon] = true;           /* ��̷���Ϊ';' */
            expression(&tx, inxtlev, lev + 1);
            is_return = true;

            if(sym == semicolon)
                getsym();
            else
            {
                error(5);
            }
        }
        table[itx0].isreturn = is_return;

        while (exit_n > 0)
        {
            code[exit_cx[--exit_n]].a = cx;
        }

        gen(opr, 0, 0);                               /* ÿ�����̳��ڶ�Ҫʹ�õ��ͷ����ݶ�ָ�� */

        for(ii = 0; ii < 10; ii++)
        {
            if(break_n[ii] != 0)
            {
                error(23);
            }
        }
        listcode(icx0);                                /* ������ֳ������ɵĴ��� */
        // end: program(tx, nxtlev, lev + 1);   /* Ƕ�׵���, ����Ϊlevmax=1, ������һ�㲻������func����*/

        if (sym == rbrace)
        {
            getsym();
            memcpy(nxtlev, statbegsys, sizeof(bool) * symnum);
            nxtlev[funcsym] = true;
            test(nxtlev, fsys, 6);
        }
        else
        {
            error(20);     /* ������������"}" */
        }
    }
    memcpy(nxtlev, statbegsys, sizeof(bool) * symnum);
    test(nxtlev, declbegsys, 7);

    code[table[tx0].adr].a = cx;        /* ��ǰ�����ɵ���ת������תλ�øĳɵ�ǰλ�� */
    table[tx0].adr = cx;                /* ��¼��ǰ���̴����ַ */
    table[tx0].size = dx;               /* ����������ÿ����һ�����������dx����1�����������Ѿ�������dx���ǵ�ǰ�������ݵ�size */

    cx0 = cx;
    gen(ini, 0, dx);                    /* ����ָ���ָ��ִ��ʱ������ջ��Ϊ�����õĹ��̿���dx����Ԫ�������� */

    if(tableswitch)                     /* ������ű� */
    {
        for (i = 1; i <= tx; i++)
        {
            switch(table[i].kind)
            {
                case variable:
                    printf("    %d var   %s ", i, table[i].name);
                    printf("lev=%d addr=%d\n", table[i].level, table[i].adr);
                    fprintf(ftable, "    %d var   %s ", i, table[i].name);
                    fprintf(ftable, "lev=%d addr=%d\n", table[i].level, table[i].adr);
                    break;
                case function:
                    printf("    %d func  %s ", i, table[i].name);
                    printf("lev=%d addr=%d size=%d para_n=%d isreturn=%d\n", table[i].level, table[i].adr, table[i].size, table[i].para_n, table[i].isreturn);
                    fprintf(ftable, "    %d func  %s ", i, table[i].name);
                    fprintf(ftable, "lev=%d addr=%d size=%d para_n=%d isreturn=%d\n", table[i].level, table[i].adr, table[i].size, table[i].para_n, table[i].isreturn);
                    break;
                case boolean:
                    printf("    %d bool  %s ", i, table[i].name);
                    printf("lev=%d addr=%d\n", table[i].level, table[i].adr);
                    fprintf(ftable, "    %d bool  %s ", i, table[i].name);
                    fprintf(ftable, "lev=%d addr=%d\n", table[i].level, table[i].adr);
                    break;
                case constant:
                    printf("    %d const %s ", i, table[i].name);
                    printf("val=%d\n", table[i].val);
                    fprintf(ftable, "    %d const %s ", i, table[i].name);
                    fprintf(ftable, "val=%d\n", table[i].val);
                    break;
                case floatnum:
                    printf("    %d float %s ", i, table[i].name);
                    printf("lev=%d addr=%d\n", table[i].level, table[i].adr);
                    fprintf(ftable, "    %d float %s ", i, table[i].name);
                    fprintf(ftable, "lev=%d addr=%d\n", table[i].level, table[i].adr);
                    break;
            }
        }
        printf("\n");
        fprintf(ftable, "\n");
    }

    memcpy(nxtlev, fsys, sizeof(bool) * symnum);  /* ÿ����̷��ż��϶������ϲ��̷��ż��ϣ��Ա㲹�� */

    current[0] = 0;
    statement_list(&tx, nxtlev, lev);

    while (exit_n > 0)
    {
        code[exit_cx[--exit_n]].a = cx;
    }

    gen(opr, 0, 0);                               /* ÿ�����̳��ڶ�Ҫʹ�õ��ͷ����ݶ�ָ�� */

    for(i = 0; i < 10; i++)
    {
        if(break_n[i] != 0)
        {
            error(23);
        }
    }


    if (!feof(fin))
    {
        memset(nxtlev, 0, sizeof(bool) * symnum); /* �ֳ���û�в��ȼ��� */
        test(fsys, nxtlev, 8);                    /* ����̷�����ȷ�� */
    }

    listcode(cx0);                                /* ������ֳ������ɵĴ��� */
}


/*
 * �ڷ��ű��м���һ��
 *
 * k:      ��ʶ��������
 * ptx:    ���ű�βָ���ָ�룬Ϊ�˿��Ըı���ű�βָ���ֵ
 * pdx:    dxΪ��ǰӦ����ı�������Ե�ַ�������Ҫ����1
 * lev:    ��ʶ�����ڵĲ��
 */
void enter(enum object k, int* ptx, int* pdx, int lev)
{
    (*ptx)++;
    strcpy(table[(*ptx)].name, id);     /* ���ű��name���¼��ʶ�������� */
    table[(*ptx)].kind = k;
    switch (k)
    {
        case variable:                  /* ���� */
            table[(*ptx)].level = lev;
            table[(*ptx)].adr = (*pdx);
            (*pdx)++;
            break;
        case function:                  /* ���� */
            table[(*ptx)].level = lev;
            break;
        case boolean:
            table[(*ptx)].level = lev;
            table[(*ptx)].adr = (*pdx);
            (*pdx)++;
            break;
        case constant:
            if (num > amax)
            {
                error(30);              /* ����Խ�� */
                num = 0;
            }
            table[(*ptx)].val = num;    /* �Ǽǳ�����ֵ */
            break;
        case floatnum:
            table[(*ptx)].level = lev;
            table[(*ptx)].adr = (*pdx);
            (*pdx)++;
            break;
    }
}


/*
 * ���ұ�ʶ���ڷ��ű��е�λ�ã���tx��ʼ������ұ�ʶ��
 * �ҵ��򷵻��ڷ��ű��е�λ�ã����򷵻�0
 *
 * id:    Ҫ���ҵ�����
 * tx:    ��ǰ���ű�βָ��
 */
int position(char* id, int tx)
{
    int i;
    strcpy(table[0].name, id);
    i = tx;
    while (strcmp(table[i].name, id) != 0 && strcmp(table[i].name, current)!= 0)
    {
        i--;
    }
    if(strcmp(table[i].name, id) != 0)
    {
        i = varmax;
        while (strcmp(table[i].name, id) != 0)
        {
            i--;
        }
    }
    return i;
}


/* ������������ */
void var_declaration_list(int* ptx, int* pdx, int lev)
{
    bool nxtlev[symnum];
    memcpy(nxtlev, declbegsys, sizeof(bool) * symnum);
    nxtlev[funcsym] = false;
    while (inset(sym, nxtlev))              /* ���������������ţ���ʼ����������� */
    {
        enum object type;
        if(sym == varsym)
        {
            type = variable;
        }
        else if(sym == boolsym)
        {
            type = boolean;
        }
        else if(sym == constsym)
        {
            type = constant;
        }
        else if(sym == floatsym)
        {
            type = floatnum;
        }
        getsym();
        if (sym == ident && type != constant)
        {
            getsym();
            if (sym != lbrack)
                enter(type, ptx, pdx, lev); /* ��д���ű� */
        }
        else if(sym == ident && type == constant)
        {
            getsym();
            if (sym == eql || sym == becomes)
            {
                if (sym == eql)
                {
                    error(1);           /* ��=д����== */
                }
                getsym();
                if (sym == number)
                {
                    enter(constant, ptx, pdx, lev);
                    getsym();
                }
                else
                {
                    error(2);           /* ���������е�=��Ӧ������ */
                }
            }
            else
            {
                error(3);               /* ���������еı�ʶ����Ӧ��= */
            }
        }
        else
        {
            error(42);                 /* var����Ӧ�Ǳ�ʶ�� */
        }

        /* �Ƿ������� */
        if (sym == lbrack)
        {
            getsym();
            if(sym == number)
            {
                int tmpi = 0;
                while(tmpi < num)
                {
                    enter(type, ptx, pdx, lev);
                    table[(*ptx)].size = tmpi;
                    tmpi++;
                }
                getsym();
            }
            else
                error(26);

            if (sym == rbrack)
            {
                getsym();
            }
            else
                error(27);
        }

        if (sym == semicolon)
        {
            getsym();
        }
        else
        {
            error(5);                 /* ©���˷ֺ� */
        }
    }
}


/* ���Ŀ������嵥 */
void listcode(int cx0)
{
    int i;
    if (listswitch)
    {
        printf("\n");
        for (i = cx0; i < cx; i++)
        {
            printf("%d %s %d %f\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
        }
    }
}

/* �������Ŀ����벢д��fcode�ļ� */
void listall()
{
    int i;
    if (listswitch)
    {
        for (i = 0; i < cx; i++)
        {
            printf("%d %s %d %f\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
            fprintf(fcode,"%d %s %d %f\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
        }
    }
}

/* ��䴦�� */
void statement_list(int* ptx, bool* fsys, int lev)
{
    int i, cx1, cx2;
    bool nxtlev[symnum];
    while (inset(sym, statbegsys)||( (sym == continuesym || sym == breaksym ) && isjmp[circlenum] == true ) )  /* ���������������ţ���ʼ����������� */
    {
        if (sym == ifsym)
        {
            getsym();
            memcpy(nxtlev, fsys, sizeof(bool) * symnum);
            nxtlev[lbrace] = true;  /* ��̷���Ϊ'{' */
            condition(ptx, nxtlev, lev); /* ������������ */

            if (sym == lbrace)
            {
                getsym();
            }
            else
            {
                error(52);         /* ��������"{" */
            }

            cx1 = cx;               /* ���浱ǰָ���ַ */
            gen(jpc, 0, 0);         /* ����������תָ��, ��ת��ַδ֪, ��д��0 */
            if(sym != rbrace)
                statement_list(ptx, nxtlev, lev);
            code[cx1].a = cx;       /* ��statement�����, cxΪthen�����ִ�����λ��, ������ǰ��δ������ת��ַ, ��ʱ���л��� */

            if (sym == rbrace)
            {
                getsym();
            }
            else
            {
                error(53);          /* ����ȱ��"}" */
            }

            if(sym == elsesym)
            {
                cx2 = cx;
                gen(jmp, 0, 0);

                getsym();
                if (sym == lbrace)
                {
                    getsym();
                }
                else
                {
                    error(54);      /* ����ȱ��"{" */
                }

                if(sym != rbrace)
                    statement_list(ptx, nxtlev, lev);

                code[cx2].a = cx;
                code[cx1].a = code[cx1].a + 1;

                if (sym == rbrace)
                {
                    getsym();
                }
                else
                {
                    error(55);      /* ����ȱ��"}" */
                }
            }
        }
        else if (sym == whilesym)
        {
            circlenum++;
            cx1 = cx;               /* �����ж�����������λ�� */
            getsym();
            memcpy(nxtlev, fsys, sizeof(bool) * symnum);
            nxtlev[lbrace] = true;  /* ��̷���Ϊ'{' */
            condition(ptx, nxtlev, lev); /* ������������ */
            cx2 = cx;               /* ����ѭ����Ľ�������һ��λ�� */
            gen(jpc, 0, 0);         /* ����������ת��������ѭ���ĵ�ַδ֪�����Ϊ0�ȴ����� */

            if (sym == lbrace)
            {
                getsym();
            }
            else
            {
                error(56);          /* ��������"{" */
            }

            fsys[continuesym] = true;
            fsys[breaksym] = true;
            isjmp[circlenum] = true;
            if(sym != rbrace)
                statement_list(ptx, fsys, lev);
            isjmp[circlenum] = false;

            if (sym == rbrace)
            {
                getsym();
            }
            else
            {
                error(57);         /* ����ȱ��"}" */
            }

            gen(jmp, 0, cx1);
            code[cx2].a = cx;

            while (break_n[circlenum] > 0)
            {
                code[break_cx[circlenum][--break_n[circlenum]]].a = cx;
            }
            while (continue_n[circlenum] > 0)
            {
                code[continue_cx[circlenum][--continue_n[circlenum]]].a = cx1;
            }
            circlenum--;
        }
        else if (sym == readsym)
        {
            getsym();

            if (sym != lparen)
            {
                error(34);          /* ��ʽ����Ӧ�������� */
            }
            else
            {
                getsym();
                if (sym == ident)
                {
                    i = position(id, *ptx); /* ����Ҫ���ı��� */
                }
                else
                {
                    i = 0;
                }

                if (i == 0)
                {
                    error(4);       /* read��������еı�ʶ��Ӧ�����������ı��� */
                }
                else
                {
                    if(table[i].kind == variable)
                    {
                        gen(opr, 0, 16);/* ��������ָ��, ��ȡֵ��ջ�� */
                    }
                    else if(table[i].kind == floatnum)
                    {
                        gen(opr, 0, 24);
                    }
                    else if(table[i].kind == boolean)
                    {
                        gen(opr, 0, 25);
                    }
                    gen(sto, lev-table[i].level, table[i].adr); /* ��ջ���������������Ԫ�� */
                }

                getsym();
            }
            if(sym != rparen)
            {
                error(33);          /* ��ʽ����Ӧ�������� */
                while (!inset(sym, fsys))   /* �����ȣ�ֱ�������ϲ㺯���ĺ�̷��� */
                {
                    getsym();
                }
            }
            else
            {
                getsym();
            }
        }
        else if (sym == printsym)
        {
            getsym();
            if (sym != lparen)
            {
                error(34);          /* ��ʽ����Ӧ�������� */
            }
            else
            {
                getsym();
                memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                nxtlev[rparen] = true;
                if (sym == ident)
                {
                    i = position(id, *ptx); /* ����Ҫ���ı��� */
                }
                else
                {
                    i = 0;
                }


                if (i == 0)
                {
                    error(4);      /* print��������еı�ʶ��Ӧ�����������ı��� */
                }
                else
                {
                    getsym();

                    /* �Ƿ������� */
                    if(sym == lbrack)
                    {
                        getsym();
                        if(sym == number)
                        {
                            num = table[i].size - num;
                            i -= num;
                            getsym();
                        }
                        else
                        {
                            error(26);
                        }

                        if(sym == rbrack)
                        {
                            getsym();
                        }
                        else
                            error(27);
                    }

                    if(table[i].kind == constant)
                    {
                        gen(lit, 0, table[i].val);
                    }
                    else
                        gen(lod, lev-table[i].level, table[i].adr);

                    if(table[i].kind == variable || table[i].kind == constant)
                    {
                        gen(opr, 0, 14);    /* �������ָ����ջ����ֵ */
                    }
                    else if(table[i].kind == floatnum)
                    {
                        gen(opr, 0, 22);
                    }
                    else if(table[i].kind == boolean)
                    {
                        gen(opr, 0, 23);
                    }
                    gen(opr, 0, 15);    /* ���ɻ���ָ�� */
                }

            }
            if(sym != rparen)
            {
                error(33);          /* ��ʽ����Ӧ�������� */
                while (!inset(sym, fsys))   /* �����ȣ�ֱ�������ϲ㺯���ĺ�̷��� */
                {
                    getsym();
                }
            }
            else
            {
                getsym();
            }
        }
        else if (sym == ident)
        {
            i = position(id, *ptx); /* ���ұ�ʶ���ڷ��ű��е�λ�� */

            if (i == 0)
            {
                printf("%d\n", sym);
                error(4);           /* ��ʶ��δ���� */
            }
            else
            {
                if( (table[i].kind != variable && table[i].kind != boolean && table[i].kind != floatnum )|| table[i].kind == constant)
                {
                    error(12);      /* ��ֵ����У���ֵ���󲿱�ʶ��Ӧ���Ǳ��� */
                    i = 0;
                }
                else
                {
                    getsym();

                    /* �Ƿ������� */
                    if(sym == lbrack)
                    {
                        getsym();
                        if(sym == number)
                        {
                            num = table[i].size - num;
                            i -= num;
                            getsym();
                        }
                        else
                        {
                            error(26);
                        }

                        if(sym == rbrack)
                        {
                            getsym();
                        }
                        else
                            error(27);
                    }

                    if(sym == eql || sym == becomes)
                    {
                        if (sym == eql)
                        {
                            error(1);           /* ��=д����== */
                        }
                        getsym();

                        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                        nxtlev[semicolon] = true;

                        if(table[i].kind == variable)
                        {
                            isfloat = false;
                        }
                        else if(table[i].kind == floatnum)
                        {
                            isfloat = true;
                        }

                        int ii;
                        if(sym == callsym)
                        {
                            getsym();
                            if (sym == ident && table[ii = position(id, *ptx)].kind == function)
                            {
                                getsym();
                                if (sym != lparen)
                                {
                                    error(17);           /* ��ʽ����Ӧ�������� */
                                }
                                else
                                {
                                    getsym();
                                }

                                if (sym == plus || sym == minus || inset(sym, facbegsys))
                                {
                                    int tn = 0;
                                    memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                                    nxtlev[comma] = true;
                                    nxtlev[rparen] = true;
                                    expression(ptx, nxtlev, lev);
                                    tn++;
                                    while(sym == comma)
                                    {
                                        getsym();
                                        expression(ptx, nxtlev, lev);
                                        tn++;
                                    }

                                    if(tn != table[ii].para_n)
                                    {
                                        error(45);     /* ���ú��������������� */
                                    }

                                    gen(opr, tn, 28);
                                    gen(opr, tn, 30);
                                    gen(opr, tn, 29);
                                }


                                if (table[ii].kind == function)
                                {
                                    gen(cal, lev-table[ii].level, table[ii].adr); /* ����callָ�� */
                                    if(table[ii].isreturn == true)
                                    {
                                        gen(opr, table[ii].size, 31);
                                    }
                                }
                                else
                                {
                                    error(15);          /* ��ʶ������ӦΪ���� */
                                }

                                if(sym != rparen)
                                {
                                    error(18);          /* ��ʽ����Ӧ�������� */
                                }
                                else
                                {
                                    getsym();
                                }
                            }
                        }
                        else
                            expression(ptx, nxtlev, lev);   /* ����ֵ�����Ҳ���ʽ */

                        if(i != 0)
                        {
                            /* expression��ִ��һϵ��ָ������ս�����ᱣ����ջ����ִ��sto������ɸ�ֵ */
                            gen(sto, lev-table[i].level, table[i].adr);
                        }
                    }
                    else if(sym == plusone)
                    {
                        if(i != 0)
                        {
                            gen(lod, lev-table[i].level, table[i].adr);
                        }
                        gen(lit, 0, 1);
                        gen(opr, 0, 2);
                        gen(sto, lev-table[i].level, table[i].adr);
                        getsym();
                    }
                    else if(sym == minusone)
                    {
                        if(i != 0)
                        {
                            gen(lod, lev-table[i].level, table[i].adr);
                        }
                        gen(lit, 0, 1);
                        gen(opr, 0, 3);
                        gen(sto, lev-table[i].level, table[i].adr);
                        getsym();
                    }
                    else
                    {
                        error(13);                  /* û�м�⵽��ֵ���� */
                    }
                }
            }
        }
        else if (sym == tointsym)
        {
            getsym();
            if(sym == lparen)
            {
                getsym();
            }
            else
            {
                error(10);
            }
            if(sym == ident)            /* ����Ϊ���� */
            {
                i = position(id, *ptx); /* ���ұ�ʶ���ڷ��ű��е�λ�� */
                if (i == 0)
                {
                    error(4);           /* ��ʶ��δ���� */
                }
                else
                {
                    switch (table[i].kind)
                    {
                        case constant:  /* ��ʶ��Ϊ���� */
                            error(21);
                            getsym();
                            break;
                        case boolean:
                            error(21);
                            getsym();
                            break;
                        case floatnum:
                            gen(lod, lev-table[i].level, table[i].adr); /* �ҵ�������ַ������ֵ��ջ */
                            gen(opr, 0, 20);//20 is to int
                            table[i].kind = variable;
                            getsym();
                            break;
                        case variable:  /* ��ʶ��Ϊ���� */
                            error(21);
                            getsym();
                            break;
                        case function:  /* ��ʶ��Ϊ���� */
                            error(21);
                            getsym();
                            break;
                    }
                }
            }
            if(sym == rparen)
            {
                getsym();
            }
            else
            {
                error(11);
            }
        }
        else if (sym == tofloatsym)
        {
            getsym();
            if(sym == lparen)
            {
                getsym();
            }
            else
            {
                error(10);
            }
            if(sym == ident)            /* ����Ϊ���� */
            {
                i = position(id, *ptx); /* ���ұ�ʶ���ڷ��ű��е�λ�� */
                if (i == 0)
                {
                    error(4);           /* ��ʶ��δ���� */
                }
                else
                {
                    switch (table[i].kind)
                    {
                        case constant:  /* ��ʶ��Ϊ���� */
                            error(22);
                            getsym();
                            break;
                        case boolean:
                            error(22);
                            getsym();
                            break;
                        case floatnum:
                            error(22);
                            getsym();
                            break;
                        case variable:  /* ��ʶ��Ϊ���� */
                            gen(lod, lev-table[i].level, table[i].adr); /* �ҵ�������ַ������ֵ��ջ */
                            gen(opr, 0, 21);//21 is to float
                            table[i].kind = floatnum;
                            getsym();
                            break;
                        case function:  /* ��ʶ��Ϊ���� */
                            error(22);  /* ����Ϊ���� */
                            getsym();
                            break;
                    }
                }
            }
            if(sym == rparen)
            {
                getsym();
            }
            else
            {
                error(11);
            }
        }
        else if (sym == forsym)
        {
            circlenum++;
            isfor = true;
            getsym();

            int center = 0;             /* for��in֮��ı�ʶ��λ�ñ�� */
            if (sym == ident)
            {
                i = position(id, *ptx); /* ����Ҫ���ı��� */
                center = i;
            }
            else
            {
                i = 0;
            }

            if (i == 0)
            {
                error(4);
            }

            getsym();

            if (sym == insym)
            {
                getsym();
            }
            else
            {
                error(49);             /* ȱ��"in" */
            }

            if (sym == ident)
            {
                i = 0;
                i = position(id, *ptx); /* ���ұ�ʶ���ڷ��ű��е�λ�� */
                if (i == 0)
                {
                    error(4);           /* ��ʶ��δ���� */
                }
                else
                {
                    if(table[i].kind != variable && table[i].kind != constant)
                    {
                        error(46);      /* for-in����У�ʡ�Ժ��󲿱�ʶ��Ӧ�������ͱ��� */
                        i = 0;
                    }
                    else
                    {
                        if (center != 0)
                        {
                            memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                            nxtlev[threepoints] = true;
                            expression(ptx, nxtlev, lev);
                            /* expression��ִ��һϵ��ָ������ս�����ᱣ����ջ����ִ��sto������ɸ�ֵ */
                            gen(sto, lev-table[center].level, table[center].adr);
                        }
                        // getsym(); ɾ����һ������Ϊexpression�����Ѿ���getsym()��:P
                    }
                }
            }
            else if (sym == number)
            {
                memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                nxtlev[threepoints] = true;
                expression(ptx, nxtlev, lev);   /* ����ֵ�����Ҳ���ʽ */
                if(center != 0)
                {
                    /* expression��ִ��һϵ��ָ������ս�����ᱣ����ջ����ִ��sto������ɸ�ֵ */
                    gen(sto, lev-table[center].level, table[center].adr);
                }
                // getsym(); //ɾ����һ������Ϊexpression�����Ѿ���getsym()��:P
            }
            else
            {
                error(46);         /* "in"����������/���������� */
            }

            if (sym == threepoints)
            {
                getsym();
            }
            else
            {
                error(48);         /* ȱ��"..." */
            }

            if (sym == ident)
            {
                cx1 = cx;

                i = 0;
                i = position(id, *ptx); /* ���ұ�ʶ���ڷ��ű��е�λ�� */
                if (i == 0)
                {
                    error(4);           /* ��ʶ��δ���� */
                }
                else
                {
                    if (table[i].kind != variable && table[i].kind != constant)
                    {
                        error(47);      /* for-in����У�ʡ�Ժ��Ҳ���ʶ��Ӧ���Ǳ��� */
                        i = 0;
                    }
                    else
                    {
                        if (center != 0)
                        {
                            gen(lod, lev-table[center].level, table[center].adr);

                            memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                            nxtlev[lbrace] = true;
                            expression(ptx, nxtlev, lev);

                            /* expression��ִ��һϵ��ָ������ս�����ᱣ����ջ����ִ��opr�������<=�ıȽ� */
                            gen(opr, 0, 13);
                        }
                        // getsym(); ɾ����һ������Ϊexpression�����Ѿ���getsym()��:P
                    }
                }
                cx2 = cx;
                gen(jpc, 0 ,0);
            }
            else if (sym == number)
            {
                cx1 = cx;
                getsym();
                memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                nxtlev[lbrace] = true;

                if(center != 0)
                {
                    gen(lod, lev-table[center].level, table[center].adr);
                    if (num > amax)
                    {
                        error(30); /* ��Խ�� */
                        num = 0;
                    }
                    gen(lit, 0, num);
                    gen(opr, 0, 13);
                }
                cx2 = cx;
                gen(jpc, 0 ,0);
            }
            else
            {
                error(47);         /* "..."����������/���������� */
            }

            if (sym == lbrace)
            {
                getsym();
            }
            else
            {
                error(50);         /* ��������"{" */
            }

            isfor = false;
            isjmp[circlenum] = true;
            if(sym != rbrace)
                statement_list(ptx, fsys, lev);
            isjmp[circlenum] = false;

            while (continue_n[circlenum] > 0)
            {
                code[continue_cx[circlenum][--continue_n[circlenum]]].a = cx;
            }

            /* centerָ��ı�ʶ������һ */
            gen(lit, 0, 1);
            gen(lod, lev-table[center].level, table[center].adr);
            gen(opr, 0, 2);
            gen(sto, lev-table[center].level, table[center].adr);

            gen(jmp, 0, cx1);       /* ����ѭ�������ж� */

            code[cx2].a = cx;

            if (sym == rbrace)
            {
                getsym();
            }
            else
            {
                error(51);         /* ����ȱ��"}" */
            }

            while (break_n[circlenum] > 0)
            {
                code[break_cx[circlenum][--break_n[circlenum]]].a = cx;
            }
            circlenum--;
        }
        else if (sym == callsym)
        {
            getsym();

            if (sym != ident)
            {
                error(14);          /* call��ӦΪ��ʶ�� */
            }
            else
            {
                i = position(id, *ptx);
                if (i == 0)
                {
                    error(4);       /* ������δ�ҵ� */
                }
                getsym();
            }
            if (sym != lparen)
            {
                error(34);          /* ��ʽ����Ӧ�������� */
            }
            else
            {
                getsym();
            }

            if (sym == plus || sym == minus || inset(sym, facbegsys))
            {
                int tn = 0;
                memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                nxtlev[comma] = true;
                nxtlev[rparen] = true;
                expression(ptx, nxtlev, lev);
                tn++;
                while(sym == comma)
                {
                    getsym();
                    expression(ptx, nxtlev, lev);
                    tn++;
                }

                if(tn != table[i].para_n)
                {
                    error(45);     /* ���ú��������������� */
                }

                gen(opr, tn, 28);
                gen(opr, tn, 30);
                gen(opr, tn, 29);
            }


            if (table[i].kind == function)
            {
                gen(cal, lev-table[i].level, table[i].adr); /* ����callָ�� */
            }
            else
            {
                error(15);          /* call���ʶ������ӦΪ���� */
            }

            if(sym != rparen)
            {
                error(33);          /* ��ʽ����Ӧ�������� */
            }
            else
            {
                getsym();
            }
        }
        else if (sym == switchsym)
        {
            int case_cx[10];
            int case_n = 0;
            getsym();
            memcpy(nxtlev, fsys, sizeof(bool) * symnum);
            nxtlev[lbrace] = true;           /* ��̷���Ϊ'{' */
            expression(ptx, nxtlev, lev);    /* ������������ */

            if(sym == lbrace)
            {
                getsym();
                while(sym == casesym)
                {
                    getsym();
                    if (sym == number)
                    {
                        if (num > amax)
                        {
                            error(30);      /* ��Խ�� */
                            num = 0;
                        }
                        gen(lit, 0, num);
                        gen(opr, 0, 18);
                        cx1 = cx;           /* ���浱ǰָ���ַ */
                        gen(jpc, 0, 0);
                        getsym();
                    }
                    else
                    {
                        error(36);
                    }

                    if (sym == colon)
                    {
                        getsym();
                        if(sym != breaksym)
                            statement_list(ptx, fsys, lev);
                    }
                    else
                    {
                        error(37);
                    }

                    if(sym == breaksym)
                    {
                        getsym();
                    }
                    else
                    {
                        error(38);
                    }

                    if (sym == semicolon)
                    {
                        getsym();
                    }
                    else
                    {
                        error(40);
                    }

                    case_cx[case_n++] = cx;
                    gen(jmp, 0, 0);

                    code[cx1].a = cx;
                }
                if(sym == defaultsym)
                {
                    getsym();
                    if(sym == colon)
                    {
                        gen(opr, 0, 17);
                        getsym();
                        if(sym != breaksym)
                            statement_list(ptx, fsys, lev);
                    }
                    else
                    {
                        error(37);
                    }
                    if(sym == breaksym)
                    {
                        getsym();
                    }
                    else
                    {
                        error(38);
                    }
                    if (sym == semicolon)
                    {
                        getsym();
                    }
                    else
                    {
                        error(40);
                    }
                    if(sym == rbrace)
                    {
                        getsym();
                    }
                    else
                    {
                        error(35);
                    }
                }
                else
                {
                    error(39);
                }
                while(case_n > 0)
                    code[case_cx[--case_n]].a = cx;
            }
        }
        else if (sym == repeatsym)
        {
            circlenum++;
            getsym();
            cx1 = cx;
            if (sym == lbrace)
            {
                getsym();
            }
            else
            {
                error(56);          /* ��������"{" */
            }

            fsys[rbrace] = true;
            isjmp[circlenum] = true;
            if(sym != rbrace)
                statement_list(ptx, fsys, lev);
            isjmp[circlenum] = false;

            if (sym == rbrace)
            {
                getsym();
            }
            else
            {
                error(57);         /* ����ȱ��"}" */
            }

            if(sym == whilesym)
            {
                getsym();
                memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                nxtlev[semicolon] = true;  /* ��̷���Ϊ'{' */
                condition(ptx, nxtlev, lev); /* ������������ */
                cx2 = cx;
                gen(jpc, 0, 0);
                gen(jmp, 0, cx1);
                code[cx2].a = cx;

                while (break_n[circlenum] > 0)
                {
                    code[break_cx[circlenum][--break_n[circlenum]]].a = cx;
                }

                while (continue_n[circlenum] > 0)
                {
                    code[continue_cx[circlenum][--continue_n[circlenum]]].a = cx1;
                }

                circlenum--;
            }
        }
        else if (sym == continuesym && isjmp[circlenum] == true)
        {
            continue_cx[circlenum][continue_n[circlenum]++] = cx;
            gen(jmp, 0, 0);
            getsym();
        }
        else if (sym == breaksym && isjmp[circlenum] == true)
        {
            break_cx[circlenum][break_n[circlenum]++] = cx;
            gen(jmp, 0, 0);
            getsym();
        }
        else if(sym == exitsym)
        {
            exit_cx[exit_n++] = cx;
            gen(jmp, 0, 0);
            getsym();
        }

        if (sym == semicolon)
        {
            getsym();
        }
        else
        {
            printf("%d\n", sym);
            error(5);               /* ©���˷ֺ� */
        }
    }
}


/* ���ʽ���� */
void expression(int* ptx, bool* fsys, int lev)
{
    enum symbol addop;              /* ���ڱ��������� */
    bool nxtlev[symnum];

    if(sym == plus || sym == minus || sym == notsym) /* ���ʽ��ͷ�������ţ���ʱ��ǰ���ʽ������һ�����Ļ򸺵��� */
    {
        addop = sym;                /* ���濪ͷ�������� */
        getsym();
        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
        nxtlev[plus] = true;
        nxtlev[minus] = true;
        term(ptx, nxtlev, lev);     /* ������ */
        if (addop == minus)
        {
            gen(opr, 0, 1);         /* �����ͷΪ��������ȡ��ָ�� */
        }
        if (addop == notsym)
        {
            gen(opr, 0, 26);
        }
    }
    else                            /* ��ʱ���ʽ��������ļӼ� */
    {
        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
        nxtlev[plus] = true;
        nxtlev[minus] = true;
        term(ptx, nxtlev, lev);     /* ������ */
    }
    while (sym == plus || sym == minus)
    {
        addop = sym;
        getsym();
        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
        nxtlev[plus] = true;
        nxtlev[minus] = true;
        term(ptx, nxtlev, lev);     /* ������ */
        if (addop == plus)
        {
            gen(opr, 0, 2);         /* ���ɼӷ�ָ�� */
        }
        else
        {
            gen(opr, 0, 3);         /* ���ɼ���ָ�� */
        }
    }
}


/* ��� */
void term(int* ptx, bool* fsys, int lev)
{
    enum symbol mulop;              /* ���ڱ���˳������� */
    bool nxtlev[symnum];

    memcpy(nxtlev, fsys, sizeof(bool) * symnum);
    nxtlev[times] = true;
    nxtlev[slash] = true;
    nxtlev[mod] = true;
    nxtlev[andsym] = true;
    nxtlev[orsym] = true;
    factor(ptx, nxtlev, lev);       /* �������� */
    while(sym == times || sym == slash || sym == mod)
    {
        mulop = sym;
        getsym();
        factor(ptx, nxtlev, lev);
        if(mulop == times)
        {
            gen(opr, 0, 4);         /* ���ɳ˷�ָ�� */
        }
        else if(mulop == slash)
        {
            if(isfloat == false)
            {
                gen(opr, 0, 5);     /* ���ɳ���ָ�� */
            }
            if(isfloat == true)
            {
                gen(opr, 0, 27);
            }
        }
        else
        {
            gen(opr, 0, 7);         /* ����ȡ��ָ�� */
        }
    }
    while(sym == andsym || sym == orsym )
    {
        mulop = sym;
        getsym();
        factor(ptx, nxtlev, lev);
        if(mulop == andsym)
        {
            gen(opr, 0, 4);         /* ���ɳ˷�ָ�� */
        }
        else if(mulop == orsym)
        {
            gen(opr, 0, 19);         /* ���ɻ�ָ�� */
        }
    }
}


/* ���Ӵ��� */
void factor(int* ptx, bool* fsys, int lev)
{
    int i;
    bool nxtlev[symnum];
    test(facbegsys, fsys, 24);      /* ������ӵĿ�ʼ���� */
    while(inset(sym, facbegsys))    /* ѭ���������� */
    {
        if(sym == ident)            /* ����Ϊ���� */
        {
            i = position(id, *ptx); /* ���ұ�ʶ���ڷ��ű��е�λ�� */
            if (i == 0)
            {
                error(4);           /* ��ʶ��δ���� */
            }
            else
            {
                switch (table[i].kind)
                {
                    case constant:  /* ��ʶ��Ϊ���� */
                        gen(lit, 0, table[i].val);  /* ֱ�Ӱѳ�����ֵ��ջ */
                        getsym();
                        break;
                    case boolean:
                        getsym();
                        /* �Ƿ������� */
                        if(sym == lbrack)
                        {
                            getsym();
                            if(sym == number)
                            {
                                num = table[i].size - num;
                                i -= num;
                                getsym();
                            }
                            else
                            {
                                error(26);
                            }

                            if(sym == rbrack)
                            {
                                getsym();
                            }
                            else
                                error(27);
                        }
                        gen(lod, lev-table[i].level, table[i].adr); /* �ҵ�������ַ������ֵ��ջ */
                        break;
                    case floatnum:
                        getsym();
                        /* �Ƿ������� */
                        if(sym == lbrack)
                        {
                            getsym();
                            if(sym == number)
                            {
                                num = table[i].size - num;
                                i -= num;
                                getsym();
                            }
                            else
                            {
                                error(26);
                            }

                            if(sym == rbrack)
                            {
                                getsym();
                            }
                            else
                                error(27);
                        }
                        gen(lod, lev-table[i].level, table[i].adr); /* �ҵ�������ַ������ֵ��ջ */
                        isfloat = true;
                        break;
                    case variable:  /* ��ʶ��Ϊ���� */
                        getsym();
                        /* �Ƿ������� */
                        if(sym == lbrack)
                        {
                            getsym();
                            if(sym == number)
                            {
                                num = table[i].size - num;
                                i -= num;
                                getsym();
                            }
                            else
                            {
                                error(26);
                            }

                            if(sym == rbrack)
                            {
                                getsym();
                            }
                            else
                                error(27);
                        }
                        gen(lod, lev-table[i].level, table[i].adr); /* �ҵ�������ַ������ֵ��ջ */
                        if(sym == plusone)
                        {
                            gen(lit, 0, 1);
                            gen(opr, 0, 2);
                            gen(sto, lev-table[i].level, table[i].adr);
                            gen(lod, lev-table[i].level, table[i].adr);
                            getsym();
                        }
                        else if(sym == minusone)
                        {
                            gen(lit, 0, 1);
                            gen(opr, 0, 3);
                            gen(sto, lev-table[i].level, table[i].adr);
                            gen(lod, lev-table[i].level, table[i].adr);
                            getsym();
                        }
                        break;
                    case function:  /* ��ʶ��Ϊ���� */
                        error(16);  /* ����Ϊ���� */
                        getsym();
                        break;
                }
            }
        }
        else
        {
            if(sym == number)       /* ����Ϊ�� */
            {
                if (num > amax)
                {
                    error(30);      /* ��Խ�� */
                    num = 0;
                }
                gen(lit, 0, num);
                getsym();
            }
            else
            {
                if (sym == lparen)  /* ����Ϊ���ʽ */
                {
                    getsym();
                    memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                    nxtlev[rparen] = true;
                    expression(ptx, nxtlev, lev);
                    if (sym == rparen)
                    {
                        getsym();
                    }
                    else
                    {
                        error(28);  /* ȱ�������� */
                    }
                }
                else if (sym == tointsym)
                {
                    getsym();
                    if(sym == lparen)
                    {
                        getsym();
                    }
                    else
                    {
                        error(10);
                    }
                    if(sym == ident)            /* ����Ϊ���� */
                    {
                        i = position(id, *ptx); /* ���ұ�ʶ���ڷ��ű��е�λ�� */
                        if (i == 0)
                        {
                            error(4);           /* ��ʶ��δ���� */
                        }
                        else
                        {
                            switch (table[i].kind)
                            {
                                case constant:  /* ��ʶ��Ϊ���� */
                                    error(21);
                                    getsym();
                                    break;
                                case boolean:
                                    error(21);
                                    getsym();
                                    break;
                                case floatnum:
                                    gen(lod, lev-table[i].level, table[i].adr); /* �ҵ�������ַ������ֵ��ջ */
                                    gen(opr, 0, 20);//20 is to int
                                    gen(lod, lev-table[i].level, table[i].adr);
                                    table[i].kind = variable;
                                    getsym();
                                    break;
                                case variable:  /* ��ʶ��Ϊ���� */
                                    error(21);
                                    getsym();
                                    break;
                                case function:  /* ��ʶ��Ϊ���� */
                                    error(21);
                                    getsym();
                                    break;
                            }
                        }
                    }
                    if(sym == rparen)
                    {
                        getsym();
                    }
                    else
                    {
                        error(11);
                    }
                }
                else if (sym == tofloatsym)
                {
                    getsym();
                    if(sym == lparen)
                    {
                        getsym();
                    }
                    else
                    {
                        error(10);
                    }
                    if(sym == ident)            /* ����Ϊ���� */
                    {
                        i = position(id, *ptx); /* ���ұ�ʶ���ڷ��ű��е�λ�� */
                        if (i == 0)
                        {
                            error(4);           /* ��ʶ��δ���� */
                        }
                        else
                        {
                            switch (table[i].kind)
                            {
                                case constant:  /* ��ʶ��Ϊ���� */
                                    error(22);
                                    getsym();
                                    break;
                                case boolean:
                                    error(22);  /* ����Ϊ���� */
                                    getsym();
                                    break;
                                case floatnum:
                                    error(22);  /* ����Ϊ���� */
                                    getsym();
                                    break;
                                case variable:  /* ��ʶ��Ϊ���� */
                                    gen(lod, lev-table[i].level, table[i].adr); /* �ҵ�������ַ������ֵ��ջ */
                                    gen(opr, 0, 21);//21 is to float
                                    gen(lod, lev-table[i].level, table[i].adr);
                                    table[i].kind = floatnum;
                                    getsym();
                                    break;
                                case function:  /* ��ʶ��Ϊ���� */
                                    error(22);  /* ����Ϊ���� */
                                    getsym();
                                    break;
                            }
                        }
                    }
                    if(sym == rparen)
                    {
                        getsym();
                    }
                    else
                    {
                        error(11);
                    }
                }
                else if (sym == truesym)
                {
                    gen(lit, 0, 1);
                    getsym();
                }
                else if (sym == falsesym)
                {
                    gen(lit, 0, 0);
                    getsym();
                }
            }
        }
        memset(nxtlev, 0, sizeof(bool) * symnum);
        nxtlev[lparen] = true;
        test(fsys, nxtlev, 25);     /* һ�����Ӵ�����ϣ������ĵ���Ӧ��fsys������ */
                                    /* ������ǣ������ҵ���һ�����ӵĿ�ʼ��ʹ�﷨�������Լ���������ȥ */
    }
}


/*  �������� */
void condition(int* ptx, bool* fsys, int lev)
{
    enum symbol relop;
    bool nxtlev[symnum];

    if(sym == oddsym)   /* ׼������odd���㴦�� */
    {
        getsym();
        expression(ptx, fsys, lev);
        gen(opr, 0, 6); /* ����oddָ�� */
    }
    else
    {
        /* �߼����ʽ���� */
        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
        nxtlev[eql] = true;
        nxtlev[neq] = true;
        nxtlev[lss] = true;
        nxtlev[leq] = true;
        nxtlev[gtr] = true;
        nxtlev[geq] = true;
        expression(ptx, nxtlev, lev);
        if (sym != eql && sym != neq && sym != lss && sym != leq && sym != gtr && sym != geq)
        {
            error(9);                  /* Ӧ��Ϊ��ϵ����� */
        }
        else
        {
            relop = sym;
            getsym();
            expression(ptx, fsys, lev);
            switch (relop)
            {
                case eql:
                    gen(opr, 0, 8);
                    break;
                case neq:
                    gen(opr, 0, 9);
                    break;
                case lss:
                    gen(opr, 0, 10);
                    break;
                case geq:
                    gen(opr, 0, 11);
                    break;
                case gtr:
                    gen(opr, 0, 12);
                    break;
                case leq:
                    gen(opr, 0, 13);
                    break;
                default:
                    break;
            }
        }
    }
}

/* ���ͳ��� */
void interpret()
{
    int p = 0;                      /* ָ��ָ�� */
    int b = 1;                      /* ָ���ַ */
    int t = 0;                      /* ջ��ָ�� */
    struct instruction i;           /* ��ŵ�ǰָ�� */
    float fs[stacksize];            /* С��ջ */

    printf("Start SW\n");
    fprintf(fresult,"Start SW\n");
    fs[0] = 0;                       /* s[0]���� */
    fs[1] = 0;                       /* �������������ϵ��Ԫ����Ϊ0 */
    fs[2] = 0;
    fs[3] = 0;
    do {
        i = code[p];                /* ����ǰָ�� */
        p = p + 1;
        switch (i.f)
        {
            case lit:               /* ������a��ֵȡ��ջ�� */
                t = t + 1;
                fs[t] = i.a;
                break;
            case opr:               /* ��ѧ���߼����� */
                switch ((int)i.a)
                {
                    case 0:         /* �������ý����󷵻� */
                        t = b - 1;
                        p = (int)fs[t + 3];
                        b = (int)fs[t + 2];
                        break;
                    case 1:         /* ջ��Ԫ��ȡ�� */
                        fs[t] = - fs[t];
                        break;
                    case 2:         /* ��ջ�������ջ���������ջԪ�أ����ֵ��ջ */
                        t = t - 1;
                        fs[t] = fs[t] + fs[t + 1];
                        break;
                    case 3:         /* ��ջ�����ȥջ���� */
                        t = t - 1;
                        fs[t] = fs[t] - fs[t + 1];
                        break;
                    case 4:         /* ��ջ�������ջ���� */
                        t = t - 1;
                        fs[t] = fs[t] * fs[t + 1];
                        break;
                    case 5:         /* ��ջ�������ջ���� */
                        t = t - 1;
                        fs[t] = (int)fs[t] / (int)fs[t + 1];
                        break;
                    case 6:         /* ջ��Ԫ�ص���ż�ж� */
                        fs[t] = (int)fs[t] % 2;
                        break;
                    case 7:         /* ��ջ����ȡ��ջ���� */
                        t = t - 1;
                        fs[t] = (int)fs[t] % (int)fs[t + 1];
                        break;
                    case 8:         /* ��ջ������ջ�����Ƿ���� */
                        t = t - 1;
                        fs[t] = (fs[t] == fs[t + 1]);
                        break;
                    case 9:         /* ��ջ������ջ�����Ƿ񲻵� */
                        t = t - 1;
                        fs[t] = (fs[t] != fs[t + 1]);
                        break;
                    case 10:        /* ��ջ�����Ƿ�С��ջ���� */
                        t = t - 1;
                        fs[t] = (fs[t] < fs[t + 1]);
                        break;
                    case 11:        /* ��ջ�����Ƿ���ڵ���ջ���� */
                        t = t - 1;
                        fs[t] = (fs[t] >= fs[t + 1]);
                        break;
                    case 12:        /* ��ջ�����Ƿ����ջ���� */
                        t = t - 1;
                        fs[t] = (fs[t] > fs[t + 1]);
                        break;
                    case 13:        /* ��ջ�����Ƿ�С�ڵ���ջ���� */
                        t = t - 1;
                        fs[t] = (fs[t] <= fs[t + 1]);
                        break;
                    case 14:        /* ջ��ֵ��� */
                        printf("%d", (int)fs[t]);
                        fprintf(fresult, "%d", (int)fs[t]);
                        t = t - 1;
                        break;
                    case 15:        /* ������з� */
                        printf("\n");
                        fprintf(fresult,"\n");
                        break;
                    case 16:        /* ����һ����������ջ�� */
                        t = t + 1;
                        printf("?");
                        fprintf(fresult, "?");
                        int tmp;
                        scanf("%d", &tmp);
                        fs[t] = tmp;
                        fprintf(fresult, "%d\n", tmp);
                        break;
                    case 17:
                        t = t - 1;
                        break;
                    case 18:        /* ��ջ������ջ�����Ƿ���� */
                        fs[t] = (fs[t] == fs[t - 1]);
                        break;
                    case 19:
                        fs[t] = fs[t] + fs[t - 1];
                        if(fs[t]>=1)
                            fs[t] = 1;
                        else
                            fs[t] = 0;
                        break;
                    case 20:
                        fs[t] = (int)fs[t];
                        break;
                    case 21:
                        fs[t] = (float)fs[t];
                        break;
                    case 22:        /* ջ��ֵ��� */
                        printf("%f", fs[t]);
                        fprintf(fresult, "%f", fs[t]);
                        t = t - 1;
                        break;
                    case 23:        /* ջ��ֵ��� */
                        if(fs[t] == 1)
                        {
                            printf("true");
                            fprintf(fresult, "true");
                        }
                        else if(fs[t] == 0)
                        {
                            printf("false");
                            fprintf(fresult, "false");
                        }
                        else
                        {
                            error(31);
                        }
                        t = t - 1;
                        break;
                    case 24:        /* ����һ����������ջ�� */
                        t = t + 1;
                        printf("?");
                        fprintf(fresult, "?");
                        scanf("%f", &fs[t]);
                        fprintf(fresult, "%f\n", fs[t]);
                        break;
                    case 25:        /* ����һ����������ջ�� */
                        t = t + 1;
                        printf("?");
                        fprintf(fresult, "?");
                        char inputtmp[6];
                        scanf("%s", inputtmp);
                        if (strcmp(inputtmp,"true") == 0)
                        {
                            fs[t] = 1;
                            fprintf(fresult, "%s\n", "true");
                        }
                        else if(strcmp(inputtmp, "false") == 0)
                        {
                            fs[t] = 0;
                            fprintf(fresult, "%s\n", "false");
                        }
                        break;
                    case 26:
                        if(fs[t] == 0)
                        {
                            fs[t] = 1;
                        }
                        else
                        {
                            fs[t] = 0;
                        }
                        break;
                    case 27:         /* ��ջ�������ջ���� */
                        t = t - 1;
                        fs[t] = fs[t] / fs[t + 1];
                        break;
                    case 28:
                        {
                            int ti;
                            for(ti = 0;ti<(int)i.l;ti++)
                            {
                                fs[t+3+(int)i.l] = fs[t];
                                t--;
                            }
                        }
                        break;
                    case 29:
                        t-=3+(int)i.l;
                        break;
                    case 30:
                        {
                            int ti;
                            for(ti = 0;ti<3+(int)i.l;ti++)
                            {
                                t++;
                                fs[t] = fs[t+(int)i.l];
                            }
                        }
                        break;
                    case 31:
                        t++;
                        fs[t] = fs[t+(int)i.l];
                        break;
                }
                break;
            case lod:               /* ȡ��Ե�ǰ���̵����ݻ���ַΪa���ڴ��ֵ��ջ�� */
                t = t + 1;
                fs[t] = fs[base(i.l, fs, b) + (int)i.a];
                break;
            case sto:               /* ջ����ֵ�浽��Ե�ǰ���̵����ݻ���ַΪa���ڴ� */
                fs[base(i.l, fs, b) + (int)i.a] = fs[t];
                t = t - 1;
                break;
            case cal:               /* �����ӹ��� */
                fs[t + 1] = base(i.l, fs, b); /* �������̻���ַ��ջ����������̬�� */
                fs[t + 2] = b;       /* �������̻���ַ��ջ����������̬�� */
                fs[t + 3] = p;       /* ����ǰָ��ָ����ջ�������淵�ص�ַ */
                b = t + 1;          /* �ı����ַָ��ֵΪ�¹��̵Ļ���ַ */
                p = (int)i.a;            /* ��ת */
                break;
            case ini:               /* ������ջ��Ϊ�����õĹ��̿���a����Ԫ�������� */
                t = t + (int)i.a;
                break;
            case jmp:               /* ֱ����ת */
                p = (int)i.a;
                break;
            case jpc:               /* ������ת */
                if (fs[t] == 0)
                    p = (int)i.a;
                t = t - 1;
                break;
        }
    } while (p != 0);
    printf("End sw\n");
    fprintf(fresult,"End sw\n");
}

/* ͨ�����̻�ַ����l����̵Ļ�ַ */
int base(int l, float* s, int b)
{
    int b1;
    b1 = b;
    while (l > 0)
    {
        b1 = (int)s[b1];
        l--;
    }
    return b1;
}
