#include <mpi.h>
#include <stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<igraph/igraph.h>
#include<string.h>
#include<math.h>
#include<time.h>
#include<sys/time.h>
#include <unistd.h>

void print_vector(igraph_vector_t *nei);
void send_batches(int,int,int);
int findIntersection(int a[], int b[], int m, int n);
int get_array(igraph_vector_t);
// clock_t start,end;
FILE *fp; 
FILE *tp; 
int cmpfunc(const void * a, const void * b);
struct keep_neighbors{
        int d_j;
        int neighbors[1000];
};
int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    
/*******************************************************workers**********************************************************/ 
    if (world_rank > 2) {
      printf("worker %d started: \n",world_rank);
      struct timeval start,end,w_start, w_end; 
        int arr[4];
        MPI_Recv(arr, 1000, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        gettimeofday(&start,  NULL);
        int batch_start = arr[0]; int batch_end = arr[1]; int nodes_count = arr[2];int emb_size = arr[3];
        
        int emb[emb_size];
        for (int e=0; e<emb_size; e++)
        {
          emb[e] = 0;
        }
        // printf("batch start: %d, batch end: %d, nodes_count: %d, \n", batch_start,batch_end,nodes_count);
        long worker_micro = 0;
        double bin_width = 1/emb_size;
        for (int i = batch_start; i<batch_end; i++)
        {
        if (batch_end == 500000){
                if(i%1000 == 0){
                     printf("i : %d \t",i);
                }
          }               
            //communication time
            gettimeofday(&w_start,NULL);
            MPI_Send(&i,1, MPI_INT, 0, 1,MPI_COMM_WORLD);
            int arr_i[1000];
            MPI_Recv(&arr_i, 1000, MPI_INT, MPI_ANY_SOURCE,1,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            gettimeofday(&w_end, NULL);
            long w_seconds = (w_end.tv_sec - w_start.tv_sec);
	          long w_micros = ((w_seconds * 1000000) + w_end.tv_usec) - (w_start.tv_usec);
            // printf("micor %ld,", w_micros);
            worker_micro += w_micros;
            int d_i =  arr_i[0];
            int nei_i[d_i];
            for (int x= 1; x<=d_i;x++)
            {
              nei_i[x-1] = arr_i[x];
            }
            FILE *fr_;
            char c[1000];
            char * line = NULL;
            size_t len = 0;
            ssize_t read;
            int batch = 1000;
            struct keep_neighbors kn[1000]; 
            int new_index = 0;
            for (int j= 0; j<nodes_count; j++)
            {
              if (i==j){
                emb[0] += 1;
                continue; 
              }
                //communication time
                gettimeofday(&w_start, NULL);
                // MPI_Send(&j,1, MPI_INT, 0, 1,MPI_COMM_WORLD);
                int arr_j[1000];
                // MPI_Recv(&arr_j, 1000, MPI_INT, 2,11,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if(j%batch==0){
                        if (fr_ !=NULL){
                                fclose(fr_);
                                // printf("file closed %s \n", path);
                        }
                        char fname[20];
                        char index[10];
                        char path[] = "data/10k/ER10k"; 
                        sprintf(index, "%d", j);
                        strcpy(fname, index);
                        strcat(fname, ".txt");
                        strcat(path,fname);
                        fr_ = fopen(path,"r");
                        // printf("file open for %s\n",path);
                        int ind = 0;
                        while ((read = getline(&line, &len, fr_)) != -1) {
                                // printf("Retrieved line of length %zu:\n", read);
                                // printf("%s \n", line);
                                // printf("size of line: %ld, %ld \n", sizeof(line), len);
                                char *res;
                                unsigned long num;
                                res = strtok(line,"\t");
                                // int nei[1000];
                                int count = 0;
                                while(res!=NULL){
                                        kn[ind].neighbors[count] = atoi(res);
                                        // printf(" %s - %d \t", res, nei[count]);
                                        res = strtok(NULL, "\t");
                                        count++;
                                }
                                kn[ind].d_j = count-1;
                                ind++;
                        }
                        // printf("data loaded for %d \n",j);
                    }

                // printf("neighbor received for %d", j);  
                gettimeofday(&w_end, NULL);  
                 w_seconds = (w_end.tv_sec - w_start.tv_sec);
                w_micros = ((w_seconds * 1000000) + w_end.tv_usec) - (w_start.tv_usec);
                // printf("micor %ld,", w_micros);
                worker_micro += w_micros;    
                int d_j = kn[new_index].d_j;
                
                int nei_j[d_j];
                for (int x = 0; x<d_j; x++){
                        nei_j[x] = kn[new_index].neighbors[x];
                }

                // printf("%d - %d \t", i,d_j);
                new_index++;
                if (new_index == batch){
                        new_index = 0;
                }

                // int d_j = arr_j[0];
                // int nei_j[d_j];
                // for (int z= 1; z<=d_j; z++)
                // {
                // nei_j[z-1] = arr_j[z];
                // }
                int delta = 0;
                int * key;

                key = (int*) bsearch (&i, &nei_j, d_j, sizeof(nei_j[0]), cmpfunc);
                if (key !=NULL){  
                    delta =1;
                }else{
                  delta = 0;
                } 
              int c_n = findIntersection(nei_i, nei_j, d_i, d_j);
              
              double dist = 0;
              if (d_i+d_j+c_n+delta>0)
              {
                dist = ((double)(d_i + d_j)/((d_i+d_j) + c_n + delta));
              }else{
                dist = 1.0;
              }
              delta = 0; 
              if (dist ==1.0){
                emb[emb_size-1] += +1; 
              }else{
              int bin = (dist * emb_size);
              emb[bin] = emb[bin]+1;
              }
            }
          // fclose(fr_);
        }
        gettimeofday(&end,  NULL);
        long seconds = (end.tv_sec - start.tv_sec);
	long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

	printf("worker: %d, communication time: %ld, and processing time: %ld, \n", world_rank, worker_micro/1000000,(micros-worker_micro)/1000000);
        tp = fopen("time_res.txt","a");
        fprintf(tp," worker, %d, nodes,%d, batch  size, %d, comm time, %ld, processing time, %ld \n", world_rank,nodes_count, (batch_end-batch_start),worker_micro/1000000,(micros-worker_micro)/1000000);
        fclose(tp);
         MPI_Send(&emb,1000, MPI_INT, 1, 10,MPI_COMM_WORLD);        
    } 

    if (world_rank  == 2) {
      printf("worker %d started: \n",world_rank);
      struct timeval start,end,w_start, w_end; 
        int arr[4];
        MPI_Recv(arr, 1000, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        gettimeofday(&start,  NULL);
        int batch_start = arr[0]; int batch_end = arr[1]; int nodes_count = arr[2];int emb_size = arr[3];
        int emb[emb_size];
        for (int e=0; e<emb_size; e++)
        {
          emb[e] = 0;
        }
        // printf("batch start: %d, batch end: %d, nodes_count: %d, \n", batch_start,batch_end,nodes_count);
        long worker_micro = 0;
        double bin_width = 1/emb_size;
        for (int i = batch_start; i<batch_end; i++)
        {
            //communication time
            gettimeofday(&w_start,NULL);
            MPI_Send(&i,1, MPI_INT, 0, 1,MPI_COMM_WORLD);
            int arr_i[1000];
            MPI_Recv(&arr_i, 1000, MPI_INT, MPI_ANY_SOURCE,1,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            gettimeofday(&w_end, NULL);
            long w_seconds = (w_end.tv_sec - w_start.tv_sec);
	          long w_micros = ((w_seconds * 1000000) + w_end.tv_usec) - (w_start.tv_usec);
            // printf("micor %ld,", w_micros);
            worker_micro += w_micros;
            int d_i =  arr_i[0];
            int nei_i[d_i];
            for (int x= 1; x<=d_i;x++)
            {
              nei_i[x-1] = arr_i[x];
            }
        //     int batch = 100;
            FILE *fr;
        //     int V = 1000;
            char c[1000];
            char * line = NULL;
            size_t len = 0;
            ssize_t read;
        //     int data [1000];
            int batch = 1000;
            struct keep_neighbors kn[1000]; 
            int new_index = 0;
            for (int j= 0; j<nodes_count; j++)
            {
              if (i==j){
                emb[0] += 1;
                continue; 
              }
              
                //communication time
                gettimeofday(&w_start, NULL);
                // MPI_Send(&j,1, MPI_INT, 0, 1,MPI_COMM_WORLD);
                int arr_j[1000];
                // MPI_Recv(&arr_j, 1000, MPI_INT, 0,1,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                // for (int p=3; p<world_size; p++){
                //         MPI_Send(&arr_j,1, MPI_INT, p, 11,MPI_COMM_WORLD);

                // } 
                if(j%batch==0){
                        char fname[20];
                        char index[10];
                        char path[] = "data/10k/ER10k";
                        sprintf(index, "%d", j);
                        strcpy(fname, index);
                        strcat(fname, ".txt");
                        strcat(path,fname);
                        if (fr !=NULL){
                                fclose(fr);
                                // printf("file closed for batch %d \n", j);
                        }
                        fr = fopen(path,"r");
                        int ind = 0;
                        while ((read = getline(&line, &len, fr)) != -1) {
                                // printf("Retrieved line of length %zu:\n", read);
                                // printf("%s \n", line);
                                // printf("size of line: %ld, %ld \n", sizeof(line), len);
                                char *res;
                                unsigned long num;
                                res = strtok(line,"\t");
                                // int nei[1000];
                                int count = 0;
                                while(res!=NULL){
                                        kn[ind].neighbors[count] = atoi(res);
                                        // printf(" %s - %d \t", res, nei[count]);
                                        res = strtok(NULL, "\t");
                                        count++;
                                }
                                kn[ind].d_j = count-1;
                                ind++;
                        }
                        // printf("data loaded for %d \n",i);
                    }
                gettimeofday(&w_end, NULL);  
                 w_seconds = (w_end.tv_sec - w_start.tv_sec);
                w_micros = ((w_seconds * 1000000) + w_end.tv_usec) - (w_start.tv_usec);
                // printf("micor %ld,", w_micros);
                worker_micro += w_micros;           
                // int d_j = arr_j[0];
                // int nei_j[d_j];
                // for (int z= 1; z<=d_j; z++)
                // {
                // nei_j[z-1] = arr_j[z];
                // }
                int d_j = kn[new_index].d_j;
                
                int nei_j[d_j];
                for (int x = 0; x<d_j; x++){
                        nei_j[x] = kn[new_index].neighbors[x];
                }
                new_index++;
                if (new_index == batch){
                        new_index = 0;
                }
                int delta = 0;
                int * key;
                key = (int*) bsearch (&i, &nei_j, d_j, sizeof(nei_j[0]), cmpfunc);
                if (key !=NULL){  
                    delta =1;
                }else{
                  delta = 0;
                } 
              int c_n = findIntersection(nei_i, nei_j, d_i, d_j);
              double dist = 0;
              if (d_i+d_j+c_n+delta>0)
              {
                dist = ((double)(d_i + d_j)/((d_i+d_j) + c_n + delta));
              }else{
                dist = 1.0;
              }
              
              delta = 0; 
              if (dist ==1.0){
                emb[emb_size-1] += +1; 
              }else{
              int bin = (dist * emb_size);
              emb[bin] = emb[bin]+1;
              }
              
            }  
            // fclose(fr);
        }
        gettimeofday(&end,  NULL);
        long seconds = (end.tv_sec - start.tv_sec);
	      long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

	printf("worker: %d, communication time: %ld, and processing time: %ld, \n", world_rank, worker_micro/1000000,(micros-worker_micro)/1000000);
        tp = fopen("time_res.txt","a");
        fprintf(tp," worker, %d, nodes,%d, batch  size, %d, comm time, %ld, processing time, %ld \n", world_rank,nodes_count, (batch_end-batch_start),worker_micro/1000000,(micros-worker_micro)/1000000);
        fclose(tp);
         MPI_Send(&emb,1000, MPI_INT, 1, 10,MPI_COMM_WORLD);        
    } 
    /*****************************************************aggregator*******************************************************/

    if (world_rank==1){
      /* AGGREGATOR */
      int emb_size = 100;
      int final_emb[emb_size];
//       MPI_Request request; 
      for (int i = 0; i<emb_size; i++)
      {
        final_emb[i] = 0;
      }
      
      int count = 2;
      while(true){
        int v[100];
         for (int i = 0; i<emb_size; i++)
        {
          v[i] = 0;
        }
        MPI_Recv(v, 1000, MPI_INT, MPI_ANY_SOURCE,10,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i=0; i<emb_size; i++){
          int res = v[i]+final_emb[i];
          final_emb[i] = res;
        }
        count++;
        if(count == world_size)
        {
          int i = -1; 
          MPI_Send(&i,1, MPI_INT, 0, 1,MPI_COMM_WORLD);
          break;
        }

      }
      
    }

/*********************************************************master*********************************************************/

    if (world_rank == 0) {
      struct timeval m_start,m_end,m_st,m_ed; 
      gettimeofday(&m_start,  NULL);
      /* master function */
        // struct timeval m_start,m_end, c_start, c_end;
        igraph_t graph;
        int N = 1000;
        int masters = 2;
        float p = 0.001;
        // printf("master: reading graph \n");
        
        // igraph_erdos_renyi_game(&graph, IGRAPH_ERDOS_RENYI_GNP,N,p,0,0);
        // waiting to done with the loading graph
        
        FILE *fr;
        fr = fopen("graphs/ER10k.gxl","r");
        igraph_read_graph_graphml(&graph, fr,0);
        fclose(fr);
        int E = igraph_ecount(&graph);
        int V = igraph_vcount(&graph);
        
        printf("total nodes and edges in the graph %d, %d\n",V, E);
        int workers = world_size-masters;
        printf("workers: %d \n", workers);
        int batch_size = V/workers;
        int emb_size = 100;
        for (int i = masters; i<workers+masters;i++)
        {
          int start = batch_size*(i-masters);
          int end = batch_size*(i-(masters-1));
          if(i+1 == workers+masters){
                  end = V;
          }
          printf("batch start %d, batch end %d, \n", start, end);
            int arr[4] = {start, end,V,emb_size};
            MPI_Send(&arr,1000, MPI_INT, i, 0,MPI_COMM_WORLD);
        }
        long master_micro = 0;
        while(true)
        {
            MPI_Status status;
            int node; 
            gettimeofday(&m_st,NULL);
            MPI_Recv(&node, 1, MPI_INT, MPI_ANY_SOURCE,1,MPI_COMM_WORLD, &status);
            gettimeofday(&m_ed,NULL);
            long m_seconds = (m_ed.tv_sec - m_st.tv_sec);
            long m_micros = ((m_seconds * 1000000) + m_ed.tv_usec) - (m_st.tv_usec);
            master_micro += m_micros; 
            if (node ==-1){
              break;
            }
            // igraph_vector_t neighbors = nei_arr[node].node_neighbors;
            igraph_vector_t neighbors;
            igraph_vector_init(&neighbors, 1000);
            igraph_neighbors(&graph, &neighbors, node, IGRAPH_ALL);
            
             int size = (int)igraph_vector_size(&neighbors);
              int arr[size+1];
              arr[0] = size;
              for (int i= 1; i<=size; i++)
              {
                arr[i] = (int)VECTOR(neighbors)[i-1];
                
              }
            
            gettimeofday(&m_st,NULL);           
            MPI_Send(arr,1000, MPI_INT, status.MPI_SOURCE, 1,MPI_COMM_WORLD);
             gettimeofday(&m_ed,NULL);
            m_seconds = (m_ed.tv_sec - m_st.tv_sec);
            m_micros = ((m_seconds * 1000000) + m_ed.tv_usec) - (m_st.tv_usec);
            master_micro += m_micros;
            igraph_vector_destroy(&neighbors);
            
            
        } 
        gettimeofday(&m_end, NULL);  
        long seconds = (m_end.tv_sec - m_start.tv_sec);
	      long micros = ((seconds * 1000000) + m_end.tv_usec) - (m_start.tv_usec);
        printf(" master, %d,  comm time, %ld, processing time, %ld \n", world_rank,master_micro/1000000,(micros-master_micro)/1000000);
        tp = fopen("time_res.txt","a");
        fprintf(tp," master, %d,  comm time, %ld, processing time, %ld \n", world_rank,master_micro/1000000,(micros-master_micro)/1000000);
        fclose(tp);
    }

  
    MPI_Finalize();
}

/*********************************************************************************************************************/

void send_batches(int batch_start, int batch_end, int dest)
{
  int arr[2] = {batch_start, batch_end};
  MPI_Send(&arr,1000, MPI_INT, dest, 0,MPI_COMM_WORLD);
}

void print_vector(igraph_vector_t *nei)
{
    if (igraph_vector_size(nei) == 0)
    {
      printf("no neighbors found\n");
    }else{
      printf("%d neighbors found \n", (int)igraph_vector_size(nei));
    }
    for (long int i= 0; i<igraph_vector_size(nei); i++)
    {
      printf("%d \n",(int)VECTOR(*nei)[i]);
    }
    
}
int cmpfunc(const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int findIntersection(int a[], int b[], int m, int n) 
{ 
  
  int count = 0; 
  int i = 0, j = 0; 
  while (i < m && j < n) 
  { 
    if (a[i] <b[j]) 
      i++; 
    else if (b[j] < a[i]) 
      j++; 
    else /* if arr1[i] == arr2[j] */
    { 
      count++; 
      i++; 
    } 
  } 
  return count; 
}