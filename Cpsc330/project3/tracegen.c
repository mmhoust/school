/* tracegen.c - version of mmgen.c version 1.1 with memory refs traced
 *
 * generate a mm loop nest, also do tiling and also do unrolling
 *
 * current use:
 *  % cc -O tracegen.c -o tracegen
 *  % tracegen ijk$ > mm.c
 *  % cc -O mm.c
 *  % a.out
 *  <access type (r/w) and address printed to stdout>
 *
 *
 * mark smotherman
 * mark@cs.clemson.edu
 * clemson university
 *
 * (command line input and input error checking added in april 2003)
 *
 * a mm loop nest can be described by the order of the loop indices, so
 * this program takes as input a simple language describing these indices:
 *  <number>d  ==> generate tiling loop for index i with step size of <number>
 *  <number>e  ==> generate tiling loop for index j with step size of <number>
 *  <number>f  ==> generate tiling loop for index k with step size of <number>
 *  <number>i  ==> generate loop for index i with unrolling factor of <number>
 *  <number>j  ==> generate loop for index j with unrolling factor of <number>
 *  <number>k  ==> generate loop for index k with unrolling factor of <number>
 *  $          ==> input terminator
 * rules are:
 *  i,j,k tokens must appear
 *  if d appears, it must appear before i
 *  if e appears, it must appear before j
 *  if f appears, it must appear before k
 *  unrolling factors must evenly divide the matrix size
 *  $ must appear
 * currently <number> is only 2,4,8 but this can be changed if the program
 * proves interesting.  matrix size is first string on command line.
 *
 * (I am also reserving the tokens a,b,c to indicate use of a transpose
 * matrix.)
 *
 * (The coding is not pretty, but this is a first cut.)
 *
 * e.g., normal method is ijk$
 * e.g., a better method if ikj$
 * e.g., warner's method is 8d8e8f2i2jk$   (note the first three numbers
 *   are the tile size parameters in his original code, I use 8 for
 *   convenience)
 * e.g., maeno's method is 8d8f4j2ik$   (again note that the first two
 *   numbers are parameters in his original code, I use 8 for convenience)
 *
 * my interest in this program generator is to hook it to a sentence
 * generator and a minimum execution time finder, that is
 *   while((sentence=sgen())!=NULL){
 *     genprogram=mmgen(sentence);
 *     system("cc -O4 genprogram.c");
 *     system("a.out >> mmresults");
 *   }
 *   findmintime(mmresults);
 * this will find the optimum algorithm for the host system via an
 * exhaustive search.
 *
 * please report bugs and suggestions for enhancements to me.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* initially
 *
 *    head-->line-->line-->NULL
 *            ^      ^
 *            |      |
 *     insetafter  insertbefore
 */

#define ALLOC1 temp1=(struct line *)malloc(sizeof(struct line));\
temp1->indentcnt=indentcnt;

#define LINK1 temp1->next=insertbefore;\
insertafter->next=temp1;\
insertafter=temp1;

/* LINK1
 *
 *    head-->line-->line-->NULL
 *            ^      ^
 *            |      |
 *     insetafter  insertbefore
 *
 *    head-->line-->temp1-->line-->NULL
 *                   ^       ^
 *                   |       |
 *            insetafter   insertbefore
 */

#define INSERT1 temp1->next=start;\
start=temp1;

#define ALLOC2 temp1=(struct line *)malloc(sizeof(struct line));\
temp2=(struct line *)malloc(sizeof(struct line));\
temp1->indentcnt=indentcnt;\
temp2->indentcnt=indentcnt++;

#define LINK2 temp1->next=temp2;\
temp2->next=insertbefore;\
insertafter->next=temp1;\
insertafter=temp1;\
insertbefore=temp2;

/* LINK2
 *
 *    head-->line-->line-->NULL
 *            ^      ^
 *            |      |
 *     insetafter  insertbefore
 *
 *    head-->line-->temp1-->temp2-->line-->NULL
 *                   ^       ^
 *                   |       |
 *            insetafter   insertbefore
 */

#define ALLOCtr temp_trace=(struct line *)malloc(sizeof(struct line));\
temp_trace->indentcnt=indentcnt;

#define LINKtr temp_trace->next=insertbefore;\
insertafter->next=temp_trace;\
insertafter=temp_trace;

#define ALLOCtr2 temp_trace=(struct line *)malloc(sizeof(struct line));\
temp_trace2=(struct line *)malloc(sizeof(struct line));\
temp_trace->indentcnt=indentcnt;\
temp_trace2->indentcnt=indentcnt-1;

