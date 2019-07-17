#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

const int MAX_HASH = 30000000;
const int MAX_VOCAB = 1000000;
const int MAX_WORD_SIZE = 20;
const char text_file[100];
const char emb_file[100];
const int EMB_DIM = 100;

struct VocabT{
	char *word;
	int cnt;
	float *emb;
};

int *hash_table;
struct VocabT *vocab_table;
int vocab_ctr = 0;

int getHash(char *word){
	unsigned long long int hash = 0;
	for(int i=0;i<strlen(word);i++){
		hash = hash * 257 + word[i];
	}
	hash = hash%MAX_HASH;
	return hash;
}

int searchVocab(char *word){
	int hash = getHash(word);
	while(1){
		if(hash_table[hash]==-1)return -1;
		if(strcmp(vocab_table[hash_table[hash]].word,word)==0){
			return hash_table[hash];
		}
		hash = (hash + 1)%MAX_HASH;
	}
}

void addToVocab(char *word){
	int pos = searchVocab(word);
	if(pos==-1){
		strcpy(vocab_table[vocab_ctr].word,word);
		vocab_table[vocab_ctr].cnt = 1;
		vocab_ctr++;
		int hash = getHash(word);
		while(hash_table[hash]!=-1){
			hash = (hash + 1)%MAX_HASH;
		}
		hash_table[hash] = vocab_ctr-1;
	}else{
		vocab_table[pos].cnt++;
	}
}

void readWord(char *word, FILE *fpT){
	int a = 0;
	char c;
	while(!feof(fpT)){
		c = fgetc(fpT);
		if(c==' ' || c=='\n' || c=='\t'){
			if(a>0)break;
			else continue;
		}
		word[a] = c;
		a++;
		if(a >= MAX_WORD_SIZE - 1)a--;
	}
	word[a] = 0;
}

void addToTable(char *word, float *emb){
	int pos = searchVocab(word);
	if(pos!=-1){
		for(int i=0;i<EMB_DIM;i++)vocab_table[pos].emb[i] = emb[i];
	}
}

void readEmb(char *word, float *emb, FILE *fpE){
	int a = 0;
	int flag = 0;
	char c;
	char e[EMB_DIM][10];
	int b=0,ct=0;	
	while(!feof(fpE)){
		c=fgetc(fpE);
		if (flag==0){
			if(c==' '){
				if(a>0){
					word[a]=0;
					flag=1;
					continue;
				}
				else continue;
			}
			word[a] = c;
			a++;
			if(a >= MAX_WORD_SIZE - 1)a--;
		}else{
			if(c=='\n'){
				e[b][ct]=0;
				break;
			}
			if(c==' '){ 
				e[b][ct]=0;
				b++;
				ct=0;
			}
			e[b][ct] = c;
			ct++;			
		}
	}
	for(int i=0;i<EMB_DIM;i++){
		char *pend;
		emb[i] = strtof(e[i],&pend);
	}
}

void main(int *argc, char **argv){

	if(argv[1]!=NULL)strcpy((char *)text_file,argv[1]);
	else {
		printf("Text corpus not specified");
		exit(0);
	}
	if(argv[2]!=NULL)strcpy((char *)emb_file,argv[2]);
	else {
		printf("Embeddings file not specified");
		exit(0);
	}

	hash_table = (int *)malloc(MAX_HASH*sizeof(int));
	for(int i=0;i<MAX_HASH;i++){
		hash_table[i] = -1;
	}
	vocab_table = (struct VocabT *)malloc(MAX_VOCAB*sizeof(struct VocabT));
	for(int i=0;i<MAX_VOCAB;i++){
		vocab_table[i].word = (char *)malloc(MAX_WORD_SIZE*sizeof(char));
		vocab_table[i].emb = (float *)malloc(EMB_DIM*sizeof(float));
		for(int j=0;j<EMB_DIM;j++)vocab_table[i].emb[j] = 0.0;
	}

	FILE *fpT;
	fpT = fopen(text_file,"r");
	char word[MAX_WORD_SIZE];
	while(!feof(fpT)){
		if(vocab_ctr >= MAX_VOCAB){
			vocab_ctr = MAX_VOCAB - 1;
			break;
		}
		readWord(word,fpT);
		addToVocab(word);
	}

	FILE *fpE;
	fpE = fopen(emb_file,"r");
	float emb[EMB_DIM];
	int h=0;
	while(!feof(fpE)){
		readEmb(word,emb,fpE);
		addToTable(word,emb);
		h++;
		if(h>10000)break;
	}

	for(int j=0;j<10;j++){
		printf("\n%s,%d,",vocab_table[j].word,vocab_table[j].cnt);
		for(int i=0;i<EMB_DIM;i++){
			printf("%f,",vocab_table[j].emb[i]);
		}
	}

	fclose(fpT);
	fclose(fpE);
	free(hash_table);
	free(vocab_table);
}
