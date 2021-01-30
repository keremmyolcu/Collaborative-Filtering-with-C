#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>


void printMatrix(int **arr,int n,int m);
float sim(int **arr,int u1,int u2,int movnum);
int *benzeyenKlar(int **arr,int user,int movnum,int usnum,int k,char users[50][30]);
float *selectKBest(float *arr, int k,int n);
void swap(float *xp, float *yp);
float elemanOrtBul(int **arr,int user,int movnum);
float pred(int **arr,int anaUser,int kitapindex,int movnum,int usnum, int k,int *keleman,char movies[30][30]);
float ortakliOrtBul(int **arr,int user,int user2,int movnum);

int main(){
	int usnum, movnum,i=0,j=0,indis,kbest;
	int **utility;
	FILE *fp;
	char line[300];
	char users[50][30],movies[30][30];	//max 50 user and 30 movies
	int *indexes;

	printf("Bilgi almak istediginiz kullanicinin indisini giriniz: \n Ornegin U2 icin 2:\n");
	scanf("%d",&indis);
	printf("En cok benzeyen k degerini giriniz:\n");
	scanf("%d",&kbest);
	
	fp = fopen("RecommendationDataSet.csv","r");
	if(fp == NULL){
		printf("Couldnt open data file,exitting..\n");
		exit(0);
	}
	char *token;
	fgets(line,299,fp);
	token = strtok(line,";\n");
	
	while(token!=NULL){
		if(j>0){
			strcpy(movies[j-1],token);
		}
		j++;
		token=strtok(NULL,";\n");
		
	}
	while(fgets(line,299,fp)){
		token=NULL;
		movnum =0;
		token = strtok(line,";\n");
		//if(usnum >= 1){
			strcpy(users[i],token);
			i++; 
		//}
		while(token!=NULL){
			if(strcmp(token," ") == 0 || strcmp(token,"") == 0){
				//printf("B");
			}
			else{
				//printf("%s",token);	
			}
			token=strtok(NULL,";\n");
			movnum++;
		}
		usnum++;		
	}
	rewind(fp);
	
	usnum = usnum;
	movnum = movnum-1;
	utility =  (int**)malloc(usnum*sizeof(int*));
	for (i = 0; i < usnum; i++)
    	utility[i] = (int*)malloc(movnum*sizeof(int));
	
	if(utility == NULL){
		printf("Could allocate matrix, exitting..\n");
		exit(0);
	}
	
	int satir=0;
	int sutun=0;
	fgets(line,299,fp);
	while(fgets(line,299,fp)){

		token=NULL;
		token = strtok(line,";\n");

		while(token!=NULL){
			if(strcmp(token," ") != 0 && strcmp(token,"") != 0){
				if(sutun>0){
					utility[satir][sutun-1] = atoi(token);
				}
			}
			else{
				utility[satir][sutun-1] = 0;				//atoi kullan
			}
			token=strtok(NULL,";\n");
			sutun++;
		}
		sutun =0;
		satir++;		
	}

	indexes = benzeyenKlar(utility,indis-1,movnum,usnum,kbest,users);
	
	for(i=0; i<movnum; i++)
		if(utility[indis][i] != 0)
			pred(utility,indis-1,i,movnum,usnum,kbest,indexes,movies);
	
	fclose(fp);
	
	
	return 0;
}

void printMatrix(int **arr,int n,int m){
	int i,j;
	for(i=0;i<n; i++){
		for(j=0; j<m; j++){
			printf("%d	",arr[i][j]);
		}
		printf("\n");
	}
}
float sim(int **arr,int u1,int u2,int movnum){
	int i,j=0,u1sum=0,u2sum=0,u1cnt=0,u2cnt=0;
	float u1mn,u2mn,result=0.0,payda1=0.0,payda2=0.0;		//ortak okuduklari kitaplarin ortalamalari
	for(i=0; i<movnum; i++){						//movnum = film sayisi
		if(arr[u1][i] != 0){
			u1sum = u1sum+arr[u1][i];
			u1cnt++;
		}
		if(arr[u2][i] != 0){
			u2sum = u2sum+arr[u2][i];
			u2cnt++;
		}		
						
	}
	u1mn = (float)u1sum/(float)u1cnt;				
	u2mn = (float)u2sum/(float)u2cnt;
	
	for(i=0; i<movnum; i++){
			if(ortakMi(arr,u1,u2,i)){
				result = result + ((float)arr[u1][i]-u1mn)*((float)arr[u2][i] - u2mn); //sim'de pay
			//	printf("RESULT %d %.3f \n\n",i,result);
			}
	}
	//printf("PAY : %.3f\n\n",result);
	for(i=0; i<movnum; i++){
		if(ortakMi(arr,u1,u2,i)){
			payda1 = payda1 + ((float)arr[u1][i]-u1mn)*((float)arr[u1][i]-u1mn); //sim'de paydanin sol kismi(u1 icin)
		}
	}
	payda1 = sqrt(payda1);
	//printf("Payda sol : %.3f\n\n",payda1);	
	for(i=0; i<movnum; i++){
		if(ortakMi(arr,u1,u2,i)){
			payda2 = payda2 +  ((float)arr[u2][i] - u2mn)*((float)arr[u2][i] - u2mn);//sim'de paydanin sag kismi(u2 icin)
		}
	}	
	payda2 = sqrt(payda2);
	//printf("Payda sag : %.3f\n\n",payda2);
	return (result)/(payda1*payda2);
}