#define LINKtr2 temp_trace->next=insertbefore;\
temp_trace2->next=insertbefore->next;\
insertbefore->next=temp_trace2;\
insertafter->next=temp_trace;\
insertafter=temp_trace;

/* LINKtr2
 *
 *    head-->line-->line-->NULL
 *            ^      ^
 *            |      |
 *     insetafter  insertbefore
 *
 *    head-->line-->temp_trace-->line-->temp_trace2-->NULL
 *                   ^            ^
 *                   |            |
 *            insetafter        insertbefore
 */

struct line{ int indentcnt; char line[256]; struct line *next; };

int indentcnt;
int iflag,jflag,kflag;
int dflag,eflag,fflag;
int sflag,tflag,uflag;
int ikflag,kjflag,ijflag;
int dsize,esize,fsize;
int loopcnt;
int zeroflag;
int counter;
int iunroll,junroll,kunroll;

char c;
int n = 100;
int i,mt;
char *p0;
char mmtype[30];

struct line *start,*head,*insertafter,*insertbefore,*temp1,*temp2,
       *temp_trace,*temp_trace2;

void processloop();
void processdcltemp();
void processtemp();
void processtemp2();
void processtemp3();
void processtemp4();

int main( int argc, char * argv[] ){
  int c_index=0;

  n = atoi(argv[1]);

  indentcnt=0;
  iflag=jflag=kflag=0;
  dflag=eflag=fflag=0;
  sflag=tflag=uflag=0;
  ikflag=kjflag=ijflag=0;
  dsize=esize=fsize=0;
  loopcnt=0;
  zeroflag=0;
  counter=1;
  mt=0;

  if(argc!=3){
    fprintf(stderr,"matrix size and generating string should be given on the command line\n");
    fprintf(stderr,"e.g., ./tracegen 100 ijk$\n");
    exit(-1);
  }

  start=NULL;

  ALLOC2
  sprintf(temp1->line,"\n");
  sprintf(temp2->line,"\n");
  head=temp1; temp1->next=temp2; temp2->next=NULL;
  insertafter=temp1; insertbefore=temp2;

  while((c=argv[2][c_index++])!='$'){
    mmtype[mt++]=c;
    switch(c){
      case '2':
        counter=2;
        break;
      case '4':
        counter=4;
        break;
      case '8':
        counter=8;
        break;
      case 'd':
        if(counter==1){ fprintf(stderr,"d must have a valid tile size!\n"); exit(1); }
        if(iflag){ fprintf(stderr,"d cannot appear after i!\n"); exit(1); }
        dflag++;
        ALLOC1
        sprintf(temp1->line,"#define IISTEP %d\n",counter);
        INSERT1
        dsize=counter;
        counter=1;
        ALLOC2
        sprintf(temp1->line,"for(ii=0;ii<%d;ii+=IISTEP){\n",n);
        sprintf(temp2->line,"}\n",n);
        LINK2
        ALLOC1
        sprintf(temp1->line,"it=min(ii+IISTEP,%d);\n",n);
        LINK1
        break;
      case 'e':
        if(counter==1){ fprintf(stderr,"e must have a valid tile size!\n"); exit(1); }
        if(jflag){ fprintf(stderr,"e cannot appear after j!\n"); exit(1); }
        eflag++;
        ALLOC1
        sprintf(temp1->line,"#define JJSTEP %d\n",counter);
        INSERT1
        esize=counter;
        counter=1;
        ALLOC2
        sprintf(temp1->line,"for(jj=0;jj<%d;jj+=JJSTEP){\n",n);
        sprintf(temp2->line,"}\n",n);
        LINK2
        ALLOC1
        sprintf(temp1->line,"jt=min(jj+JJSTEP,%d);\n",n);
        LINK1
        break;
      case 'f':
        if(counter==1){ fprintf(stderr,"f must have a valid tile size!\n"); exit(1); }
        if(kflag){ fprintf(stderr,"f cannot appear after k!\n"); exit(1); }
        fflag++;
        ALLOC1
        sprintf(temp1->line,"#define KKSTEP %d\n",counter);
        INSERT1
        fsize=counter;
        counter=1;
        ALLOC2
        sprintf(temp1->line,"for(kk=0;kk<%d;kk+=KKSTEP){\n",n);
        sprintf(temp2->line,"}\n",n);
        LINK2
        ALLOC1
        sprintf(temp1->line,"kt=min(kk+KKSTEP,%d);\n",n);
        LINK1
        break;
      case 'i':
        loopcnt++;
        iunroll=counter;
        counter=1;
        if((dsize!=0)&&(dsize<iunroll)){
          fprintf(stderr,"unrolling factor for");
          fprintf(stderr," i should not be larger than d tile size!\n");
          exit(1);
        }
        if(n%iunroll){
          fprintf(stderr,"unrolling factor for");
          fprintf(stderr," i does not evenly divide matrix size!\n");
          exit(1);
        }
        iflag++;
        /* ? */ if(jflag) ijflag++; if(kflag) ikflag++;
        processloop('i',dflag,iunroll);
        break;
      case 'j':
        loopcnt++;
        junroll=counter;
        counter=1;
        if((esize!=0)&&(esize<junroll)){
          fprintf(stderr,"unrolling factor for");
          fprintf(stderr," j should not be larger than e tile size!\n");
          exit(1);
        }
        if(n%junroll){
          fprintf(stderr,"unrolling factor for");
          fprintf(stderr," j does not evenly divide matrix size!\n");
          exit(1);
        }
        jflag++; if(iflag) ijflag++; if(kflag) kjflag++;
        processloop('j',eflag,junroll);
        break;
      case 'k':
        loopcnt++;
        kunroll=counter;
        counter=1;
        if((fsize!=0)&&(fsize<kunroll)){
          fprintf(stderr,"unrolling factor for");
          fprintf(stderr," k should not be larger than f tile size!\n");
          exit(1);
        }
        if(n%kunroll){
          fprintf(stderr,"unrolling factor for");
          fprintf(stderr," k does not evenly divide matrix size!\n");
          exit(1);
        }
        kflag++; if(iflag) ikflag++;
        /* ? */ if(jflag) kjflag++;
        processloop('k',fflag,kunroll);
        break;
      default: 
        fprintf(stderr,"unrecognized character - %c - ignored\n",c);
        fprintf(stderr,"must be one of these characters: defijk248\n");
        exit(-1);
        break;
    }
    if((ikflag==1)||(junroll>1)){
      ikflag+=2;
      if((loopcnt!=3)||(junroll>1)){
        processtemp('s','a','i',iunroll,'k',kunroll);
        sflag++;
      }
    }
    if((kjflag==1)||(iunroll>1)){
      kjflag+=2;
      if((loopcnt!=3)||(iunroll>1)){
        processtemp('t','b','k',kunroll,'j',junroll);
        tflag++;
      }
    }
    if(ijflag==1){
      ijflag+=2;
      if(loopcnt!=3){
        if(kflag||fflag){
          processtemp2();
        }else{
          processtemp3();
          zeroflag++;
        }
        uflag++;
      }
    }
  }
  processtemp4();

  if((iflag==0)||(jflag==0)||(kflag==0)){
    fprintf(stderr,"at least one of the three loops (i,j,k) was not specified!\n");
    exit(1);
  }

  temp1=start;
  while(temp1!=NULL){
    printf("%s",temp1->line);
    temp1=temp1->next;
  }
  printf("#include <stdio.h>\n");
  printf("#include <stdlib.h>\n");
  if(dflag|eflag|fflag) printf("#define min(a,b) ((a)<=(b)?(a):(b))\n");
  printf("\ndouble a[%d][%d],b[%d][%d],c[%d][%d],d[%d][%d];\n\nmain(){\n"
    ,n,n,n,n,n,n,n,n);
  printf("  int i,j,k;\n");
  if(dflag) printf("  int ii,it;\n");
  if(eflag) printf("  int jj,jt;\n");
  if(fflag) printf("  int kk,kt;\n");
  if(sflag) processdcltemp('s',iunroll,kunroll);
  if(tflag) processdcltemp('t',kunroll,junroll);
  if(uflag) processdcltemp('u',iunroll,junroll);


  printf("%s",head->line);
  if(zeroflag==0){
    printf("  for(i=0;i<%d;i++){\n",n);
    printf("    for(j=0;j<%d;j++){\n",n);
    printf("      c[i][j]=0.0;\n");
    printf("        printf(\"w %%p\\n\",&(c[i][j]));    }\n  }\n");
  }
  temp1=head->next;
  while(temp1!=NULL){
    for(i=0;i<temp1->indentcnt;i++) printf("  ");
    while((p0=strstr(temp1->line,"+0"))!=NULL){
      *p0++=' '; *p0=' ';
    }
    printf("%s",temp1->line);
    temp1=temp1->next;
  }

  printf("}\n");

}

