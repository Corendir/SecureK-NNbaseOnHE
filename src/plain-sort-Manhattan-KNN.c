#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

int main() {

    //declaration of interesting indexes
    int numberOfVectors = 10;
    int kParam = 7;
    //int modelIndexes [10] = {864018, 857010, 855563, 848406, 857156, 862717, 8510653, 86208, 861598, 852781}; //SET N°1 DEFAULT
    int modelIndexes [10] = {84501001, 849014, 8510426, 852781, 89869, 899187, 9113156, 915460, 915143, 925311}; //SET N°2
    int sourceIndex = /*842302*/857374;
    char modelClasses [10];
    char sourceClass;
    int modelParameters [10][32];
    int sourceParameters [32];
    float f_modelParameters [10][32];
    float f_sourceParameters [32];

    //import the radiuses from the dataset
    FILE* dataset_data = fopen("wdbc.data","r");
    if (dataset_data == NULL){
        printf("Cannot find the dataset wdbc.data");
        exit(0);
    }
    else {
        for(int i = 0;i<numberOfVectors+1;i++){
            int tempIndex = 0;
            char tempClass;
            int temp3;
            int temp4;
            float tempParameters[30];
            float tempTEST, tempTEST2, tempTEST3;
            char line[300];
            int nb_val_lues;
            //read by line
            while (fgets(line, sizeof(line), dataset_data) != NULL) {
                //get the values of current line
                //nb_val_lues = sscanf( line, "%d,%c,%d.%d", &temp1, &temp2[i], &temp3, &temp4) ;
                nb_val_lues = sscanf( line, "%d,%c,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                 &tempIndex, &tempClass, &tempParameters[0], &tempParameters[1], &tempParameters[2], &tempParameters[3], &tempParameters[4],
                  &tempParameters[5], &tempParameters[6], &tempParameters[7], &tempParameters[8], &tempParameters[9], &tempParameters[10],
                   &tempParameters[11], &tempParameters[12], &tempParameters[13], &tempParameters[14], &tempParameters[15], &tempParameters[16],
                    &tempParameters[17], &tempParameters[18], &tempParameters[19], &tempParameters[20], &tempParameters[21], &tempParameters[22],
                     &tempParameters[23], &tempParameters[24], &tempParameters[25], &tempParameters[26],&tempParameters[27], &tempParameters[28],
                      &tempParameters[29], &tempParameters[30], &tempParameters[31]);
                //printf("Index : %d ; Class : %c ; %f , %f , %f\n", tempIndex, tempClass, tempParameters[0], tempParameters[1], tempParameters[2]); //TEST
                //check if this is the good line
                if(i<numberOfVectors)
                    if(tempIndex == modelIndexes[i]){
                        //register the values
                        modelClasses[i] = tempClass;
                        for(int j = 0;j<32;j++)
                            f_modelParameters[i][j] = tempParameters[j];
                        //printf("\nN°%d\n", i); //TEST
                        //printf("temp: %d\nclass: %c\nradius: %d\nnb_val_lues: %d\n", temp1, temp2[i], radiuses[i], nb_val_lues); //TEST
                    }
                else
                    if(tempIndex == sourceIndex){
                        //register the values
                        sourceClass = tempClass;
                        for(int j = 0;j<32;j++)
                            f_sourceParameters[j] = tempParameters[j];
                        //printf("\nN°%d\n", i); //TEST
                        //printf("temp: %d\nclass: %c\nradius: %d\nnb_val_lues: %d\n", temp1, temp2[i], radiuses[i], nb_val_lues); //TEST
                    }
            }
            rewind(dataset_data);
        }
    }
    fclose(dataset_data);

    //TEST
    for(int p=0;p<30;p++)
        printf("vecteur %d, param %d : %f\n", 1, p, f_modelParameters[0][p]); //TEST


    //Normalize model and source vectors by parameter
    for(int i = 0;i<30;i++){
        //Init min max
        float min = f_modelParameters[0][i];
        float max = f_modelParameters[0][i];
        //Identify min max
        for(int j = 0;j<numberOfVectors;j++){
            if(f_modelParameters[j][i]<min)
                min = f_modelParameters[j][i];
            if(f_modelParameters[j][i]>max)
                max = f_modelParameters[j][i];
        }
        //Normalize
        for(int j = 0;j<numberOfVectors;j++)
            modelParameters[j][i] = (int) (((f_modelParameters[j][i] - min) / (max - min)) * 1000);
        sourceParameters[i] = (int) (((f_sourceParameters[i] - min) / (max - min)) * 1000);
    }

    //TEST
    for(int p=0;p<30;p++)
        printf("vecteur %d, param %d : %d\n", 1, p, modelParameters[0][p]); //TEST
    printf("\nINFO\n%d\n", sourceParameters[0]); //INFO


    //-|-|-Homomorphic computation-|-|-
    printf("doing the homomorphic computation...\n");
    printf("operation : build delta matrix\n");

    time_t start_timeComputation = clock(); //TIME

    ///1-Build delta list

    //Declaration
    int deltaList[10];

    time_t start_timeDeltaList = clock(); //TIME

    printf("\nINFO\n%d   %d\n", modelParameters[0][0], sourceParameters[0]); //INFO

    //Computation
    //For each vector
    for(int i = 0;i<numberOfVectors;i++){
        deltaList[i] = 0;
        //For each parameters
        for(int j = 0;j<1;j++){ //1 paramètre sur 30 (essai)
            //Compute model - source
            if(i==2)
                printf("vector 2, dif %d -> %d\n", j, modelParameters[i][j] - sourceParameters[j]);
            deltaList[i] += abs(modelParameters[i][j] - sourceParameters[j]);
        }
    }

    //TEST
    printf("\nDELTA LIST\n");
    for(int i = 0;i<numberOfVectors;i++)
        printf("Vector n°%d -> %d\n", i, deltaList[i]);
    //printf("\nvecteur %d, delta : %d\n", 3, deltaList[3]); //TEST
    //printf("\nvecteur %d, delta : %d\n", 20, deltaList[20]); //TEST

    time_t end_timeDeltaList = clock(); //TIME
    printf("......DeltaList took: %ld microsecs\n",end_timeDeltaList-start_timeDeltaList); //TIME

    time_t start_timeSort = clock(); //TIME

    ///2-Sort
    int dOriginalRanks[10] = {0,1,2,3,4,5,6,7,8,9};

    /*//Tri par sélection
    for (int c = 0; c < (10 - 1); c++){
        int position = c;

        for (int d = c + 1; d < 10; d++){
            if (deltaList[position] > deltaList[d])
                position = d;
        }
        if (position != c){
            int t1 = deltaList[c];
            int t2 = modelClasses[c];
            int t3 = dOriginalRanks[c];
            deltaList[c] = deltaList[position];
            modelClasses[c] = modelClasses[position];
            dOriginalRanks[c] = dOriginalRanks[position];
            deltaList[position] = t1;
            modelClasses[position] = t2;
            dOriginalRanks[position] = t3;
        }
    }*/

    //Tri par insertion
    /*int flag = 0;
    for (int i = 1 ; i <= 10 - 1; i++) {
        int t1 = deltaList[i];
        int t2 = modelClasses[i];
        int t3 = dOriginalRanks[i];
        int j = 0;
        for (j = i - 1 ; j >= 0; j--) {
            if (deltaList[j] > t1){
                deltaList[j+1] = deltaList[j];
                modelClasses[j+1] = modelClasses[j];
                dOriginalRanks[j+1] = dOriginalRanks[j];
                flag = 1;
            }
        }
        if (flag)
        deltaList[j+1] = t1;
        modelClasses[j+1] = t2;
        dOriginalRanks[j+1] = t3;
    }*/
    /*int i,j,t1,t2,t3;
    for (i=1;i<10-1;i++) {
        t1 = deltaList[i];
        t2 = modelClasses[i];
        t3 = dOriginalRanks[i];
        for (j=i;j>0 && deltaList[j-1] > t1;j--) {
            deltaList[j] = deltaList[j-1];
            modelClasses[j] = modelClasses[j-1];
            dOriginalRanks[j] = dOriginalRanks[j-1];
        }
        deltaList[j] = t1;
        modelClasses[j] = t2;
        dOriginalRanks[j] = t3;
    }*/

    //Bubble sort
    int t1,t2,t3;
    for (int i=10-1;i>0;i--){
        for (int j=0;j<i;j++) {
            if(deltaList[j+1] < deltaList[j]){
                t1 = deltaList[j];
                t2 = modelClasses[j];
                t3 = dOriginalRanks[j];
                deltaList[j] = deltaList[j+1];
                modelClasses[j] = modelClasses[j+1];
                dOriginalRanks[j] = dOriginalRanks[j+1];
                deltaList[j+1] = t1;
                modelClasses[j+1] = t2;
                dOriginalRanks[j+1] = t3;
            }
        }
    }

    time_t end_timeSort = clock(); //TIME
    printf("......Sort took: %ld microsecs\n",end_timeSort-start_timeSort); //TIME

    //TEST
    printf("\nSORTED DELTA LIST\n");
    for(int v=0;v<numberOfVectors;v++)
        printf("rank : %d ; originalRank = %d ; delta : %d ; class : %c\n", v, dOriginalRanks[v], deltaList[v], modelClasses[v]); //TEST

    time_t start_timeVote = clock(); //TIME

    ///3-Vote
    int countB = 0;
    int countM = 0;
    int addB ;//bit
    int addM;//bit
    for(int i=0;i<kParam;i++){
        addB = (modelClasses[i]=='B')?1:0;
        addM = (modelClasses[i]=='M')?1:0;
        countB += addB;
        countM += addM;
    }
    char predictedClass = (countB<=countM)?'M':'B';

    //TEST
    printf("\nVOTE\n");
    printf("countB : %d\ncountM : %d\npredicted class : %c\n", countB, countM, predictedClass);
    printf("right class : %c\n", sourceClass);

    time_t end_timeVote = clock(); //TIME
    printf("......Vote took: %ld microsecs\n",end_timeVote-start_timeVote); //TIME

    /*
    time_t start_timeDeltaMatrix = clock(); //TIME

    //2-Build delta matrix

    //Declaration
    int deltaMatrix[10][10];

    //Computation for corners
    //For each line
    for(int i=0;i<numberOfVectors;i++){
        //For each column
        for(int j=0;j<i;j++){
            //Avoid the diagonal
            if(i!=j){
                deltaMatrix[i][j] = (deltaList[i] < deltaList[j])?1:0;
                deltaMatrix[j][i] = (deltaMatrix[i][j]==1)?0:1;
            }
        }
    }
    //Computation for diagonal
    //For each line
    for(int i=0;i<numberOfVectors;i++){
        deltaMatrix[i][i] = 0;
    }*/


    /*//Complete matrix for top-right corner
    //For each column
    for(int i=1;i<numberOfVectors;i++){
        //For each line
        for(int i=j+1;i<numberOfVectors;i++){
            deltaMatrix[j][i] = deltaMatrix[i][j];
        }
    }*/

    /*//TEST
    printf("\nMATRIX\n");
    for(int i=0;i<numberOfVectors;i++){
        for(int j=0;j<numberOfVectors;j++)
            printf("%d ", deltaMatrix[i][j]); //TEST
        printf("\n"); //TEST
    }

    time_t end_timeDeltaMatrix = clock(); //TIME
    printf("......DeltaMatrix took: %ld microsecs\n",end_timeDeltaMatrix-start_timeDeltaMatrix); //TIME

    time_t start_timeFindNeighbour = clock(); //TIME

    //3-Find neighbour

    //d line sum
    //Init
    int dSum[10];
    //For each vector
    for(int i=0;i<numberOfVectors;i++){
        dSum[i] = 0;
    }

    printf("------------------------------ %d\n", deltaMatrix[3][0]); //TEST
 
    //Compute
    //For each line
    for(int i=0;i<numberOfVectors;i++)
        //For each column
        for(int j=0;j<numberOfVectors;j++){
            dSum[i] += deltaMatrix[i][j];
        }
    
    //TEST
    for(int v=0;v<numberOfVectors;v++)
        printf("sum of vector %d : %d\n", v, dSum[v]); //TEST*/

    //time_t start_timeFindNeighbour = clock(); //TIME

    ///3-Find neighbour

    /*//d-k
    int dMinusk = numberOfVectors - kParam;
    printf("dMinusk : %d\n", dMinusk); //TEST*/

    //Compar
    /*int isInK[10];
    //For each vector
    for(int i=0;i<numberOfVectors;i++){
        isInK[i] = (dSum[i]>=dMinusk)?1:0;
    }*/

    //time_t end_timeFindNeighbour = clock(); //TIME
    //printf("......FindNeighbour took: %ld microsecs\n",end_timeFindNeighbour-start_timeFindNeighbour); //TIME

    time_t end_timeComputation = clock(); //TIME
    printf("......Computation took: %ld microsecs\n",end_timeComputation-start_timeComputation); //TIME
    
    //Print
    //For each vector
    /*for(int i=0;i<numberOfVectors;i++)
        printf("Vector n°%d -> %d\n", i+1, isInK[i]);*/

}
