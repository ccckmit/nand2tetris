#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

char hexDigits[] = "0123456789ABCDEF";

void hex2binary(string hex, string binary) {
  for (int i=0; hex[i] != '\0'; i++) {
    char *ptr = strchr(hexDigits, hex[i]);
    assert(ptr != NULL);
    char h = ptr - hexDigits;
    sprintf(&binary[4*i], "%s", h2b[h]);
  }
}

void decimal2binary(int d, string binary) {
  char hex[100];
  sprintf(hex, "%04X", d);
  hex2binary(hex, binary);
}

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

Pair jMap[] = {
  {"",   "000"}, {"JGT","001"}, {"JEQ","010"}, {"JGE","011"},
  {"JLT","100"}, {"JNE","101"}, {"JLE","110"}, {"JMP","111"}
};

#define SYM_SIZE 1000
Pair symTable[SYM_SIZE] = {
  {"R0", "0"}, {"R1", "1"}, {"R2", "2"}, {"R3", "3"}, 
  {"R4", "4"}, {"R5", "5"}, {"R6", "6"}, {"R7", "7"},
  {"R8", "4"}, {"R9", "9"}, {"R10", "10"}, {"R11", "11"},
  {"R12", "12"}, {"R13", "13"}, {"R14", "14"}, {"R15", "15"},
  {"SCREEN","16384"}, {"KBD","24576"}, 
  {"SP","0"}, {"LCL","1"}, {"ARG","2"}, {"THIS","3"}, {"THAT","4"}
};

int symTop = 22;

#define arraySize(array) (sizeof(array)/sizeof(array[0]))

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

void add(Pair pair, Pair map[], int top) {
  map[top++] = pair;
}

void code2binary(string code, string binary) {
  char d[10], comp[100], j[10];
  string dcode, ccode, jcode;
  if (code[0]=='@') { // A 指令： ＠number
    int number;
    sscanf(code, "@%d", &number);
    decimal2binary(number, binary);  
  } else { // C 指令： d = comp;j
    if (strchr(code, '=') != NULL) {
      sscanf(code, "%[^=]=%s", d, comp);
      dcode = lookup(d, dMap, arraySize(dMap));
      ccode = lookup(comp, cMap, arraySize(cMap));
      sprintf(binary, "111%s%s000", ccode, dcode);
    } else {
      sscanf(code, "%[^;];%s", comp, j);
      ccode = lookup(comp, cMap, arraySize(cMap));
      jcode = lookup(j, jMap, arraySize(jMap));
      sprintf(binary, "111%s000%s", ccode, jcode);      
    }
  }
}

void pass1(string infile) {
  char line[100], binary[17];
  FILE *fp = fopen(infile, "r");
  int address = 0;
  while (fgets(line, sizeof(line), fp)) {
    char code[100]="\0", *codePtr = line, *codeEnd;
    while (strchr("\t ", *codePtr) != NULL) codePtr++;
    for (codeEnd = codePtr; *codeEnd != '\0' && strchr("/\n\r", *codeEnd) == NULL; codeEnd++);
    *codeEnd = '\0';
    if (strlen(codePtr)==0) continue;
    printf("code=%s len=%lu\n", codePtr, strlen(codePtr));
    if (codePtr[0] == '(') {
      char label[100], addressStr[100];
      sscanf(codePtr, "(%[^)])", label);
      printf("  1. sym:%s address:%d\n", label, address);
      decimal2binary(address, addressStr);
      Pair p;
      p.key = strdup(label);
      p.value = strdup(addressStr); 
      add(p, symTable, symTop);
      printf("  2. sym:%s address:%d\n", label, address);
    } else {
      address ++;
    }
  }
  fclose(fp);
}

void pass2(string infile) {
  char line[100], binary[17];
  FILE *fp = fopen(infile, "r");
  while (fgets(line, sizeof(line), fp)) {
    char code[100];
    sscanf(line, "%[^/\n\r]", code);
    if (strlen(code)==0) continue;
    code2binary(code, binary);
    printf("%s\n", binary);
  }
  fclose(fp);
}

void assemble(string fname) {
  char infile[100];
  sprintf(infile, "%s.asm", fname);
  pass1(infile);
//  pass2(infile);
}

// run: ./asm Add > Add.hack
int main(int argc, char *argv[]) {
  assemble(argv[1]);
}