void processloop(index,flag,unroll)
char index;
int flag;
int unroll;
{
  ALLOC2
  if(unroll==1){
    if(flag){
      sprintf(temp1->line,"for(%c=%c%c;%c<%ct;%c++){\n",index,index,index,
        index,index,index);
    }else{
      sprintf(temp1->line,"for(%c=0;%c<%d;%c++){\n",index,index,n,index);
    }
  }else{
    if(flag){
      sprintf(temp1->line,"for(%c=%c%c;%c<%ct;%c+=%d){\n",index,index,index,
        index,index,index,unroll);
    }else{
      sprintf(temp1->line,"for(%c=0;%c<%d;%c+=%d){\n",index,index,n,index,
        unroll);
    }
  }
  sprintf(temp2->line,"}\n",n);
  LINK2
}

void processdcltemp(name,unroll1,unroll2)
char name;
int unroll1;
int unroll2;
{
  int i,j;
  for(i=0;i<unroll1;i++){
    for(j=0;j<unroll2;j++){
      printf("  double %c%d%d;\n",name,i,j);
    }
  }
}

void processtemp(name,array,index1,unroll1,index2,unroll2)
char name;
char array;
char index1;
int unroll1;
char index2;
int unroll2;
{
  int i,j;
  for(i=0;i<unroll1;i++){
    for(j=0;j<unroll2;j++){
      ALLOC1
      sprintf(temp1->line,"%c%d%d=%c[%c+%d][%c+%d];\n",
        name,i,j,array,index1,i,index2,j);
      LINK1
      ALLOCtr
      sprintf(temp_trace->line,"printf(\"r %%p\\n\",&(%c[%c+%d][%c+%d]));\n",
        array,index1,i,index2,j);
      LINKtr
    }
  }
}

