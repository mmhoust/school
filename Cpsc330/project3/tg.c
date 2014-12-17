#define KKSTEP 8
#define JJSTEP 8
#define IISTEP 8
#include <stdio.h>
#include <stdlib.h>
#define min(a,b) ((a)<=(b)?(a):(b))

double a[320][320],b[320][320],c[320][320],d[320][320];

main(){
  int i,j,k;
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

  for(i=0;i<320;i++){
    for(j=0;j<320;j++){
      c[i][j]=0.0;
        printf("w %p\n",&(c[i][j]));    }
  }
  for(ii=0;ii<320;ii+=IISTEP){
    it=min(ii+IISTEP,320);
    for(jj=0;jj<320;jj+=JJSTEP){
      jt=min(jj+JJSTEP,320);
      for(kk=0;kk<320;kk+=KKSTEP){
        kt=min(kk+KKSTEP,320);
        for(i=ii;i<it;i+=2){
          for(j=jj;j<jt;j+=2){
            u00=c[i  ][j  ];
              printf("r %p\n",&(c[i  ][j  ]));
            u01=c[i  ][j+1];
              printf("r %p\n",&(c[i  ][j+1]));
            u10=c[i+1][j  ];
              printf("r %p\n",&(c[i+1][j  ]));
            u11=c[i+1][j+1];
              printf("r %p\n",&(c[i+1][j+1]));
            for(k=kk;k<kt;k++){
              s00=a[i  ][k  ];
              printf("r %p\n",&(a[i  ][k  ]));
              s10=a[i+1][k  ];
              printf("r %p\n",&(a[i+1][k  ]));
              t00=b[k  ][j  ];
              printf("r %p\n",&(b[k  ][j  ]));
              t01=b[k  ][j+1];
              printf("r %p\n",&(b[k  ][j+1]));
              u00+=s00*t00;
              u01+=s00*t01;
              u10+=s10*t00;
              u11+=s10*t01;
            }
            c[i+1][j+1]=u11;
            printf("w %p\n",&(c[i+1][j+1]));
            c[i+1][j  ]=u10;
            printf("w %p\n",&(c[i+1][j  ]));
            c[i  ][j+1]=u01;
            printf("w %p\n",&(c[i  ][j+1]));
            c[i  ][j  ]=u00;
            printf("w %p\n",&(c[i  ][j  ]));
          }
        }
      }
    }
  }

}
