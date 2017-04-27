#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char* string;

typedef struct _Pair {
  string key;
  void *value;
} Pair;

string h2b[] = {
  "0000", "0001", "0010", "0011",
  "0100", "0101", "0110", "0111", 
  "1000", "1001", "1010", "1011",
  "1100", "1101", "1110", "1111"};

void hex2binary(string hex, string binary) {
  for (int i=0; hex[i] != '\0'; i++) {
    char h = hex[i] - '0';
    if (h > 10) h = hex[i] - 'A' + 10;
    sprintf(&binary[4*i], "%s", h2b[h]);
  }
}

void decimal2binary(int d, string binary) {
  char hex[100];
  sprintf(hex, "%04x", d);
  hex2binary(hex, binary);  
}

string codes[]={
"@2",
"D=A",
"@3",
"D=D+A",
"@0",
"M=D"
};

int codesLen = 6, dTableSize = 8, dSize = 8;

Pair dMap[] = {
  {"", "000"}, {"M", "001"}, {"D", "010"}, {"MD", "011"},
  {"A","100"}, {"AM","101"}, {"AD","110"}, {"AMD","111"}
};

Pair cMap[] = {
  {"0",   "0101010"}, {"1",   "0111111"}, {"-1",  "0111010"},
  {"D",   "0001100"}, {"A",   "0110000"}, {"!D",  "0001101"},
  {"!A",  "0110001"}, {"-D",  "0001111"}, {"-A",  "0110011"},
  {"D+1", "0011111"}, {"A+1", "0110111"}, {"D-1", "0001110"},
  {"A-1", "0110010"}, {"D+A", "0000010"}, {"D-A", "0010011"},
  {"A-D", "0000111"}, {"D&A", "0000000"}, {"D|A", "0010101"},
  {"M",   "1110000"}, {"!M",  "1110001"}, {"-M",  "1110011"},
  {"M+1", "1110111"}, {"M-1", "1110010"}, {"D+M", "1000010"},
  {"D-M", "1010011"}, {"M-D", "1000111"}, {"D&M", "1000000"},
  {"D|M", "1010101"}
};

int find(string key, Pair map[], int len) {
    for (int i=0; i<len; i++) {
        if (strcmp(map[i].key, key)==0)
            return i;
    }
    return -1;
}

string lookup(string key, Pair map[], int len) {
   int i = find(key, map, len);
   if (i==-1) return NULL;
   return map[i].value;
}

void code2binary(string code, string binary) {
    if (code[0]=='@') {
      // A 指令： ＠number
      int number;
      sscanf(code, "@%d", &number);
//      printf("d=%d\n", d);
      decimal2binary(number, binary);  
    } else {
      // C 指令： d = exp
      // sprintf(binary, "error");
      char d[10], comp[100];
      sscanf(code, "%[^=]=%s", d, comp);
//      printf("  d=%s comp=%s\n", d, comp);
      string dcode = lookup(d, dMap, dSize);
//      printf("  dcode=%s\n", dcode);
      string ccode = lookup(comp, cMap, sizeof(cMap)/sizeof(cMap[0]));
//      printf("  ccode=%s\n", ccode);
      string jcode = "000";
      sprintf(binary, "111%s%s%s", ccode, dcode, jcode);
    }
}

void assemble(string fname) {
  char infile[100], outfile[100], line[100], binary[17];
  sprintf(infile, "%s.asm", fname);
//  sprintf(outfile, "%s.hack", fname);
  FILE *fp = fopen(infile, "r");
  while (fgets(line, sizeof(line), fp)) {
    char code[100];
    sscanf(line, "%[^/\n\r]", code);
    if (strlen(code)==0) continue;
//    printf("code=%s len=%lu\n", code, strlen(code));
    code2binary(code, binary);
    printf("%s\n", binary);
  }
  fclose(fp);
}

int main(int argc, char *argv[]) {
  assemble(argv[1]);
}

/*  
  char hex[5];
  char binary[17];
  for (int i=0;i<codesLen;i++) {
//    printf("%s\n", codes[i]);
    code2binary(codes[i], binary);
    printf("%s\n", binary);
  }
*/  
/*
@2
D=A
@3
D=D+A
@0
M=D
*/


/*
char *dTable[]  = { "",    "M",   "D",   "MD",  "A",   "AM",  "AD",  "AMD"};
char *dBinary[] = { "000", "001", "010", "011", "100", "101", "110", "111"};

int find(char *item, char *array[], int len) {
    for (int i=0; i<len; i++) {
        if (strcmp(array[i], item)==0)
            return i;
    }
    return -1;
}

char *lookup(char *item, char *name[], char *value[], int len) {
   int i = find(item, name, len);
   if (i==-1) return NULL;
   return value[i];
}
*/
/*  
  printf("%x\n", 10);
  printf("%x\n", 100);
  sprintf(hex, "%04x", 33);
  printf("%s\n", hex);
  hex2binary(hex, binary);
  printf("%s\n", binary);
  decimal2binary(33, binary);
  printf("binary(33)=%s\n", binary);
*/  