void processtemp2()
{
  int i,j;
  for(i=0;i<iunroll;i++){
    for(j=0;j<junroll;j++){
          ALLOC2
          sprintf(temp1->line,"u%d%d=c[i+%d][j+%d];\n",i,j,i,j);
          sprintf(temp2->line,"c[i+%d][j+%d]=u%d%d;\n",i,j,i,j);
          LINK2
          ALLOCtr2
          sprintf(temp_trace->line,"printf(\"r %%p\\n\",&(c[i+%d][j+%d]));\n",i,j);
          sprintf(temp_trace2->line,"printf(\"w %%p\\n\",&(c[i+%d][j+%d]));\n",i,j);
          LINKtr2
          indentcnt--;
    }
  }
}

void processtemp3()
{
  int i,j;
  for(i=0;i<iunroll;i++){
    for(j=0;j<junroll;j++){
          ALLOC2
          sprintf(temp1->line,"u%d%d=0.0;\n",i,j);
          sprintf(temp2->line,"c[i+%d][j+%d]=u%d%d;\n",i,j,i,j);
          LINK2
          ALLOCtr2
          sprintf(temp_trace->line,"\n");
          sprintf(temp_trace2->line,"printf(\"w %%p\\n\",&(c[i+%d][j+%d]));\n",i,j);
          LINKtr2
          indentcnt--;
    }
  }
}

void processtemp4()
{
  int i,j,k;
  char build[80],temp[40];
  for(i=0;i<iunroll;i++){
    for(j=0;j<junroll;j++){
      for(k=0;k<kunroll;k++){
        build[0]='\0';
        if(uflag){
          sprintf(temp,"u%d%d+=",i,j);
          strcat(build,temp);
        }else{
          sprintf(temp,"c[i+%d][j+%d]+=",i,j);
          strcat(build,temp);
        }
        if(sflag){
          sprintf(temp,"s%d%d*",i,k);
          strcat(build,temp);
        }else{
          sprintf(temp,"a[i+%d][k+%d]*",i,k);
          strcat(build,temp);
        }
        if(tflag){
          sprintf(temp,"t%d%d;",k,j);
          strcat(build,temp);
        }else{
          sprintf(temp,"b[k+%d][j+%d];",k,j);
          strcat(build,temp);
        }
        ALLOC1
        sprintf(temp1->line,"%s\n",build);
        LINK1

        if(sflag==0){
          ALLOCtr
          sprintf(temp_trace->line,"printf(\"r %%p\\n\",&(a[i+%d][k+%d]));\n",i,k);
          LINKtr
        }
        if(tflag==0){
          ALLOCtr
          sprintf(temp_trace->line,"printf(\"r %%p\\n\",&(b[k+%d][j+%d]));\n",k,j);
          LINKtr
        }
        if(uflag==0){
          ALLOCtr
          sprintf(temp_trace->line,"printf(\"w %%p\\n\",&(c[i+%d][j+%d]));\n",i,j);
          LINKtr
        }
      }
    }
  }
}