void swap(float *xp, float *yp) 
{ 
    float temp = *xp; 
    *xp = *yp; 
    *yp = temp; 
} 

float *selectKBest(float *arr, int k,int n) 
{ 
    int i, j, max_idx; 
  
    // One by one move boundary of unsorted subarray 
    for (i = 0; i < k; i++) 
    { 
        // Find the minimum element in unsorted array 
        max_idx = i; 
        for (j = i+1; j < n; j++) 
          if (arr[j] > arr[max_idx]) 
            max_idx = j; 
  
        // Swap the found minimum element with the first element 
        swap(&arr[max_idx], &arr[i]); 
    }
	return arr; 
} 

int *benzeyenKlar(int **arr,int user,int movnum,int usnum,int k,char users[50][30]){
	int i,j,index=0;
	float *points=(float *)malloc(sizeof(float)*usnum);
	float *pointsyedek=(float*)malloc(sizeof(float)*usnum);
	int *kbestindex=(int*)malloc(sizeof(int)*k);
	for(i=0; i<usnum; i++){
		if(i != user){
			points[i] = sim(arr,user,i,movnum);
			pointsyedek[i] = sim(arr,user,i,movnum);
		}
		else{
			points[i] = -2.0;
			pointsyedek[i] = -2.0;
		}
	}
	
	selectKBest(points,k,usnum);
	printf("%s 'e En cok benzeyen:\n",users[user]);
	for(i=0; i<k; i++){
		for(j=0; j<usnum; j++){
			if(points[i] == pointsyedek[j]){
				printf("%d. eleman %s ----> %.3f\n",i+1,users[j],points[i]);
				kbestindex[index] = j;	//k en benzer elemanlarin indexleri
				index++;
			}
		}
		
	}
	return kbestindex;
}


float elemanOrtBul(int **arr,int user,int movnum){
	float ort=0.0;
	int i,toplam,cnt=0;
	for(i=0; i<movnum; i++){
		if(arr[user][i]!=0){
			toplam = toplam + arr[user][i];	
			cnt++;
		}
	}
	return (float)toplam/(float)cnt;
}


float ortakliOrtBul(int **arr,int user,int user2,int movnum){
	float ort=0.0;
	int i,toplam,cnt=0;
	for(i=0; i<movnum; i++){
		if(ortakMi(arr,user,user2,i)){
			toplam = toplam + arr[user][i];	
			cnt++;
		}
	}
	return (float)toplam/(float)cnt;
}

float pred(int **arr,int anaUser,int kitapindex,int movnum,int usnum, int k,int *keleman,char movies[30][30]){
	float pay=0.0,payda=0.0,anaOrt,kOrt;
	int i,u1cnt=0,u1sum,genelcnt,genelsum;
	
	for(i=0; i<k; i++){
		if(i != anaUser){
			payda = payda + sim(arr,anaUser,i,movnum);
		}
	}	
	anaOrt = elemanOrtBul(arr,anaUser,movnum);
	
	for(i=0; i<k; i++){							//keleman en cok benzeyenlerin k buyuklugundeki dizisi
		if(arr[keleman[i]][kitapindex != 0])
			pay = pay + (sim(arr,anaUser,keleman[i],movnum))*((float)arr[keleman[i]][kitapindex] - elemanOrtBul(arr,keleman[i],movnum));
	}				//sim(a,b)  *   (rbp - rb) 													ortaklarin ortalama veya normal ortalama olabilir
	
	if(arr[anaUser][kitapindex] != 0)
		printf("%s e tahmini puani : %.3f\n\n",movies[kitapindex],anaOrt+(pay/payda));
	
	return anaOrt+(pay/fabs(payda));
}




int ortakMi(int **arr,int u1,int u2,int i){
	if((arr[u1][i] != 0) && (arr[u2][i] != 0)){
		return 1;
	}
	return 0;
} 

