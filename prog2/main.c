#include <stdio.h>
#include <stdib.h>
#include "buf.h"
#include "dlist.h"
#include "state.h"
#include "getblk.h"
#include <assert.h>

void help_proc(int, char *[]);
void init_proc(int, char *[]);
void buf_proc(int, char *[]);
void hash_proc(int, char *[]);
void free_proc(int, char *[]);
void getblk_proc(int, char *[]);
void brelse_proc(int, char *[]);
void set_proc(int, char *[]);
void reset_proc(int, char *[]);
void quit_proc(int, char *[]);

struct command_table{
  char *cmd;
  void (*func)(int, char *[]);
};

struct command_table cmd_tbl[]{
  {"help", help_proc},
  {"init", init_proc},
  {"buf", buf_proc},
  {"hash", hash_proc},
  {"free", free_proc},
  {"getblk", getblk_proc},
  {"brelse", brelse_proc},
  {"set", set_proc},
  {"reset", reset_proc},
  {"quit", quit_proc},
  {NULL, NULL}
};

int main(int argc, char *argv[]){
  struct command_table *p;
  int ac;
  char *av[16];


  for(p = cmd_tbl; p -> cmd; p++){
    if(strcmp(av[0], p -> cmd) == 0){
      (*p -> func)(ac, av);
      break;
    }
  }
  if(p -> cmd == NULL){
    fprintf(stderr, "unknown cmmand: %s\n", av[0]);
  }

}


void help_proc(int num, char *name[]){
  //init
  printf("init : \n");
  printf("\tinitialize hash list and free list and make the initial ");
  printf("state shown in the figure 2.15. \n ");
  //buf
  printf("buf[n ...] \n");
  printf("\tIf there is no input value, ");
  printf("display all the status of the buffers.\n");
  printf("\tIf there is an input block number, ");
  printf("display the status of the buffer at the block number.\n");
  //hash
  printf("hash[n ...] \n");
  printf("\tIf there is no input value, ");
  printf("display all the hash list.\n");
  printf("\tIf there is an input hash key, ");
  printf("display the hash list at the hash key given from user.\n");
  //free
  printf("free\n");
  printf("\tDisplay free list\n");
  //getblk 
  printf("getblk n\n");
  printf("\ttake the blkno from the user, execute getblk(n)\n");
  //brelse  
  printf("brelse n\n");
  printf("\ttake the blkno from the user, execute brelse(bp), \n");
  printf("where bp is the pointer to buffer header with blkno = n\n");  
  //set
  printf("set n stat [stat]\n");
  printf("\tset the status of the buffer of blkno n to stat\n");
  //reset
  printf("reset n stat [stat]\n");
  printf("\treset the status of the buffer of blkno n to stat\n");
  //quit
  printf("exit the software\n");
}

