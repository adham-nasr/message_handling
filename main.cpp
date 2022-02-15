#include<stdio.h>
#include<semaphore.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#include <unistd.h>
#include<map>
#include<vector>
using namespace std;

int ind_in=0,ind_out=0,counter=0,Size;
sem_t sc,sb,n,e;
map<pthread_t,int> m;
vector<int> buffer(10);

void* CounterFun(void* p){
	while(true){

        /// COUT STREAMS <<    Interleaving Cout outputs

        printf("Counter thread %d : recived a message\n",m[pthread_self()]);

        printf("Counter thread %d : waiting to write\n",m[pthread_self()]);

        sem_wait(&sc);
            counter++;
            printf("Counter thread %d : now adding to counter , counter value = %d\n",m[pthread_self()],counter);
		sem_post(&sc);


		int rn = rand()%5 + 2;  //2
		sleep(rn);
	}
}

void* MonitorFun(void *p){
	while(true)
	{
        int monitor;

        printf("Monitor thread : waiting to read counter\n");
        sem_wait(&sc);
            monitor = counter;
            counter = 0;
            printf("Monitor thread : reading a count of value %d\n",monitor);
		sem_post(&sc);


		int temp=-1,ret;
        ret = sem_getvalue(&e,&temp);

        if(!temp && !ret)
            printf("Monitor thread : buffer full !!\n");

        sem_wait(&e);
        sem_wait(&sb);
            buffer[ind_in%Size] = monitor;
            printf("Monitor thread : writing to buffer at position %d\n",ind_in%Size + 1);
            ind_in++;
        sem_post(&sb);
        sem_post(&n);


		int rn = rand()%17 + 3;  // 3
		sleep(rn);
	}
}


void* CollectFun(void *p){

    while(true)
    {

        int temp=-1,ret;

        ret = sem_getvalue(&n,&temp);

        if(!temp && !ret)
            printf("Collector thread : nothing is in the buffer\n");

        sem_wait(&n);
        sem_wait(&sb);
            printf("Collector thread : reading from buffer at position %d\n",ind_out % Size  +1);
            ind_out ++;
        sem_post(&sb);
        sem_post(&e);


		int rn = rand()%20 + 3;   // 3
		sleep(rn);
	}
}

int main()
{

    int nth;
    printf("\nEnter number of counter threads : ");
    scanf("%d",&nth);
    printf("Enter the size of Buffer : ");
	scanf("%d",&Size);

    buffer.resize(Size);

	srand(time(0));

    sem_init(&sc, 0, 1);
    sem_init(&sb, 0, 1);
    sem_init(&n, 0, 0);
    sem_init(&e, 0, Size);

    pthread_t th1,th2,tharr[nth];

	for(int i=0;i<nth;i++)
    {
        if(pthread_create(&tharr[i],NULL,&CounterFun,NULL))
        {
            printf("error creating Counter thread\n");
            return 0;
        }
        m[tharr[i]]=i+1;
    }
    if(pthread_create(&th1,NULL,&MonitorFun,NULL))
    {
        printf("error creating Monitor thread\n");
        return 0;
    }
    if(pthread_create(&th2,NULL,&CollectFun,NULL))
    {
        printf("error creating Collector thread\n");
        return 0;
    }

    pthread_join(th1, NULL);

    pthread_join(th2, NULL);

    for(int i=0;i<nth;i++)
        pthread_join(tharr[i], NULL);

}
