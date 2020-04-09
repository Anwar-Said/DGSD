#include <stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>
#include<igraph/igraph.h>
#include <mpi.h>
struct keep_neighbors{
        int d_j;
        int neighbors[200];
};

int main(void){

        MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    if(world_rank == 0){
            printf("i am here \n");
        igraph_t graph;
        FILE *fr;
        FILE *fw; 
        fr = fopen("graphs/ER10k.gxl","r");
        igraph_read_graph_graphml(&graph, fr,0);
        fclose(fr);
        printf("graph loaded successfully! \n");
        int V = igraph_vcount(&graph);
        printf("number of nodes in the graph %d \n",V);
        int batch = 1000;
        for (int i= 0; i<V; i++){
                
                if(i%batch==0){
                        // printf("it's %d",i);
                        // fclose(fw);
                        char fname[20];
                        char index[10];
                        char path[] = "data/10k/ER10k";
                        sprintf(index, "%d", i);
                        strcpy(fname, index);
                        strcat(fname, ".txt");
                        strcat(path,fname);
                        if (fw !=NULL){
                                fclose(fw);
                                // printf("file closed for batch %d", i);
                        }
                        fw = fopen(path,"a");
                        printf("%s \n", path);                
                }
        igraph_vector_t neighbors;
        igraph_vector_init(&neighbors, 1000);
        igraph_neighbors(&graph, &neighbors, i, IGRAPH_ALL);
        int size = (int)igraph_vector_size(&neighbors);
        int arr[size];
        // printf("neighbors computed \n");
        for (int i= 0; i<=size; i++)
        {
        int val = (int)VECTOR(neighbors)[i-1];
        fprintf(fw,"%d \t",val);

        }
         fprintf(fw,"%s \n","");  
        }
printf("done with writing******");
    }
    if (world_rank ==10){
            FILE *fr;
            int V = 1000;
            char c[1000];
            char * line = NULL;
            size_t len = 0;
            ssize_t read;
            int data [1000];
            int batch = 10000;
            struct keep_neighbors kn[10000]; 
            int new_index = 0;
            for (int i=0; i<V; i++){
                    if(i%batch==0){
                        char fname[20];
                        char index[10];
                        char path[] = "data/10k/ER10k";
                        sprintf(index, "%d", i);
                        strcpy(fname, index);
                        strcat(fname, ".txt");
                        strcat(path,fname);
                        if (fr !=NULL){
                                fclose(fr);
                                // printf("file closed for batch %d", i);
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
                        printf("data loaded for %d \n",i);
                    }
                int d_j = kn[new_index].d_j;
                
                int nei_j[d_j];
                for (int x = 0; x<d_j; x++){
                        nei_j[x] = kn[new_index].neighbors[x];
                }

                // printf("%d - %d \t", i,d_j);
                new_index++;
                if (new_index == 10000){
                        new_index = 0;
                }
                // if (i==0){
                // break;
                // }
                
        
        }
        printf("done with everything successfully!");
    }
     return 0;
}