void init_proc(int , char *[]){
  //init process
  for(int i = 0; i < NHASH; i++){
    h_head[i].hash_fp = NULL;
    h_head[i].hash_bp = NULL;
    h_head[i].stat = 0;
    h_head[i].free_fp = NULL;
    h_head[i].free_bp = NULL;
    h_head[i].cache_data = NULL;
    buf *p = malloc(sizeof(buf));
    buf *q = malloc(sizeof(buf));
    buf *r = malloc(sizeof(buf));
    if(p == NULL | q == NULL | r == NULL){
      printf("unable to assign memory\n");
      abort();
    }
    ASSERT(p != NULL && q != NULL && r != NULL);
    insert_list(h_head[i], p, HASHHEAD);
    insert_list(h_head[i].hash_fp, q, HASHHEAD);
    insert_list(((h_head[i].hash_fp) -> hash_fp), r, HASHHEAD);
  }
  //adding buffers
  //mod = 0
  buf *trac = h_head[0].hash_fp;
  trac -> blkno = 28;
  AddStatus(trac, STAT_VALID);
  trac = trac -> hash_fp;
  trac -> blkno = 4;
  AddStatus(trac, STAT_VALID);
  trac = trac -> hash_fp;
  trac -> blkno = 64;
  AddStatus(trac, STAT_VALID | STAT_LOCKED);
  //mod = 1
  trac = h_head[1].hash_fp;
  trac -> blkno = 17;
  AddStatus(trac, STAT_VALID | STAT_LOCKED);
  trac = trac -> hash_fp;
  trac -> blkno = 5;
  AddStatus(trac, STAT_VALID);
  trac = trac -> hash_fp;
  trac -> blkno = 97;
  AddStatus(trac, STAT_VALID);
  //mod = 2
  trac = h_head[2].hash_fp;
  trac -> blkno = 98;
  AddStatus(trac, STAT_VALID | STAT_LOCKED);
  trac = trac -> hash_fp;
  trac -> blkno = 50;
  AddStatus(trac, STAT_VALID | STAT_LOCKED);
  trac = trac -> hash_fp;
  trac -> blkno = 10;
  AddStatus(trac, STAT_VALID);
  //mod = 3
  trac = h_head[3].hash_fp;
  trac -> blkno = 3;
  AddStatus(trac, STAT_VALID);
  trac = trac -> hash_fp;
  trac -> blkno = 35;
  AddStatus(trac, STAT_VALID | STAT_LOCKED);
  trac = trac -> hash_fp;
  trac -> blkno = 99;
  AddStatus(trac, STAT_VALID | STAT_LOCKED);

  //adding freelist
  trac = f_flist;
  trac -> free_fp = NULL;
  insert_list(trac, h_head[3].hash_fp, FREEHEAD);
  trac = trac -> free_fp;
  insert_list(trac, (h_head[1].hash_fp) -> hash_fp, FREEHEAD);
  trac = trac -> free_fp;
  insert_list(trac, (h_head[0].hash_fp) -> hash_fp, FREEHEAD);
  trac = trac -> free_fp;
  insert_list(trac, h_head[0].hash_fp, FREEHEAD);
  trac = trac -> free_fp;
  insert_list(trac, (h_head[1].hash_fp) -> hash_fp -> hash_fp, FREEHEAD);
  trac = trac -> free_fp;
  insert_list(trac, (h_head[2].hash_fp) -> hash_fp -> hash_fp, FREEHEAD);

}
void buf_proc(int argc, char *argv[]){
  if(!argc){
    PrintBufferAll();
  }
  else{
    for(int i = 0; i < argc; i++){
      if(isalnum((int)argv[i])){
	PrintBufferOne((int)argv[i]);
      }
      else{
	printf("INPUT VALUE MUST BE NUMBER, ASS WHOLE\n");
	printf("NOTHING PRINTED FOR THIS SHITTY REQUEST\n");
      }
    }
  }
}
void hash_proc(int argc, char *argv[]){
  if(!argc){
    PrintHashALL();
  }
  else{
    for(int i = 0; i < argc; i++){
      if(0 <= (int)argv[i] && (int)argv[i] < 4){
	PrintHashLine((int)argv[i]);
      }
      else{
	printf("INPUT VALUE MUST BE WITHIN 0~3, DUSHBAG\n");
	printf("NOTHING PRINTED FOR THIS SHITTY REQUEST\n");
      }
    }
  }
}
void free_proc(int, char *[]){
  PrintFree();
}
void getblk_proc(int argc, char *argv[]){
  if(!argc){
    printf("COMMON MAN, WHICH BLOCK ARE YOU TALKING ABOUT\n");
  }
  else{
    if(isalnum((int)argv[0])){
      if((int)argv[0] <= 0 && (int)argv[0] < 12){
	getblk((int)argv[0]);
      }
      else{
	printf("INPUT VALUE MUST BE WITHING 0~11\n");
      }
    }
    else{
      printf("INPUT VALUE MUST BE NUMBER, ASS WHOLE\n");
    }
  }
}
void brelse_proc(int, char *[]){
  if(!argc){
    printf("COMMON MAN, WHICH BLOCK ARE YOU TALKING ABOUT\n");
  }
  else{
    if(isalnum((int)argv[0])){
      if((int)argv[0] <= 0 && (int)argv[0] < 12){
	brelse((int)argv[0]);
      }
      else{
	printf("INPUT VALUE MUST BE WITHING 0~11\n");
      }
    }
    else{
      printf("INPUT VALUE MUST BE NUMBER, ASS WHOLE\n");
    }
  }
}
void set_proc(int argc, char *argv[]){
  if(!argc){
    printf("JUST DON'T DO THE SHITS\n");
  }
  else{
    if(isalnum((int)argv[0])){
      if((int)argv[0] <= 0 && (int)argv[0] < 12){
	int stat = 0;
	for(int i = 1; i < argc; i++){
	  stat += (int)argv[i];
	}
	buf *p = GetBuf(h_head,(int)argv[0]); 
	SetStatus(p, stat);
      }
      else{
	printf("INPUT VALUE MUST BE WITHING 0~11\n");
      }
    }
  }
}

