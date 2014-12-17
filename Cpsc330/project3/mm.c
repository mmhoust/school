#define KKSTEP 8
#define JJSTEP 8
#define IISTEP 8
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#define min(a,b) ((a)<=(b)?(a):(b))
double second();
double t_start,t_end;
double sum,row_sum;

double a[1000][1000],b[1000][1000],c[1000][1000],d[1000][1000];

main(){
  int i,j,k;
  double mflop = (2.0*1000*1000*1000)/1.0e6;
  int ii,it;
  int jj,jt;
  int kk,kt;
  double s00;
  double s10;
  double t00;
  double t01;
  double u00;
  double u01;
  double u10;
  double u11;
/* set coefficients so that result matrix should have 
 * row entries equal to (1/2)*n*(n-1)*i in row i
 */
  for (i=0;i<1000;i++){
    for (j=0;j<1000;j++){
      a[i][j] = b[i][j] = (double) i;
    }
  }
/* try to flush cache */
  for(i=0;i<1000;i++){
    for (j=0;j<1000;j++){
      d[i][j] = 0.0;
    }
  }
/* begin */
t_start=second();
  for(i=0;i<1000;i++){
    for(j=0;j<1000;j++){
      c[i][j]=0.0;
    }
  }
  for(ii=0;ii<1000;ii+=IISTEP){
    it=min(ii+IISTEP,1000);
    for(jj=0;jj<1000;jj+=JJSTEP){
      jt=min(jj+JJSTEP,1000);
      for(kk=0;kk<1000;kk+=KKSTEP){
        kt=min(kk+KKSTEP,1000);
        for(i=ii;i<it;i+=2){
          for(j=jj;j<jt;j+=2){
            u00=c[i  ][j  ];
            u01=c[i  ][j+1];
            u10=c[i+1][j  ];
            u11=c[i+1][j+1];
            for(k=kk;k<kt;k++){
              s00=a[i  ][k  ];
              s10=a[i+1][k  ];
              t00=b[k  ][j  ];
              t01=b[k  ][j+1];
              u00+=s00*t00;
              u01+=s00*t01;
              u10+=s10*t00;
              u11+=s10*t01;
            }
            c[i+1][j+1]=u11;
            c[i+1][j  ]=u10;
            c[i  ][j+1]=u01;
            c[i  ][j  ]=u00;
          }
        }
      }
    }
  }
/* end */
t_end = second();
/* check result */
  sum = 0.5*((double)(1000*(1000-1)));
  for (i=0;i<1000;i++){
    row_sum = ((double)i)*sum;
    for (j=0;j<1000;j++){
      if (c[i][j]!=row_sum){
        printf("error in result entry c[%d][%d]: %e != %e\n",
               i,j,c[i][j],row_sum);
        exit(1);
      }
    }
  }
  printf("for mm algorithm 8d8e8f2i2jk");
  printf(" utime is %10.2f secs\n",t_end-t_start);
  printf(" mflops is %10.2f\n",mflop/(t_end-t_start));
}
double second(){
  struct rusage ru;
  double t;
  getrusage(RUSAGE_SELF,&ru);
  t = ((double)ru.ru_utime.tv_sec) +
    ((double)ru.ru_utime.tv_usec)/1.0e6;
  return t;
}
