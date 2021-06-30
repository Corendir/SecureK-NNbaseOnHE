#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>


int main() {
    //printf("TEST 1\n");
    //reads the cloud key from file
    FILE* secret_key = fopen("secret-KNN.key","rb");
    TFheGateBootstrappingSecretKeySet* key = new_tfheGateBootstrappingSecretKeySet_fromFile(secret_key);
    fclose(secret_key);
    //if necessary, the params are inside the key
    const TFheGateBootstrappingParameterSet* params = key->params;
    int16_t numberOfVectors = 10;

    ///QUESTION

    /*//read the 10*32 ciphertexts of the question
    LweSample* indexes [10];
    for(int i = 0;i<numberOfVectors;i++)
        indexes[i] = new_gate_bootstrapping_ciphertext_array(32, params);   

    //import the 24*32 ciphertexts of the result from the anwer file
    FILE* indexes_data = fopen("indexes-KNN.data","rb");
    for(int i = 0;i<numberOfVectors;i++)
        for (int j=0; j<32; j++)
            import_gate_bootstrapping_ciphertext_fromFile(indexes_data, &indexes[i][j], params);
    fclose(indexes_data);

    //decrypt and rebuild the 24 questions
    int32_t int_indexes [10];
    for(int i = 0;i<numberOfVectors;i++){
        int_indexes[i] = 0;
        for(int j = 0;j<32;j++){
            int ai = bootsSymDecrypt(&indexes[i][j], key)>0;
            //printf("%d bit n° %d = %d\n", i, j, ai);
            int_indexes[i] |= (ai<<j);

        }
        //printf("And the question n°%d is: %d\n",i+1,int_questions[i]);
    }

    //read the 1*32 ciphertext of the source index
    LweSample* sourceIndex = new_gate_bootstrapping_ciphertext_array(32, params);   

    //import the 1*32 ciphertexts of the source index
    FILE* sourceIndex_data = fopen("sourceIndex-KNN.data","rb");
    for (int j=0; j<32; j++)
        import_gate_bootstrapping_ciphertext_fromFile(sourceIndex_data, &sourceIndex[j], params);
    fclose(sourceIndex_data);

    //decrypt and rebuild the source index
    int32_t int_sourceIndex = 0;
    for(int j = 0;j<32;j++){
        int ai = bootsSymDecrypt(&sourceIndex[j], key)>0;
        int_sourceIndex |= (ai<<j);
    }


    ///ANSWER

    //read the 10*16 ciphertexts of the radiuses
    LweSample* radiuses [10];
    for(int i = 0;i<numberOfVectors;i++)
        radiuses[i] = new_gate_bootstrapping_ciphertext_array(16, params);   

    //import the 10*16 ciphertexts of the radiuses
    FILE* radiuses_data = fopen("cloud-KNN.data","rb");
    for(int i = 0;i<numberOfVectors;i++)
        for (int j=0; j<16; j++)
            import_gate_bootstrapping_ciphertext_fromFile(radiuses_data, &radiuses[i][j], params);
    fclose(radiuses_data);

    //decrypt and rebuild the 24 answers
    int16_t int_radiuses [10];
    for(int i = 0;i<numberOfVectors;i++){
        int_radiuses[i] = 0;
        for(int j = 0;j<16;j++){
            int ai = bootsSymDecrypt(&radiuses[i][j], key)>0;
            //printf("%d bit n° %d = %d\n", i, j, ai);
            int_radiuses[i] |= (ai<<j);

        }
        //printf("And the result n°%d is: %d\n",i+1,int_answers[i]);
    }

        //read the 1*16 ciphertext of the source radius
    LweSample* sourceRadius = new_gate_bootstrapping_ciphertext_array(16, params);   

    //import the 1*16 ciphertexts of the source radius
    FILE* sourceRadius_data = fopen("sourceRadius-KNN.data","rb");
    for (int j=0; j<16; j++)
        import_gate_bootstrapping_ciphertext_fromFile(sourceRadius_data, &sourceRadius[j], params);
    fclose(sourceRadius_data);

    //decrypt and rebuild the source radius
    int32_t int_sourceRadius = 0;
    for(int j = 0;j<16;j++){
        int ai = bootsSymDecrypt(&sourceRadius[j], key)>0;
        int_sourceRadius |= (ai<<j);
    }


    ///PRINT
    for(int i = 0;i<numberOfVectors;i++)
        printf("N°%d : %d radius is %d\n",i+1,int_indexes[i],int_radiuses[i]);
    printf("Source : %d radius is %d\n",int_sourceIndex,int_sourceRadius);*/

    ///Get model classes
    //read the numberOfVectors*1*8 ciphertexts of model vectors class
    LweSample* modelClassesCiphertexts[10];
    for(int i = 0;i<numberOfVectors;i++)
        modelClassesCiphertexts[i] = new_gate_bootstrapping_ciphertext_array(8, params);   

    //import the numberOfVectors*1*8 ciphertexts of model vectors class
    FILE* modelClasses_data = fopen("model-classes-KNN.data","rb");
    //For each vector
    for(int i = 0;i<numberOfVectors;i++)
        //For each bit
        for (int j=0; j<8; j++)
            import_gate_bootstrapping_ciphertext_fromFile(modelClasses_data, &modelClassesCiphertexts[i][j], params);
    fclose(modelClasses_data);

    //decrypt and rebuild the numberOfVectors*1*8 ciphertexts of model vectors class
    char char_modelClasses [10];
    printf("\nMODEL CLASSES\n");
    //For each bit
    for(int i = 0;i<numberOfVectors;i++){
        char_modelClasses[i] = 0;
        //For each bit
        for(int j = 0;j<8;j++){
            int ai = bootsSymDecrypt(&modelClassesCiphertexts[i][j], key)>0;
            //printf("vector n°%d, bit n°%d = %d\n", i, j, ai);
            char_modelClasses[i] |= (ai<<j);
        }
        printf("And the class n°%d is: %c\n",i, char_modelClasses[i]);
    }

    ///Get sorted classes
    //read the numberOfVectors*1*8 ciphertexts of sorted vectors class
    LweSample* sortedClassesCiphertexts[10];
    for(int i = 0;i<numberOfVectors;i++)
        sortedClassesCiphertexts[i] = new_gate_bootstrapping_ciphertext_array(8, params);   

    //import the numberOfVectors*1*8 ciphertexts of model vectors class
    FILE* sortedClasses_data = fopen("sorted-classes-KNN.data","rb");
    //For each vector
    for(int i = 0;i<numberOfVectors;i++)
        //For each bit
        for (int j=0; j<8; j++)
            import_gate_bootstrapping_ciphertext_fromFile(sortedClasses_data, &sortedClassesCiphertexts[i][j], params);
    fclose(sortedClasses_data);

    //decrypt and rebuild the numberOfVectors*1*8 ciphertexts of model vectors class
    char char_sortedClasses [10];
    printf("\nSORTED CLASSES\n");
    //For each bit
    for(int i = 0;i<numberOfVectors;i++){
        char_sortedClasses[i] = 0;
        //For each bit
        for(int j = 0;j<8;j++){
            int ai = bootsSymDecrypt(&sortedClassesCiphertexts[i][j], key)>0;
            //printf("vector n°%d, bit n°%d = %d\n", i, j, ai);
            char_sortedClasses[i] |= (ai<<j);
        }
        printf("And the class n°%d is: %c\n",i, char_sortedClasses[i]);
    }

    ///Get source class
    //read the verifOnNuber
    LweSample* sourceClassCiphertexts = new_gate_bootstrapping_ciphertext_array(8, params);   

    //import the verifOnNuber
    FILE* sourceClass_data = fopen("source-class-KNN.data","rb");

    //For each bit
    for(int j = 0;j<8;j++)
        import_gate_bootstrapping_ciphertext_fromFile(sourceClass_data, &sourceClassCiphertexts[j], params);
    fclose(sourceClass_data);

    //decrypt and rebuild the verifOnNuber
    char char_sourceClass =  0;
    printf("\nRIGHT SOURCE CLASS\n");
    //For each bit
    for(int j = 0;j<8;j++){
        int ai = bootsSymDecrypt(&sourceClassCiphertexts[j], key)>0;
        char_sourceClass |= (ai<<j);
        //printf("ai %d : %d\n", j, ai);
        //int_verifOnNuber |= (ai<<j);
    }
    printf("The right source class is: %c\n",char_sourceClass);

    ///Get source class
    LweSample* predictedClassCiphertexts = new_gate_bootstrapping_ciphertext_array(8, params);   

    //import the predictedClass
    FILE* predictedClass_data = fopen("predicted-class-KNN.data","rb");

    //For each bit
    for(int j = 0;j<8;j++)
        import_gate_bootstrapping_ciphertext_fromFile(predictedClass_data, &predictedClassCiphertexts[j], params);
    fclose(predictedClass_data);

    //decrypt and rebuild the verifOnNuber
    char char_predictedClass =  0;
    printf("\nPREDICTED SOURCE CLASS\n");
    //For each bit
    for(int j = 0;j<8;j++){
        int ai = bootsSymDecrypt(&predictedClassCiphertexts[j], key)>0;
        char_predictedClass |= (ai<<j);
        //printf("ai %d : %d\n", j, ai);
        //int_verifOnNuber |= (ai<<j);
    }
    printf("And the predicted source class is: %c\n",char_predictedClass);

    /*///TEST model vectors
    LweSample* modelParametersCiphertexts[10][30];
    //For each vector
    for(int i = 0;i<numberOfVectors;i++)
        //for each parameter
        for(int j = 0;j<1;j++)//1 paramètre au lieu de 30
            modelParametersCiphertexts[i][j] = new_gate_bootstrapping_ciphertext_array(16, params);
    printf("TEST 1\n");
    //FILE* modelParameters_data = fopen("answerTEST1_data-KNN.data","rb");
    FILE* modelParameters_data = fopen("model-parameters-KNN.data","rb");
    //For each vector
    for(int v = 0;v<numberOfVectors;v++)
        //For each parameter
        for (int i=0; i<1; i++)//1 paramètre au lieu de 30
            //For each bit
            for (int j=0; j<16; j++)
                import_gate_bootstrapping_ciphertext_fromFile(modelParameters_data, &modelParametersCiphertexts[v][i][j], params);
    fclose(modelParameters_data);
    //decrypt and rebuild
    int int_modelParameters [10][30];
    //For each vector
    for(int v = 0;v<numberOfVectors;v++){
        for(int i = 0;i<1;i++){ //1 sur 30
            int_modelParameters[v][i] = 0;
            for(int j = 0;j<16;j++){
                int ai = bootsSymDecrypt(&modelParametersCiphertexts[v][i][j], key)>0;
                int_modelParameters[v][i] |= (ai<<j);
            }
            //printf("Vector n°%d param %d -> %d\n", v, i, int_modelParameters[i]);
        }
    }
    //verif
    for(int v=0;v<numberOfVectors;v++)
        printf("vecteur %d, param %d : %d\n", v, 0, int_modelParameters[v][0]); //TEST*/

    ///TEST VERIF ON NUMBER
    //read the verifOnNuber
    LweSample* verifOnNumberCiphertexts = new_gate_bootstrapping_ciphertext_array(16, params);   

    //import the verifOnNuber
    FILE* verifOnNuber_data = fopen("verifOnNumber-KNN.data","rb");

    //For each bit
    for(int j = 0;j<16;j++)
        import_gate_bootstrapping_ciphertext_fromFile(verifOnNuber_data, &verifOnNumberCiphertexts[j], params);
    fclose(verifOnNuber_data);

    //decrypt and rebuild the verifOnNuber
    int int_verifOnNuber =  0;
    int bin_verifOnNuber [16];
    printf("\nVERIF ON NUMBER\n");
    //For each bit
    for(int j = 0;j<16;j++){
        int ai = bootsSymDecrypt(&verifOnNumberCiphertexts[j], key)>0;
        bin_verifOnNuber[j] = ai;
        printf("ai %d : %d\n", j, ai);
        //int_verifOnNuber |= (ai<<j);
    }
    int notBit;
        for(int j = 0;j<16-1;j++){
            if(bin_verifOnNuber[15]==0)
                int_verifOnNuber |= (bin_verifOnNuber[j]<<j);
            else{
                notBit = (bin_verifOnNuber[j]==0)?1:0;
                int_verifOnNuber |= (notBit<<j);
            }
        }
        if(bin_verifOnNuber[15]==1)
            int_verifOnNuber = 0-int_verifOnNuber;

    printf("verifOnNumber -> %d\n", int_verifOnNuber);

    ///TEST VERIF ON NUMBER 2
    //read the verifOnNuber
    LweSample* verifOnNumber2Ciphertexts = new_gate_bootstrapping_ciphertext_array(16, params);   

    //import the verifOnNuber
    FILE* verifOnNuber2_data = fopen("verifOnNumber2-KNN.data","rb");

    //For each bit
    for(int j = 0;j<16;j++)
        import_gate_bootstrapping_ciphertext_fromFile(verifOnNuber2_data, &verifOnNumber2Ciphertexts[j], params);
    fclose(verifOnNuber2_data);

    //decrypt and rebuild the verifOnNuber
    int int_verifOnNuber2 =  0;
    int bin_verifOnNuber2 [16];
    printf("\nVERIF ON NUMBER 2\n");
    //For each bit
    for(int j = 0;j<16;j++){
        int ai = bootsSymDecrypt(&verifOnNumber2Ciphertexts[j], key)>0;
        bin_verifOnNuber2[j] = ai;
        printf("ai %d : %d\n", j, ai);
        //int_verifOnNuber |= (ai<<j);
    }
    //int notBit;
        for(int j = 0;j<16-1;j++){
            if(bin_verifOnNuber2[15]==0)
                int_verifOnNuber2 |= (bin_verifOnNuber2[j]<<j);
            else{
                notBit = (bin_verifOnNuber2[j]==0)?1:0;
                int_verifOnNuber2 |= (notBit<<j);
            }
        }
        if(bin_verifOnNuber2[15]==1)
            int_verifOnNuber2 = 0-int_verifOnNuber2;

    printf("verifOnNumber2 -> %d\n", int_verifOnNuber2);

    ///TEST deltalist
    //read the numberOfVectors*1*1 ciphertexts of isInK
    LweSample* deltalistCiphertexts[10];
    for(int i = 0;i<numberOfVectors;i++)
        deltalistCiphertexts[i] = new_gate_bootstrapping_ciphertext_array(16, params);   

    //import the numberOfVectors*1*1 ciphertexts of isInK
    FILE* deltalist_data = fopen("deltalist-KNN.data","rb");
    //For each vector
    for(int i = 0;i<numberOfVectors;i++)
        for(int j = 0;j<16;j++)
            import_gate_bootstrapping_ciphertext_fromFile(deltalist_data, &deltalistCiphertexts[i][j], params);
    fclose(deltalist_data);

    //decrypt and rebuild the numberOfVectors*1*1 ciphertexts of isInK
    int int_deltalist [10];
    int bin_deltalist [16];
    printf("DELTA LIST\n");
    //For each vector
    for(int i = 0;i<numberOfVectors;i++){
        int_deltalist[i] = 0;
        for(int j = 0;j<16;j++){
            int ai = bootsSymDecrypt(&deltalistCiphertexts[i][j], key)>0;
            bin_deltalist[j] = ai;
            //int_deltalist[i] |= (ai<<j);
        }

        int notBit;
        for(int j = 0;j<16-1;j++){
            if(bin_deltalist[15]==0)
                int_deltalist[i] |= (bin_deltalist[j]<<j);
            else{
                notBit = (bin_deltalist[j]==0)?1:0;
                int_deltalist[i] |= (notBit<<j);
            }
        }
        if(bin_deltalist[15]==1)
            int_deltalist[i] = 0-int_deltalist[i];

        //printf("Vector n°%d -> %d\n", i, int_deltalist[i]);
    }

    ///TEST matrix
    //read
    LweSample* matrixCiphertexts[10][10];
    for(int i = 0;i<numberOfVectors;i++)
        for(int j = 0;j<numberOfVectors;j++)
            matrixCiphertexts[i][j] = new_gate_bootstrapping_ciphertext(params);   

    //import the numberOfVectors*1*1 ciphertexts of isInK
    FILE* matrix_data = fopen("matrix-KNN.data","rb");
    //For each line
    for(int i = 0;i<numberOfVectors;i++)
        //For each column
        for(int j = 0;j<numberOfVectors;j++)
            import_gate_bootstrapping_ciphertext_fromFile(matrix_data, matrixCiphertexts[i][j], params);
    fclose(matrix_data);

    //decrypt and rebuild the numberOfVectors*1*1 ciphertexts of isInK
    int int_matrix [10][10];
    //For each line
    for(int i = 0;i<numberOfVectors;i++)
        //For each column
        for(int j = 0;j<numberOfVectors;j++){ //2 sur 10 (essai)
            int_matrix[i][j] = 0;
            int_matrix[i][j] = bootsSymDecrypt(matrixCiphertexts[i][j], key)>0;
            //printf("Vector n°%d -> %d\n", i, int_matrix[i][j]);
        }
    printf("\nMATRIX\n");
    for(int i=0;i<numberOfVectors;i++){
        for(int j=0;j<numberOfVectors;j++);
            //printf("%d ", int_matrix[i][j]); //TEST
        //printf("\n"); //TEST
    }

    ///TEST dSum
    //read the numberOfVectors*1*1 ciphertexts of dSum
    LweSample* dSumCiphertexts[10];
    for(int i = 0;i<numberOfVectors;i++)
        dSumCiphertexts[i] = new_gate_bootstrapping_ciphertext_array(16, params);   

    //import the numberOfVectors*1*1 ciphertexts of isInK
    FILE* dSum_data = fopen("dSum-KNN.data","rb");
    //For each vector
    for(int i = 0;i<numberOfVectors;i++)
        for(int j = 0;j<16;j++)
            import_gate_bootstrapping_ciphertext_fromFile(dSum_data, &deltalistCiphertexts[i][j], params);
    fclose(dSum_data);

    //decrypt and rebuild the numberOfVectors*1*1 ciphertexts of isInK
    int int_dSum [10];
    int bin_dSum [16];
    printf("MATRIX SUMS\n");
    //For each vector
    for(int i = 0;i<numberOfVectors;i++){
        int_dSum[i] = 0;
        for(int j = 0;j<16;j++){
            int ai = bootsSymDecrypt(&deltalistCiphertexts[i][j], key)>0;
            bin_dSum[j] = ai;
        }

        int notBit;
        for(int j = 0;j<16-1;j++){
            if(bin_dSum[15]==0)
                int_dSum[i] |= (bin_dSum[j]<<j);
            else{
                notBit = (bin_dSum[j]==0)?1:0;
                int_dSum[i] |= (notBit<<j);
            }
        }
        if(bin_dSum[15]==1)
            int_dSum[i] = 0-int_dSum[i];

        //printf("Vector n°%d -> %d\n", i, int_dSum[i]);
    }

    ///Get isInK
    //read the numberOfVectors*1*1 ciphertexts of isInK
    LweSample* isInKCiphertexts[10];
    for(int i = 0;i<numberOfVectors;i++) //2 sur 10 (essai)
        isInKCiphertexts[i] = new_gate_bootstrapping_ciphertext(params);   

    //import the numberOfVectors*1*1 ciphertexts of isInK
    FILE* isInK_data = fopen("answer-KNN.data","rb");
    //For each vector
    for(int i = 0;i<numberOfVectors;i++)
        import_gate_bootstrapping_ciphertext_fromFile(isInK_data, isInKCiphertexts[i], params);
    fclose(isInK_data);

    //decrypt and rebuild the numberOfVectors*1*1 ciphertexts of isInK
    int int_isInK [10];
    printf("RESULTS\n");
    //For each vector
    for(int i = 0;i<numberOfVectors;i++){
        int_isInK[i] = 0;
        int_isInK[i] = bootsSymDecrypt(isInKCiphertexts[i], key)>0;
        //printf("Vector n°%d -> %d\n", i, int_isInK[i]);
    }

    ///Predict class
    int countB = 0;
    int countM = 0;
    printf("PREDICT SOURCE CLASS\n");
    for(int i = 0;i<numberOfVectors;i++){
        if(int_isInK[i] == 1){
            if(char_modelClasses[i] == 'B')
                countB ++;
            else
                countM ++;
        }
    }
    char predictedClass = (countB>countM)?'B':'M';
    printf("Predicted source class -> %c\n", predictedClass);

    ///Accuracy
    int TP = 0;
    int TN = 0;
    int FP = 0;
    int FN = 0;
    printf("ACCURACY\n");
    for(int i = 0;i<1;i++){ //number of prediction
        if(predictedClass == 'M'){
            if(char_sourceClass == 'M')
                TP ++;
            else
                FP ++;
        }
        else
            if(char_sourceClass == 'B')
                TN ++;
            else
                FN ++;
    }
    int accuracy = (TP + TN)/(TP + FP + TN + FN);
    printf("Accuracy -> %d%%\n", accuracy*100);
    printf("TP -> %d\n", TP);
    printf("TN -> %d\n", TN);
    printf("FP -> %d\n", FP);
    printf("FN -> %d\n", FN);


    //clean up all pointers
    for(int i = 0;i<numberOfVectors;i++){ //2 sur 10 (essai)
        //delete_gate_bootstrapping_ciphertext_array(32, indexes[i]);
        //delete_gate_bootstrapping_ciphertext_array(16, radiuses[i]);
        delete_gate_bootstrapping_ciphertext(isInKCiphertexts[i]);
        delete_gate_bootstrapping_ciphertext_array(16, deltalistCiphertexts[i]);
        /*for(int j=0;j<1;j++){ //1 sur 30
            delete_gate_bootstrapping_ciphertext_array(16, modelParametersCiphertexts[i][j]);
        }*/
        for(int j=0;j<numberOfVectors;j++){ //1 sur 30
            delete_gate_bootstrapping_ciphertext(matrixCiphertexts[i][j]);
        }
    }
    delete_gate_bootstrapping_ciphertext_array(16, verifOnNumberCiphertexts);
    //delete_gate_bootstrapping_ciphertext_array(32, sourceIndex);
    //delete_gate_bootstrapping_ciphertext_array(16, sourceRadius);
    delete_gate_bootstrapping_secret_keyset(key);

}