void reset_proc(int, char *[]){
  if(!argc){
    printf("JUST DON'T DO THE SHITS\n");
  }
  else{
    if(isalnum((int)argv[0])){
      if((int)argv[0] <= 0 && (int)argv[0] < 12){
	int stat = 0;
	for(int i = 1; i < argc; i++){
	  stat += (int)argv[i];
	}
	buf *p = GetBuf(h_head,(int)argv[0]); 
	ResetStatus(p, stat);
      }
      else{
	printf("INPUT VALUE MUST BE WITHING 0~11\n");
      }
    }
  }
}
void quit_proc(int, char *[]){
  return;  
}

void PrintState(buf *p){
  int state = p -> stat;
  if(state & 0x1)  printf("L");
  else printf("-");
  state >> 1;

  if(state & 0x1)  printf("V");
  else printf("-");
  state >> 1;

  if(state & 0x1)  printf("D");
  else printf("-");
  state >> 1;

  if(state & 0x1)  printf("K");
  else printf("-");
  state >> 1;

  if(state & 0x1)  printf("W");
  else printf("-");
  state >> 1;

  if(state & 0x1)  printf("O");
  else printf("-");

}
void PrintBufferOne(int index){
  int hkey = index % 4;
  buf *p = hash_head[hkey].hash_fp;
  for(int i = index / 4; i > 0; i++){
    p = p -> hash_fp;
  }
  printf("\t[ %d : %d ", index, p -> blkno);
  PrintState(p);
  printf("]");
}

void PrintBufferAll(){
  //printf("buf[n ...]\n");
  int index = 0;
  for(int i = 0; i < NHASH; i++){
    for(buf *p = hash_head[i].hash_fp; p != &hash_head[i]; p = p -> hash_fp){
      printf("\t[ %d : %d ", index, p -> blkno);
      PrintState(p);
      printf("]");
      index++; 
    }
  }
}

void PrintHashLine(int hkey){
  int index = hkey * 4;
  for(buf *p = hash_head[hkey].hash_fp; p != &hash_head[hkey]; p = p -> hash_fp){
    printf("\t[ %d : %d ", index, p -> blkno);
    PrintState(p);
    printf("]");
    index++;
  }
}

void PrintHashAll(){
  printf("Hash[n ...]\n");
  int index = 0;
  for(int i = 0; i < NHASH; i++){
    printf("%d :", i);
    for(buf *p = hash_head[i].hash_fp; p != &hash_head[i]; p = p -> hash_fp){
      printf("\t[ %d : %d ", index, p -> blkno);
      PrintState(p);
      printf("]");
      index++; 
    }
    printf("\n");
  }
}

void PrintFree(){
  int index = 0; 
  for(buf *p = f_head.free_fp; p != &f_head; p = p -> free_fp){
    index = SearchNum(p -> blkno);
    printf("\t[ %d : %d ", index, p -> blkno);
    PrintState(p);
    printf("]");
  }
}

int SearchNum(int blkno){
  buf *buffer = Search(blkno);
  int index = 0; 
  for(int i = 0; i < NHASH; i++){
    for(buf *p = hash_head[i].hash_fp; p != &hash_head[i]; p = p -> hash_fp){
      if(p == buffer) return index;
      index++; 
    }    
  }
}

buf *init(){
  buf *p = malloc(sizeof(buf));
  p -> hash_fp = NULL;
  p -> hash_bp = NULL;
  p -> stat = 0;
  p -> free_fp = NULL;
  p -> free_bp = NULL;
  p -> cache_data = NULL;

  return p;
}

void SetStatus(buf *h, int stat){
  h -> stat = stat;
}

void ResetStatus(buf *h, int stat){
  h -> stat ^ stat;
}

buf *GetBuf(int index){
  int quotient = index / 4;
  int remainder = index % 4;
  buf *p = h_head[quotient].hash_fp;
  for(int i = 0; i < remainder; i++){
    p -> hash_fp;
  }
  return p;
}
