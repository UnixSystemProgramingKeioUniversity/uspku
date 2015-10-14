#include <stdio.h>
#include <stdib.h>
#include "buf.h"
#include "dlist.h"
#include "state.h"
#include "getblk.h"

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

int main(){


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

void init_proc(int, char *[]){
  
}
void buf_proc(int, char *[]);
void hash_proc(int, char *[]);
void free_proc(int, char *[]);
void getblk_proc(int, char *[]);
void brelse_proc(int, char *[]);
void set_proc(int, char *[]);
void reset_proc(int, char *[]);
void quit_proc(int, char *[]);

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
