#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {
    //generate a keyset
    const int minimum_lambda = 110;
    TFheGateBootstrappingParameterSet* params = new_default_gate_bootstrapping_parameters(minimum_lambda);

    //generate a random key
    uint32_t seed[] = { 314, 1592, 657 };
    tfhe_random_generator_setSeed(seed,3);
    TFheGateBootstrappingSecretKeySet* key = new_random_gate_bootstrapping_secret_keyset(params);

    //declaration of interesting indexes
    int16_t numberOfVectors = 10;
    //int32_t modelIndexes [10] = {864018, 857010, 855563, 848406, 857156, 862717, 8510653, 86208, 861598, 852781}; //SET N°1 DEFAULT
    int32_t modelIndexes [10] = {84501001, 849014, 8510426, 852781, 89869, 899187, 9113156, 915460, 915143, 925311}; //SET N°2
    int32_t sourceIndex = /*842302*/857374;
    char existingClasses [2] = {'B', 'M'};
    char modelClasses [10];
    char sourceClass;
    int16_t modelParameters [10][32];
    int16_t sourceParameters [32];
    float f_modelParameters [10][32];
    float f_sourceParameters [32];
    /*int16_t radiuses [10];
    int16_t sourceRadius;
    int16_t textures [10];
    int16_t sourceTexture;
    int16_t perimeters [10];
    int16_t sourcePerimeter;
    int16_t areas [10];
    int16_t sourceArea;
    int16_t smoothnesses [10];
    int16_t sourceSmoothness;
    int16_t compactnesses [10];
    int16_t sourceCompactness;
    int16_t concavities [10];
    int16_t sourceConcavity;
    int16_t concavePoints [10];
    int16_t sourceConcavePoints;
    int16_t symetries [10];
    int16_t sourceSymetry;
    int16_t fractalDimension [10];
    int16_t sourceFractalDimension;
    float f_fradiuses [10];
    float f_sourceRadius;
    float f_minRadius;
    float f_maxRadius;
    float f_textures [10];
    float f_sourceTexture;
    float f_perimeters [10];
    float f_sourcePerimeter;
    float f_areas [10];
    float f_sourceArea;
    float f_smoothnesses [10];
    float f_sourceSmoothness;
    float f_compactnesses [10];
    float f_sourceCompactness;
    float f_concavities [10];
    float f_sourceConcavity;
    float f_concavePoints [10];
    float f_sourceConcavePoints;
    float f_symetries [10];
    float f_sourceSymetry;
    float f_fractalDimension [10];
    float f_sourceFractalDimension;*/

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
                nb_val_lues = sscanf( line, "%d,%c,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f"/*,%f,%f*/,
                 &tempIndex, &tempClass, &tempParameters[0], &tempParameters[1], &tempParameters[2], &tempParameters[3], &tempParameters[4],
                  &tempParameters[5], &tempParameters[6], &tempParameters[7], &tempParameters[8], &tempParameters[9], &tempParameters[10],
                   &tempParameters[11], &tempParameters[12], &tempParameters[13], &tempParameters[14], &tempParameters[15], &tempParameters[16],
                    &tempParameters[17], &tempParameters[18], &tempParameters[19], &tempParameters[20], &tempParameters[21], &tempParameters[22],
                     &tempParameters[23], &tempParameters[24], &tempParameters[25], &tempParameters[26],&tempParameters[27], &tempParameters[28],
                      &tempParameters[29]/*, &tempParameters[30], &tempParameters[31]*/);
                //printf("Index : %d ; Class : %c ; %f , %f , %f\n", tempIndex, tempClass, tempParameters[0], tempParameters[1], tempParameters[2]); //TEST
                //check if this is the good line
                if(i<numberOfVectors)
                    if(tempIndex == modelIndexes[i]){
                        //register the values
                        modelClasses[i] = tempClass;
                        for(int j = 0;j<30;j++)
                            f_modelParameters[i][j] = tempParameters[j];
                        //printf("\nN°%d\n", i); //TEST
                        //printf("temp: %d\nclass: %c\nradius: %d\nnb_val_lues: %d\n", temp1, temp2[i], radiuses[i], nb_val_lues); //TEST
                    }
                else
                    if(tempIndex == sourceIndex){
                        //register the values
                        sourceClass = tempClass;
                        for(int j = 0;j<30;j++)
                            f_sourceParameters[j] = tempParameters[j];
                        //printf("\nN°%d\n", i); //TEST
                        //printf("temp: %d\nclass: %c\nradius: %d\nnb_val_lues: %d\n", temp1, temp2[i], radiuses[i], nb_val_lues); //TEST
                    }
            }
            rewind(dataset_data);
        }
    }
    fclose(dataset_data);


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
    for(int v=0;v<numberOfVectors;v++)
        printf("vecteur %d, param %d : %d\n", v, 0, modelParameters[v][0]); //TEST
    

    //-|-|-Encrypt vectors-|-|-
    //encrypt the source index on 1*1*32 ciphertexts (32 bits)
    LweSample* sourceIndexCiphertexts;
    //printf("\nSource index : %d\n", sourceIndex); //TEST
    sourceIndexCiphertexts = new_gate_bootstrapping_ciphertext_array(32, params);
    //For each bit
    for (int j=0; j<32; j++)
        bootsSymEncrypt(&sourceIndexCiphertexts[j], (sourceIndex>>j)&1, key);
    
    //encrypt the source vector class on 1*1*8 ciphertexts (1 vector, 8 bits)
    LweSample* sourceClassCiphertexts = new_gate_bootstrapping_ciphertext_array(8, params);
    //For each bit
    for (int j=0; j<8; j++)
        bootsSymEncrypt(&sourceClassCiphertexts[j], (sourceClass>>j)&1, key);

    //encrypt the source vectors on 1*30*16 ciphertexts (30 params, 16 bits)
    LweSample* sourceParametersCiphertexts[30];
    //For each parameter
    for (int i=0; i<30; i++){
        sourceParametersCiphertexts[i] = new_gate_bootstrapping_ciphertext_array(16, params);
        //For each bit
        for (int j=0; j<16; j++)
            bootsSymEncrypt(&sourceParametersCiphertexts[i][j], (sourceParameters[i]>>j)&1, key);
    }

    //encrypt the model vectors index on numberOfVectors*1*32 ciphertexts (numberOfVectors vectors, 32 bits)
    LweSample* modelIndexesCiphertexts[10];
    //printf("\nSource index : %d\n", sourceIndex); //TEST
    //For each vector
    for (int v=0; v<numberOfVectors; v++){
        modelIndexesCiphertexts[v] = new_gate_bootstrapping_ciphertext_array(32, params);
        //For each bit
        for (int j=0; j<32; j++)
            bootsSymEncrypt(&modelIndexesCiphertexts[v][j], (modelIndexes[v]>>j)&1, key);
    }

    //encrypt the model vectors class on numberOfVectors*1*8 ciphertexts (numberOfVectors vectors, 8 bits)
    LweSample* modelClassesCiphertexts[10];
    //printf("\nSource index : %d\n", sourceIndex); //TEST
    //For each vector
    for (int v=0; v<numberOfVectors; v++){
        modelClassesCiphertexts[v] = new_gate_bootstrapping_ciphertext_array(8, params);
        //printf("\nClass of vector %d is %d\n", v, (short)modelClasses[v]);
        //For each bit
        for (int j=0; j<8; j++)
            bootsSymEncrypt(&modelClassesCiphertexts[v][j], (modelClasses[v]>>j)&1, key);
    }

    //encrypt the model vectors class on numberOfVectors*1*8 ciphertexts (numberOfVectors vectors, 8 bits)
    LweSample* existingClassesCiphertexts[10];
    //printf("\nSource index : %d\n", sourceIndex); //TEST
    //For each vector
    for (int i=0; i<2; i++){
        existingClassesCiphertexts[i] = new_gate_bootstrapping_ciphertext_array(8, params);
        //printf("\nClass of vector %d is %d\n", v, (short)modelClasses[v]);
        //For each bit
        for (int j=0; j<8; j++)
            bootsSymEncrypt(&existingClassesCiphertexts[i][j], (existingClasses[i]>>j)&1, key);
    }

    //encrypt the model vectors on numberOfVectors*30*16 ciphertexts (numberOfVectors vectors, 30 params, 16 bits)
    LweSample* modelParametersCiphertexts[10][30];
    //For each vector
    for (int v=0; v<numberOfVectors; v++){
        //For each parameter
        for (int i=0; i<1; i++){ // 1 sur 30 (essai)
            modelParametersCiphertexts[v][i] = new_gate_bootstrapping_ciphertext_array(16, params);
            //For each bit
            for (int j=0; j<16; j++)
                bootsSymEncrypt(&modelParametersCiphertexts[v][i][j], (modelParameters[v][i]>>j)&1, key);
        }
    }

    //-|-|-Export keys and vectors-|-|-
    //Export order : 1-secret key, 2-cloud key, 3-source index, 4-source class, 5-source parameters,
                    // 6-model indexes, 7-model classes, 8-existing classes, 9-model parameters
    //1-Export the secret key to file for later use
    FILE* secret_key = fopen("secret-KNN.key","wb");
    export_tfheGateBootstrappingSecretKeySet_toFile(secret_key, key);
    fclose(secret_key);
    
    //2-Export the cloud key to a file (for the cloud)
    FILE* cloud_key = fopen("cloud-KNN.key","wb");
    export_tfheGateBootstrappingCloudKeySet_toFile(cloud_key, &key->cloud);
    fclose(cloud_key);

    //3-Export the 1*1*32 source index ciphertexts to a file
    FILE* sourceIndex_data = fopen("source-index-KNN.data","wb");
    //For each bit
    for (int j=0; j<32; j++)
        export_gate_bootstrapping_ciphertext_toFile(sourceIndex_data, &sourceIndexCiphertexts[j], params);
    fclose(sourceIndex_data);

    //4-Export the 10*1*8 source class ciphertexts to a file
    FILE* sourceClass_data = fopen("source-class-KNN.data","wb");
    //For each bit
    for (int j=0; j<8; j++)
        export_gate_bootstrapping_ciphertext_toFile(sourceClass_data, &sourceClassCiphertexts[j], params);
    fclose(sourceClass_data);

    //5-Export the 1*30*16 source parameters ciphertexts to a file (for the cloud)
    FILE* sourceParameters_data = fopen("source-parameters-KNN.data","wb");
    //For each parameter
    for(int i = 0;i<30;i++)
        //For each bit
        for (int j=0; j<16; j++)
            export_gate_bootstrapping_ciphertext_toFile(sourceParameters_data, &sourceParametersCiphertexts[i][j], params);
    fclose(sourceParameters_data);

    //6-Export the 10*1*32 model indexes ciphertexts to a file
    FILE* modelIndexes_data = fopen("model-indexes-KNN.data","wb");
    //For each vector
    for(int v = 0;v<numberOfVectors;v++)
        //For each bit
        for (int j=0; j<32; j++)
            export_gate_bootstrapping_ciphertext_toFile(modelIndexes_data, &modelIndexesCiphertexts[v][j], params);
    fclose(modelIndexes_data);

    //7-Export the 10*1*8 model classes ciphertexts to a file
    FILE* modelClasses_data = fopen("model-classes-KNN.data","wb");
    //For each vector
    for(int v = 0;v<numberOfVectors;v++)
        //For each bit
        for (int j=0; j<8; j++)
            export_gate_bootstrapping_ciphertext_toFile(modelClasses_data, &modelClassesCiphertexts[v][j], params);
    fclose(modelClasses_data);

    //8-Export the 2*8 existing classes ciphertexts to a file
    FILE* existingClasses_data = fopen("existing-classes-KNN.data","wb");
    //For each vector
    for(int i = 0;i<2;i++)
        //For each bit
        for (int j=0; j<8; j++)
            export_gate_bootstrapping_ciphertext_toFile(existingClasses_data, &existingClassesCiphertexts[i][j], params);
    fclose(existingClasses_data);

    //9-Export the 10*30*16 model parameters ciphertexts to a file
    FILE* modelParameters_data = fopen("model-parameters-KNN.data","wb");
    //For each vector
    for(int v = 0;v<numberOfVectors;v++)
        //For each parameter
        for (int i=0; i<1; i++) //1 sur 30 (essai)
            //For each bit
            for (int j=0; j<16; j++)
                export_gate_bootstrapping_ciphertext_toFile(modelParameters_data, &modelParametersCiphertexts[v][i][j], params);
    fclose(modelParameters_data);
    
    
    ///-|-|-Clean up all pointers-|-|-
    //1-Clean model vectors
    //For each vector
    for(int i = 0;i<numberOfVectors;i++){
        delete_gate_bootstrapping_ciphertext_array(32, modelIndexesCiphertexts[i]);
        //For each parameter
        for(int j = 0;j<1;j++){ // 1 sur 30
            delete_gate_bootstrapping_ciphertext_array(16, modelParametersCiphertexts[i][j]);
        }
    }
    //2-Clean source vector
    delete_gate_bootstrapping_ciphertext_array(32, sourceIndexCiphertexts);
    //For each parameter
        for(int j = 0;j<30;j++){
            delete_gate_bootstrapping_ciphertext_array(16, sourceParametersCiphertexts[j]);
        }
    //3-Clean keys
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

}
