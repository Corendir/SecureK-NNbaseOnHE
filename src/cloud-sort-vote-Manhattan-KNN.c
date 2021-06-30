#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>
#include <time.h>

void sign(LweSample *output, LweSample *input, const int nb_bits, TFheGateBootstrappingCloudKeySet *bk) {
    //time_t start_time = clock(); //TIME
    LweSample *sign = new_gate_bootstrapping_ciphertext(bk->params);
    //printf("SIGN 1\n");
    bootsCOPY(sign, &input[nb_bits-1], bk); //sign = 0 if positive , = 1 if negative
    // Return 11111111 if sign is false, 00000001 if sign is true
    //printf("SIGN 2\n");
    bootsCOPY(&output[0], sign, bk);
    //printf("SIGN 3\n");
    for(int i = 1; i < nb_bits/*2*/; i++){
        //printf("SIGN 4 %d\n", i);
        bootsCONSTANT(&output[i], 0, bk);
    }

    //time_t end_time = clock(); //TIME
    //printf("sign function time : %ld\n", end_time-start_time); //TIME
}

void signBit(LweSample *output, LweSample *input, const int nb_bits, TFheGateBootstrappingCloudKeySet *bk) {
    //time_t start_time = clock(); //TIME
    bootsCOPY(output, &input[nb_bits-1], bk); //sign = 0 if positive , = 1 if negative
    // Return 1 if sign is false, 0 if sign is true

    //time_t end_time = clock(); //TIME
    //printf("sign function time : %ld\n", end_time-start_time); //TIME
}

void copy(LweSample* a, const LweSample* b,const int nb_bit, const TFheGateBootstrappingCloudKeySet* bk){
    //time_t start_time = clock(); //TIME
    for(int i=0;i<nb_bit;i++){
        bootsCOPY(&a[i],&b[i],bk);
    }
    //time_t end_time = clock(); //TIME
    //printf("vatsaltailor multiplexer time : %ld\n", end_time-start_time); //TIME
}

void compare_bit(LweSample* result, const LweSample* a, const LweSample* b, const LweSample* lsb_carry, LweSample* tmp, const TFheGateBootstrappingCloudKeySet* bk) {
    bootsXNOR(tmp, a, b, bk);
    bootsMUX(result, tmp, lsb_carry, a, bk);
}

// compares two multibit words, return 1 if a<=b, 0 otherwise
void isInfEqual(LweSample* result, const LweSample* a, const LweSample* b, const int nb_bits, const TFheGateBootstrappingCloudKeySet* bk) {
    LweSample* tmps = new_gate_bootstrapping_ciphertext_array(2, bk->params);
    
    //initialize the carry to 0
    bootsCONSTANT(&tmps[0], 0, bk);
    //run the elementary comparator gate n times
    for (int i=0; i<nb_bits; i++) {
        compare_bit(&tmps[0], &a[i], &b[i], &tmps[0], &tmps[1], bk);
    }

    bootsNOT(result, &tmps[0], bk);

    delete_gate_bootstrapping_ciphertext_array(2, tmps);    
}

// compares two multibit words, return 1 if a==b, 0 otherwise
void isEqual(LweSample* result, const LweSample* a, const LweSample* b, const int nb_bits, const TFheGateBootstrappingCloudKeySet* bk) {
    LweSample* tmps = new_gate_bootstrapping_ciphertext_array(2, bk->params);
    
    //initialize the carry to 0
    bootsCONSTANT(&tmps[0], 0, bk);
    //run the elementary comparator gate n times
    for (int i=0; i<nb_bits; i++) {
        bootsXOR(&tmps[1], &a[i], &b[i], bk);
        bootsOR(&tmps[0], &tmps[0], &tmps[1], bk);
    }

    bootsNOT(result, &tmps[0], bk);

    delete_gate_bootstrapping_ciphertext_array(2, tmps);    
}

void vatsaltailorcompare_bit(LweSample* result, const LweSample* a, const LweSample* b, LweSample* lsb_carry, LweSample* tmp, const TFheGateBootstrappingCloudKeySet* bk) {
    LweSample* temp1=new_gate_bootstrapping_ciphertext_array(1, bk->params);
    LweSample* temp2=new_gate_bootstrapping_ciphertext_array(1, bk->params);
    LweSample* temp3=new_gate_bootstrapping_ciphertext_array(1,bk->params);
    LweSample* temp4=new_gate_bootstrapping_ciphertext_array(1,bk->params);
    LweSample* temp5=new_gate_bootstrapping_ciphertext_array(1,bk->params);

    bootsXOR(temp1, a, b, bk);  //a xorb
    bootsXOR(result,temp1,lsb_carry,bk);  //a xor b xor ci
    
    bootsNOT(temp4,a,bk);  // complement of a
    bootsAND(temp3,temp4,b,bk); // complement a and b

    bootsNOT(temp5,temp1,bk);  // complement of a XOR b

    bootsAND(temp2,temp5,lsb_carry,bk);// complement of a XOR b AND lasb_carry
  
    bootsOR(tmp,temp2,temp3,bk);       // a&b + ci*(a xor b)
    bootsCOPY(lsb_carry,tmp,bk);
}

void vatsaltailorsubtract(LweSample* result, const LweSample* a, const LweSample* b, const int nb_bits, const TFheGateBootstrappingCloudKeySet* bk) {
    //time_t start_time = clock(); //TIME
    LweSample* tmps=new_gate_bootstrapping_ciphertext_array(2, bk->params);
    bootsCONSTANT(&tmps[0], 0, bk);
    bootsCONSTANT(&tmps[1], 0, bk);
    //run the elementary comparator gate n times//    
    for (int i=0; i<nb_bits; i++){
        vatsaltailorcompare_bit(&result[i], &a[i], &b[i], &tmps[0], &tmps[1], bk);
    }
    //time_t end_time = clock(); //TIME
    //printf("vatsaltailor subtractor time : %ld\n", end_time-start_time); //TIME
}

void vatsaltailorBitAddition(LweSample* top1, const LweSample* a6, const LweSample* b6, LweSample* lsb_carry1, LweSample* tmp6, const  TFheGateBootstrappingCloudKeySet* bk) {
    //time_t start_time = clock(); //TIME
    LweSample* temp1=new_gate_bootstrapping_ciphertext_array(1, bk->params);
    LweSample* temp2=new_gate_bootstrapping_ciphertext_array(1, bk->params);
    LweSample* temp3=new_gate_bootstrapping_ciphertext_array(1,bk->params);
    
    bootsXOR(temp1, a6, b6, bk);  //a xor b  
    bootsXOR(top1,temp1,lsb_carry1,bk);  //a xor b xor ci
    bootsAND(temp2,temp1,lsb_carry1,bk);   //ci and (a xor b)
    bootsAND(temp3,a6,b6,bk);             // a and b 
    bootsOR(tmp6,temp2,temp3,bk);       // a&b + ci*(a xor b)
    bootsCOPY(lsb_carry1,tmp6,bk);

    //time_t end_time = clock(); //TIME
    //printf("vatsaltailor bitAddition time : %ld\n", end_time-start_time); //TIME
}

void vatsaltailorAdder(LweSample* top1, const LweSample* a6, const LweSample* b6, const int nb_bits, const TFheGateBootstrappingCloudKeySet* bk){
    //time_t start_time = clock(); //TIME
    LweSample* tmps6 = new_gate_bootstrapping_ciphertext_array(2, bk->params);
    bootsCONSTANT(&tmps6[0], 0, bk); //initialize carry to 0

    //run the elementary comparator gate n times//
        
    for (int i=0; i<nb_bits; i++){
        vatsaltailorBitAddition(&top1[i], &a6[i], &b6[i], &tmps6[0], &tmps6[1], bk);
    }
    delete_gate_bootstrapping_ciphertext_array(2, tmps6);
    //time_t end_time = clock(); //TIME
    //printf("vatsaltailor adder time : %ld\n", end_time-start_time); //TIME
}

void adderManyAndOne(LweSample* top1, const LweSample* a, const LweSample* b, const int nb_bits, const TFheGateBootstrappingCloudKeySet* bk){
    //time_t start_time = clock(); //TIME
    LweSample* a6 = new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);
    LweSample* b6 = new_gate_bootstrapping_ciphertext(bk->params);
    copy(a6, a, nb_bits, bk);
    bootsCOPY(b6, b, bk);
    LweSample* tmps6 = new_gate_bootstrapping_ciphertext_array(2, bk->params);
    bootsCONSTANT(&tmps6[0], 0, bk); //initialize carry to 0
    LweSample* zero = new_gate_bootstrapping_ciphertext(bk->params);
    bootsCONSTANT(zero, 0, bk); //initialize zero to 0

    //run the elementary comparator gate n times//
        
    for (int i=0; i<nb_bits; i++){
        if(i==0)
            vatsaltailorBitAddition(&top1[i], &a6[i], &b6[0], &tmps6[0], &tmps6[1], bk);
        else
            vatsaltailorBitAddition(&top1[i], &a6[i], zero, &tmps6[0], &tmps6[1], bk);
    }
    delete_gate_bootstrapping_ciphertext_array(2, tmps6);
    delete_gate_bootstrapping_ciphertext_array(nb_bits, a6);
    delete_gate_bootstrapping_ciphertext(b6);
    //time_t end_time = clock(); //TIME
    //printf("vatsaltailor adder time : %ld\n", end_time-start_time); //TIME
}

void reverse(LweSample *output, LweSample *input, const int nb_bits, TFheGateBootstrappingCloudKeySet *bk) {
    //time_t start_time = clock(); //TIME
    /*copy(output, input, nb_bits, bk);
    bootsCONSTANT(&output[nb_bits-1], 0, bk);*/

    //complément à 1 de b//
    LweSample* complemented1B=new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);
    LweSample* complement=new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params); //the 1 we will add for complement
    LweSample* complementBit=new_gate_bootstrapping_ciphertext(bk->params);
    bootsCONSTANT(complementBit, 1, bk);
    bootsCONSTANT(&complement[0], 1, bk);
    for (int i=0; i<nb_bits; i++){
        bootsNOT(&complemented1B[i], &input[i], bk);
        if(i!=0){
            bootsCONSTANT(&complement[i], 0, bk);
        }
    }

    /*//complément à 2 de input//
    vatsaltailorAdder(output, complemented1B, complement, nb_bits, bk);*/
    adderManyAndOne(output, complemented1B, complement, nb_bits, bk);
    //copy(output, complemented1B, nb_bits, bk);

    //time_t end_time = clock(); //TIME
    //printf("positive function time : %ld\n", end_time-start_time); //TIME
}

void mysubtract(LweSample* result, const LweSample* a, const LweSample* b, const int nb_bits, const TFheGateBootstrappingCloudKeySet* bk) {
    //time_t start_time = clock(); //TIME
    //complément à 1 de b//
    LweSample* complemented1B=new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);
    LweSample* complemented2B=new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);
    LweSample* complement=new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params); //the 1 we will add for complement
    bootsCONSTANT(&complement[0], 1, bk);
    for (int i=0; i<nb_bits; i++){
        bootsNOT(&complemented1B[i], &b[i], bk);
        if(i!=0)
            bootsCONSTANT(&complement[i], 0, bk);
    }

    //complément à 2 de b//
    vatsaltailorAdder(complemented2B, complemented1B, complement, nb_bits, bk);

    //addition//
    vatsaltailorAdder(result, a, complemented2B, nb_bits, bk);

    LweSample* temp1=new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);

    //time_t end_time = clock(); //TIME
    //printf("my subtractor time : %ld\n", end_time-start_time); //TIME
}

//select_line?b:a
void vatsaltailorMultiplexer(LweSample* rdbdata,LweSample* a,LweSample* b,LweSample* select_line,const int nb_bit, const TFheGateBootstrappingCloudKeySet* bk){
    //time_t start_time = clock(); //TIME
    int m=0;
    for(int i=0;i<nb_bit;i++){
        bootsMUX(&rdbdata[i],&select_line[m],&b[i],&a[i],bk);
    }
    //time_t end_time = clock(); //TIME
    //printf("vatsaltailor multiplexer time : %ld\n", end_time-start_time); //TIME
}

void vatsaltailorMultiply(LweSample* product, LweSample* a, LweSample* b, const int nb_bits, const TFheGateBootstrappingCloudKeySet* bk){
    //time_t start_time = clock(); //TIME
        
    LweSample* enc_theta=new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);
    for(int i=0;i<nb_bits;i++){ //initialize theta to all zero bits
        bootsCONSTANT(&enc_theta[i],0,bk);
    }
    for(int i=0;i<2*nb_bits;i++){ //initialize product to all zero bits
        bootsCONSTANT(&product[i],0,bk);
    } 

    for (int i=0; i<nb_bits; i++) {
        LweSample* temp_result=new_gate_bootstrapping_ciphertext_array(2 * nb_bits, bk->params);
        LweSample* partial_sum=new_gate_bootstrapping_ciphertext_array(2 * nb_bits, bk->params);
        for(int j=0;j<2*nb_bits;j++){ //initialize temp_result to all zero bits
            bootsCONSTANT(&temp_result[j],0,bk);
            bootsCONSTANT(&partial_sum[j],0,bk);
        } 
        LweSample* temp2=new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);
        vatsaltailorMultiplexer(temp2,enc_theta,a,&b[i],nb_bits,bk);
        for(int j=0;j<nb_bits;j++){ 
            bootsCOPY(&temp_result[i+j],&temp2[j],bk);
        }

        //Add the valid result to partial_sum//
        vatsaltailorAdder(partial_sum,product,temp_result,2*nb_bits,bk);
        //Change the partial sum to final product//
        for(int j=0;j<2*nb_bits;j++){ 
            bootsCOPY(&product[j],&partial_sum[j],bk);
        }
    }
    //time_t end_time = clock(); //TIME
    //printf("vatsaltailor multiplier time : %ld\n", end_time-start_time); //TIME
}

void vatsaltailorScalarMultiply(LweSample* product, LweSample* a, unsigned short* u, const int nb_bits, const TFheGateBootstrappingCloudKeySet* bk){
    //time_t start_time = clock(); //TIME
        
    LweSample* enc_theta=new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);
    for(int i=0;i<nb_bits;i++){ //initialize theta to all zero bits
        bootsCONSTANT(&enc_theta[i],0,bk);
    }
    for(int i=0;i<2*nb_bits;i++){ //initialize product to all zero bits
        bootsCONSTANT(&product[i],0,bk);
    } 

    for (int i=0; i<nb_bits; i++) {
        LweSample* temp_result=new_gate_bootstrapping_ciphertext_array(2 * nb_bits, bk->params);
        LweSample* partial_sum=new_gate_bootstrapping_ciphertext_array(2 * nb_bits, bk->params);
        for(int j=0;j<2*nb_bits;j++){ //initialize temp_result to all zero bits
            bootsCONSTANT(&temp_result[j],0,bk);
            bootsCONSTANT(&partial_sum[j],0,bk);
        } 
        LweSample* temp2=new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);
        //vatsaltailorMultiplexer(temp2,enc_theta,a,&b[i],nb_bits,bk);
        if(u[i] == 1)
            copy(temp2, a, nb_bits, bk);
        else
            copy(temp2, enc_theta, nb_bits, bk);

        for(int j=0;j<nb_bits;j++){ 
            bootsCOPY(&temp_result[i+j],&temp2[j],bk);
        }

        //Add the valid result to partial_sum//
        vatsaltailorAdder(partial_sum,product,temp_result,2*nb_bits,bk);
        //Change the partial sum to final product//
        for(int j=0;j<2*nb_bits;j++){ 
            bootsCOPY(&product[j],&partial_sum[j],bk);
        }
    }
    //time_t end_time = clock(); //TIME
    //printf("vatsaltailor scalar multiplier time : %ld\n", end_time-start_time); //TIME
}

void addition_bit(LweSample* result, LweSample* carry, const LweSample* a, const LweSample* b, LweSample* tmps, const TFheGateBootstrappingCloudKeySet* bk) {
    //time_t start_time = clock(); //TIME
    //Calculate temp result
    bootsXOR(tmps, a, b, bk);
    bootsXOR(result, &tmps[0], carry, bk);

    //Calculate carry
    bootsAND(&tmps[1], &tmps[0], carry, bk);
    bootsAND(&tmps[2], a, b, bk);
    bootsOR(carry, &tmps[1], &tmps[2], bk);

    //time_t end_time = clock(); //TIME
    //printf("Bit addition time : %ld\n", end_time-start_time); //TIME
}

void addition(LweSample* result, const LweSample* a, const LweSample* b, const int nb_bits, const TFheGateBootstrappingCloudKeySet* bk) {
    //time_t start_time = clock(); //TIME
    LweSample* tmps = new_gate_bootstrapping_ciphertext_array(3, bk->params);
    LweSample* carry = new_gate_bootstrapping_ciphertext_array(1, bk->params);
    LweSample* tempRes = new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);

    //initialize the carry to 0
    bootsCONSTANT(&carry[0], 0, bk);

    //Calculate the result
    for (int i=0; i<nb_bits; i++) {
        addition_bit(&tempRes[i], carry, &a[i], &b[i], tmps, bk);
    }
    //Give the result
    for (int i=0; i<nb_bits; i++) {
        bootsAND(&result[i],&tempRes[i],&tempRes[i],bk);
    }
    delete_gate_bootstrapping_ciphertext_array(3, tmps);
    delete_gate_bootstrapping_ciphertext_array(1, carry);
    delete_gate_bootstrapping_ciphertext_array(nb_bits, tempRes);
    //Return result, ret
    //time_t end_time = clock(); //TIME
    //printf("Adder time : %ld\n", end_time-start_time); //TIME
}

// Multiply a ciphertext by a scalar
void scalarMultiplication(LweSample* result, LweSample* a, unsigned short* u, const int nb_bits, const TFheGateBootstrappingCloudKeySet* bk) {
    //time_t start_time = clock(); //TIME
    //LweSample* temp = new_gate_bootstrapping_ciphertext_array(32, bk->params);
    //LweSample* s = new_gate_bootstrapping_ciphertext_array(1, bk->params);

    //initialize the result to 0
    /*for (int i=0; i<nb_bits; i++)
        bootsCONSTANT(&result[i], 0, bk);*/

    //run the n additions
    for (int i=0 ; i < nb_bits/2 ; i++) {
        //printf("TEST 2\n");
        //printf("boucle avant if\n");
        if(u[i]==1){
            //printf("TEST 3\n");
            //printf("boucle avec if\n");
            addition (result, result, a, nb_bits, bk);  //temp = result + A
            //printf("TEST 4\n");
		    //*result = *temp ; //PROBLEME SUREMENT ICI
        }
        //printf("boucle après if\n");
        //printf("TEST 5\n");
        addition (a, a, a, nb_bits, bk); //A = A + A pour décalage 1 position vers la gauche
        //printf("TEST 6\n");
        //*a = *s;
        //printf("2e addition faite\n");
        printf("---multiplication done on bit %d\n", i);
    }
    //delete_gate_bootstrapping_ciphertext_array(32, temp);
    //delete_gate_bootstrapping_ciphertext_array(1, s);
    //time_t end_time = clock(); //TIME
    //printf("scalar multiplication time : %ld\n", end_time-start_time); //TIME
}

void junqueraCompare_bit(LweSample* result, LweSample* a, LweSample* b, LweSample* lsb_carry, LweSample* tmp, TFheGateBootstrappingCloudKeySet* bk) {
  bootsXNOR(tmp, a, b, bk);
  bootsMUX(result, tmp, lsb_carry, a, bk);
}

void junqueraU_shiftl(LweSample* result, LweSample* a, const int posiciones, const int nb_bits, TFheGateBootstrappingCloudKeySet* bk){
  LweSample* aux = new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);

  for(int i = 0; i < nb_bits; i++){
    bootsCOPY(&result[i], &a[i], bk);
  }

  for(int i = 0; i < posiciones; i++){
    for(int j = 1; j < nb_bits; j++)
      bootsCOPY(&aux[j], &result[j-1], bk);

    bootsCONSTANT(&aux[0], 0, bk);

    for(int j = 0; j < nb_bits; j++)
      bootsCOPY(&result[j], &aux[j], bk);
  }

  delete_gate_bootstrapping_ciphertext_array(nb_bits, aux);

}

// unsigned shiftr
void junqueraU_shiftr(LweSample* result, const LweSample* a, const int posiciones, const int nb_bits, const TFheGateBootstrappingCloudKeySet* bk){
  LweSample* aux = new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);

  for(int i = 0; i < nb_bits; i++){
    bootsCOPY(&result[i], &a[i], bk);
  }

  for(int i = 0; i < posiciones; i++){

    for(int j = 0; j < nb_bits - 1; j++)
      bootsCOPY(&aux[j], &result[j+1], bk);

    bootsCONSTANT(&aux[nb_bits - 1], 0, bk);

    for(int j = 0; j < nb_bits; j++)
      bootsCOPY(&result[j], &aux[j], bk);
  }
  delete_gate_bootstrapping_ciphertext_array(nb_bits, aux);

}

void junqueraNegativo(LweSample* result, LweSample* a, const int nb_bits, TFheGateBootstrappingCloudKeySet* bk){

  LweSample* ha_cambiado = new_gate_bootstrapping_ciphertext_array(2, bk->params);
  LweSample* not_x = new_gate_bootstrapping_ciphertext_array(2, bk->params);

  for(int i = 0; i < 2; i++){
    bootsCONSTANT(&ha_cambiado[i], 0, bk);
    bootsCONSTANT(&not_x[i], 0, bk);
  }

  for(int i = 0; i < nb_bits; i++){
    bootsNOT(&not_x[0], &a[i], bk);
    bootsMUX(&result[i], &ha_cambiado[0], &not_x[0], &a[i], bk);
    bootsOR(&ha_cambiado[0], &ha_cambiado[0], &a[i], bk);
  }

  delete_gate_bootstrapping_ciphertext_array(2, ha_cambiado);
  delete_gate_bootstrapping_ciphertext_array(2, not_x);
}

void junqueraAdd_bit(LweSample* result, LweSample* carry_out, const LweSample* a, const LweSample* b, const LweSample* carry_in, const TFheGateBootstrappingCloudKeySet* bk){

  LweSample* s1 = new_gate_bootstrapping_ciphertext_array(2, bk->params);
  LweSample* c1 = new_gate_bootstrapping_ciphertext_array(2, bk->params);
  LweSample* c2 = new_gate_bootstrapping_ciphertext_array(2, bk->params);

  bootsCONSTANT(&s1[0], 0, bk);
  bootsCONSTANT(&c1[0], 0, bk);
  bootsCONSTANT(&c2[0], 0, bk);

  bootsXOR(s1, a, b, bk);
  bootsXOR(result, s1, carry_in, bk);

  bootsAND(c1, s1, carry_in, bk);
  bootsAND(c2, a, b, bk);

  bootsOR(carry_out, c1, c2, bk);

  delete_gate_bootstrapping_ciphertext_array(2, s1);
  delete_gate_bootstrapping_ciphertext_array(2, c1);
  delete_gate_bootstrapping_ciphertext_array(2, c2);

}

void junqueraAdd(LweSample* result, const LweSample* a, const LweSample* b, const int nb_bits, const TFheGateBootstrappingCloudKeySet* bk) {
  LweSample* tmps_carry = new_gate_bootstrapping_ciphertext_array(2, bk->params);

  //initialize the carry to 0
  bootsCONSTANT(&tmps_carry[0], 0, bk);

  //run the elementary comparator gate n times
  for (int i=0; i<nb_bits; i++) {
      junqueraAdd_bit(&result[i], &tmps_carry[0], &a[i], &b[i], &tmps_carry[0], bk);
  }

  delete_gate_bootstrapping_ciphertext_array(2, tmps_carry);

}

void junqueraSub(LweSample* result, LweSample* a, LweSample* b, const int nb_bits, TFheGateBootstrappingCloudKeySet* bk){
  LweSample* restando = new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);

  junqueraNegativo(restando, b, nb_bits, bk);
  junqueraAdd(result, a, restando, nb_bits, bk);

  delete_gate_bootstrapping_ciphertext_array(nb_bits, restando);
}

void junqueraMaximum(LweSample* result, LweSample* a, LweSample* b, const int nb_bits, TFheGateBootstrappingCloudKeySet* bk) {
    LweSample* tmps = new_gate_bootstrapping_ciphertext_array(2, bk->params);
    LweSample* aGreater = new_gate_bootstrapping_ciphertext_array(2, bk->params);

    LweSample* minimumMismoSigno = new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);
    LweSample* minimumOneNegative = new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);
    LweSample* oneNegative = new_gate_bootstrapping_ciphertext_array(2, bk->params);
    LweSample* negativoA = new_gate_bootstrapping_ciphertext_array(2, bk->params);
    LweSample* negativoB = new_gate_bootstrapping_ciphertext_array(2, bk->params);

    signBit(negativoA, a, nb_bits, bk);
    signBit(negativoB, b, nb_bits, bk);

    bootsXOR(&oneNegative[0], &negativoA[0], &negativoB[0], bk);

    // a > b = soloOneNegative & signBit(b)
    bootsAND(&aGreater[0], &oneNegative[0], &negativoB[0], bk);
    for(int i = 0; i < nb_bits; i++){
      bootsMUX(&minimumOneNegative[i], &aGreater[0], &b[i], &a[i], bk);
    }

    //initialize the carry to 0
    bootsCONSTANT(&tmps[0], 0, bk);

    //run the elementary comparator gate n times
    for (int i=0; i<nb_bits; i++) {
        junqueraCompare_bit(&tmps[0], &a[i], &b[i], &tmps[0], &tmps[1], bk);
    }

    //tmps[0] is the result of the comparaison: 0 if a is larger, 1 if b is larger
    //select the max and copy it to the result
    for (int i=0; i<nb_bits; i++) {
        bootsMUX(&minimumMismoSigno[i], &tmps[0], &b[i], &a[i], bk);
    }

    // Todo igual que en minimum, pero devolviendo el contrario
    for (int i=0; i<nb_bits; i++) {
        bootsMUX(&result[i], &oneNegative[0], &minimumMismoSigno[i], &minimumOneNegative[i], bk);
    }

    delete_gate_bootstrapping_ciphertext_array(2, tmps);
    delete_gate_bootstrapping_ciphertext_array(2, aGreater);
    delete_gate_bootstrapping_ciphertext_array(nb_bits, minimumMismoSigno);
    delete_gate_bootstrapping_ciphertext_array(nb_bits, minimumOneNegative);
    delete_gate_bootstrapping_ciphertext_array(2, oneNegative);
    delete_gate_bootstrapping_ciphertext_array(2, negativoA);
    delete_gate_bootstrapping_ciphertext_array(2, negativoB);

}

// Reescalar de nb_bits a nb_bits_result
void junqueraU_reescala(LweSample* result, LweSample* a, const int nb_bits_result, const int nb_bits, TFheGateBootstrappingCloudKeySet* bk){

  for(int i=0; i < nb_bits_result; i++)
    bootsCONSTANT(&result[i], 0, bk);

  // TODO Si que hay que tener en cuenta el signo
  int bits = (nb_bits > nb_bits_result) ? nb_bits_result : nb_bits;
  for(int i=0; i < bits; i++)
    bootsCOPY(&result[i], &a[i], bk);
}

void junqueraReescala(LweSample* result, LweSample* a, const int nb_bits_result, const int nb_bits, TFheGateBootstrappingCloudKeySet* bk){
  LweSample* auxA = new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);

  LweSample* n = new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);
  LweSample* corrige = new_gate_bootstrapping_ciphertext_array(2, bk->params);

  LweSample* aux_res = new_gate_bootstrapping_ciphertext_array(nb_bits_result, bk->params);
  LweSample* aux_res_neg = new_gate_bootstrapping_ciphertext_array(nb_bits_result, bk->params);

  junqueraNegativo(auxA, a, nb_bits, bk);
  signBit(corrige, a, nb_bits, bk);
  // Trabajaremos con el positivo
  junqueraMaximum(n, auxA, a, nb_bits, bk);

  junqueraU_reescala(aux_res, n, nb_bits_result, nb_bits, bk);

  junqueraNegativo(aux_res_neg, aux_res, nb_bits_result, bk);
  for(int i = 0; i < nb_bits_result; i++)
    bootsMUX(&result[i], &corrige[0], &aux_res_neg[i], &aux_res[i], bk);
}

void junqueraGte(LweSample* result, LweSample* a, LweSample* b, const int nb_bits, TFheGateBootstrappingCloudKeySet* bk){
  LweSample* eq = new_gate_bootstrapping_ciphertext_array(2, bk->params);

  bootsCONSTANT(&result[0], 0, bk);
  for(int i = 0; i < nb_bits; i++){
    bootsXNOR(&eq[0], &a[i], &b[i], bk);
    bootsMUX(&result[0], &eq[0], &result[0], &a[i], bk);
  }

  delete_gate_bootstrapping_ciphertext_array(2, eq);

}

void junqueraU_div(LweSample* cociente, LweSample* a, LweSample* b, const int nb_bits, TFheGateBootstrappingCloudKeySet* bk) {
  /*
  def div(dividendo, divisor):
      cociente = [i for i in cero]
      resto = [i for i in cero]
      padding = int(bits/2)
      divisor = shiftl(divisor, padding - 1)
      for i in range(padding):
          if debug:
              print(i)
              print(dividendo)
              print(divisor)
              print(cociente)
              print(resto)
              input()
          gt = 1 if gte(dividendo, divisor) else 0
          cociente[padding + i] = gt
          resto = sub(dividendo, divisor) if gt else resto
          dividendo = resto if gt else dividendo
          divisor = shiftr(divisor, 1)
      return cociente
  */
  LweSample* aux = new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);
  LweSample* aux2 = new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);

  LweSample* gt = new_gate_bootstrapping_ciphertext_array(2, bk->params);

  LweSample* div_aux = new_gate_bootstrapping_ciphertext_array(2*nb_bits, bk->params);
  LweSample* div_aux2 = new_gate_bootstrapping_ciphertext_array(2*nb_bits, bk->params);
  LweSample* dividendo = new_gate_bootstrapping_ciphertext_array(2*nb_bits, bk->params);
  LweSample* divisor = new_gate_bootstrapping_ciphertext_array(2*nb_bits, bk->params);
  LweSample* resto = new_gate_bootstrapping_ciphertext_array(2*nb_bits, bk->params);

  junqueraReescala(dividendo, a, 2*nb_bits, nb_bits, bk);

  junqueraReescala(div_aux, b, 2*nb_bits, nb_bits, bk);

  junqueraU_shiftl(divisor, div_aux, nb_bits - 1, 2*nb_bits, bk);



  for(int i = 0; i < nb_bits; i++) {
    // gt = dividendo >= divisor
    junqueraGte(gt, dividendo, divisor, 2*nb_bits, bk);

    bootsCOPY(&cociente[nb_bits-i-1], &gt[0], bk);

    // resto = gt? sub(dividendo, divisor) : resto
    junqueraSub(div_aux, dividendo, divisor, 2*nb_bits, bk);
    // divisor = shiftr(divisor, 1)
    junqueraU_shiftr(div_aux2, divisor, 1, 2*nb_bits, bk);
    for(int j = 0; j < 2*nb_bits; j++){
      bootsMUX(&resto[j], &gt[0], &div_aux[j], &dividendo[j], bk);
      // dividendo = gt ? resto : dividendo
      bootsMUX(&dividendo[j], &gt[0], &resto[j], &dividendo[j], bk);
      bootsCOPY(&divisor[j], &div_aux2[j], bk);
    }
  }

  delete_gate_bootstrapping_ciphertext_array(nb_bits, aux);
  delete_gate_bootstrapping_ciphertext_array(nb_bits, aux2);
  delete_gate_bootstrapping_ciphertext_array(2, gt);
  delete_gate_bootstrapping_ciphertext_array(2, div_aux);
  delete_gate_bootstrapping_ciphertext_array(2, div_aux2);
  delete_gate_bootstrapping_ciphertext_array(2, dividendo);
  delete_gate_bootstrapping_ciphertext_array(2, divisor);
  delete_gate_bootstrapping_ciphertext_array(2, resto);

}

void junqueraDiv(LweSample* result, LweSample* a, LweSample* b, const int nb_bits, TFheGateBootstrappingCloudKeySet* bk) {

  LweSample* aux = new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);
  LweSample* aux2 = new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);
  LweSample* negatA = new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);
  LweSample* negatB = new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);
  LweSample* opA = new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);
  LweSample* opB = new_gate_bootstrapping_ciphertext_array(nb_bits, bk->params);

  LweSample* gt = new_gate_bootstrapping_ciphertext_array(2, bk->params);
  LweSample* bit = new_gate_bootstrapping_ciphertext_array(2, bk->params);
  LweSample* corrige = new_gate_bootstrapping_ciphertext_array(2, bk->params);
  LweSample* isNegativeA = new_gate_bootstrapping_ciphertext_array(2, bk->params);
  LweSample* isNegativeB = new_gate_bootstrapping_ciphertext_array(2, bk->params);

  LweSample* div_aux = new_gate_bootstrapping_ciphertext_array(2*nb_bits, bk->params);
  LweSample* div_aux2 = new_gate_bootstrapping_ciphertext_array(2*nb_bits, bk->params);
  LweSample* dividendo = new_gate_bootstrapping_ciphertext_array(2*nb_bits, bk->params);
  LweSample* divisor = new_gate_bootstrapping_ciphertext_array(2*nb_bits, bk->params);
  LweSample* cociente = new_gate_bootstrapping_ciphertext_array(2*nb_bits, bk->params);
  LweSample* resto = new_gate_bootstrapping_ciphertext_array(2*nb_bits, bk->params);

  for(int i = 0; i < nb_bits; i++){
    bootsCONSTANT(&aux[i], 0, bk);
    bootsCONSTANT(&aux2[i], 0, bk);
    bootsCONSTANT(&opA[i], 0, bk);
    bootsCONSTANT(&opB[i], 0, bk);
  }

  for(int i = 0; i < 2*nb_bits; i++) {
    bootsCONSTANT(&dividendo[i], 0, bk);
    bootsCONSTANT(&div_aux[i], 0, bk);
    bootsCONSTANT(&div_aux2[i], 0, bk);
    bootsCONSTANT(&divisor[i], 0, bk);
    bootsCONSTANT(&cociente[i], 0, bk);
    bootsCONSTANT(&resto[i], 0, bk);
  }

  for(int i = 0; i < 2; i++){
    bootsCONSTANT(&gt[i], 0, bk);
    bootsCONSTANT(&bit[i], 0, bk);
  }

  // BEGIN LOGICA_SIGNO
  junqueraNegativo(negatA, a, nb_bits, bk);
  junqueraNegativo(negatB, b, nb_bits, bk);

  /**
    Ponemos los dos números en positivo
  */
  junqueraMaximum(opA, negatA, a, nb_bits, bk);
  junqueraMaximum(opB, negatB, b, nb_bits, bk);

  // Si solo uno de los dos es negativo, el resultado es negativo
  signBit(isNegativeA, a, nb_bits, bk);
  signBit(isNegativeB, b, nb_bits, bk);
  bootsXOR(corrige, isNegativeA, isNegativeB, bk);
  // END LOGICA_SIGNO

  junqueraU_div(result, opA, opB, nb_bits, bk);

  // BEGIN LOGICA_SIGNO
  // Determinamos si devolver el resultado positivo o negativo
  junqueraNegativo(aux, result, nb_bits, bk);

  for(int i = 0; i < nb_bits; i++){
    bootsMUX(&result[i], &corrige[0], &aux[i], &result[i], bk);
  }

    //Add 1
    LweSample* one = new_gate_bootstrapping_ciphertext(bk->params);
    bootsCONSTANT(one, 1, bk);
    adderManyAndOne(result, result, one, nb_bits, bk);

  // END LOGICA_SIGNO

  delete_gate_bootstrapping_ciphertext_array(nb_bits, aux);
  delete_gate_bootstrapping_ciphertext_array(nb_bits, aux2);
  delete_gate_bootstrapping_ciphertext_array(nb_bits, negatA);
  delete_gate_bootstrapping_ciphertext_array(nb_bits, negatB);
  delete_gate_bootstrapping_ciphertext_array(nb_bits, opA);
  delete_gate_bootstrapping_ciphertext_array(nb_bits, opB);

  delete_gate_bootstrapping_ciphertext_array(2, gt);
  delete_gate_bootstrapping_ciphertext_array(2, bit);
  delete_gate_bootstrapping_ciphertext_array(2, corrige);
  delete_gate_bootstrapping_ciphertext_array(2, isNegativeA);
  delete_gate_bootstrapping_ciphertext_array(2, isNegativeB);

  delete_gate_bootstrapping_ciphertext_array(2*nb_bits, div_aux);
  delete_gate_bootstrapping_ciphertext_array(2*nb_bits, div_aux2);
  delete_gate_bootstrapping_ciphertext_array(2*nb_bits, dividendo);
  delete_gate_bootstrapping_ciphertext_array(2*nb_bits, divisor);
  delete_gate_bootstrapping_ciphertext_array(2*nb_bits, cociente);
  delete_gate_bootstrapping_ciphertext_array(2*nb_bits, resto);

  //delete_gate_bootstrapping_ciphertext(one);
}


int main() {
    
    printf("reading the key...\n");

    //reads the cloud key from file
    FILE* cloud_key = fopen("cloud-KNN.key","rb");
    TFheGateBootstrappingCloudKeySet* bk = new_tfheGateBootstrappingCloudKeySet_fromFile(cloud_key);
    fclose(cloud_key);
 
    //if necessary, the params are inside the key
    const TFheGateBootstrappingParameterSet* params = bk->params;

    //VERIF ON NUMBER 1&2
    LweSample* verifOnNumber = new_gate_bootstrapping_ciphertext_array(16, params);
    LweSample* verifOnNumber2 = new_gate_bootstrapping_ciphertext_array(16, params);

    //-|-|-Reading the input-|-|-
    printf("reading the input...\n");

    //read : 1-source parameters, 2-model classes, 3-model parameters
    //Declaration
    int numberOfVectors = 10;
    LweSample* LweNumberOfVectors = new_gate_bootstrapping_ciphertext_array(16, params); // d = 10
    bootsCONSTANT(&LweNumberOfVectors[0], 0, bk);
    bootsCONSTANT(&LweNumberOfVectors[1], 1, bk);
    bootsCONSTANT(&LweNumberOfVectors[2], 0, bk);
    bootsCONSTANT(&LweNumberOfVectors[3], 1, bk);
    for(int i=4;i<16;i++)
        bootsCONSTANT(&LweNumberOfVectors[i], 0, bk);
        
    int int_kParam = 7;
    LweSample* kParam = new_gate_bootstrapping_ciphertext_array(16, params); // k = 7
    bootsCONSTANT(&kParam[0], 1, bk);
    bootsCONSTANT(&kParam[1], 1, bk);
    bootsCONSTANT(&kParam[2], 1, bk);
    for(int i=3;i<16;i++)
        bootsCONSTANT(&kParam[i], 0, bk);

    LweSample* sourceParametersCiphertexts[30]; //1*30*16 (1 vector, 30 parameters, 16 bits) 
    LweSample* modelClassesCiphertexts[10]; //10*1*8 (10 vector, 1 parameter, 8 bits)
    LweSample* existingClassesCiphertexts[2]; //2*8 (2 classes, 8 bits)
    LweSample* modelParametersCiphertexts[10][30]; //10*30*16 (10 vector, 30 parameters, 16 bits)

    //Initialisation
    //For each parameter
    for(int j = 0;j<1;j++) //1 paramètre au lieu de 30
        sourceParametersCiphertexts[j] = new_gate_bootstrapping_ciphertext_array(16, params);
    //For each vector
    for(int i = 0;i<numberOfVectors;i++){
        modelClassesCiphertexts[i] = new_gate_bootstrapping_ciphertext_array(8, params);
        //for each parameter
        for(int j = 0;j<1;j++)//1 paramètre au lieu de 30
            modelParametersCiphertexts[i][j] = new_gate_bootstrapping_ciphertext_array(16, params);
    }
    //For each existing class
    for(int j = 0;j<2;j++) //1 paramètre au lieu de 30
        existingClassesCiphertexts[j] = new_gate_bootstrapping_ciphertext_array(8, params);
    //For each verif on number TEST
    for(int i = 0;i<16;i++){
        bootsCONSTANT(&verifOnNumber[i], 0, bk);
        bootsCONSTANT(&verifOnNumber2[i], 0, bk);
    }

    //Reading
    //1-Reads the 1*30*16 source parameters ciphertexts from file
    FILE* sourceParameters_data = fopen("source-parameters-KNN.data","rb");
    //For each parameter
    for(int i = 0;i<1;i++)//1 paramètre au lieu de 30
        //For each bit
        for (int j=0; j<16; j++)
            import_gate_bootstrapping_ciphertext_fromFile(sourceParameters_data, &sourceParametersCiphertexts[i][j], params);
    fclose(sourceParameters_data);

    //2-Reads the 10*1*8 model classes ciphertexts from file
    FILE* modelClasses_data = fopen("model-classes-KNN.data","rb");
    //For each vector
    for(int v = 0;v<numberOfVectors;v++)
        //For each bit
        for (int j=0; j<8; j++)
            import_gate_bootstrapping_ciphertext_fromFile(modelClasses_data, &modelClassesCiphertexts[v][j], params);
    fclose(modelClasses_data);

    //3-Reads the 2*8 existing classes ciphertexts from file
    FILE* existingClasses_data = fopen("existing-classes-KNN.data","rb");
    //For each vector
    for(int i = 0;i<2;i++)
        //For each bit
        for (int j=0; j<8; j++)
            import_gate_bootstrapping_ciphertext_fromFile(existingClasses_data, &existingClassesCiphertexts[i][j], params);
    fclose(existingClasses_data);

    //4-Reads the 10*30*16 model parameters ciphertexts from file
    FILE* modelParameters_data = fopen("model-parameters-KNN.data","rb");
    //For each vector
    for(int v = 0;v<numberOfVectors;v++)
        //For each parameter
        for (int i=0; i<1; i++)//1 paramètre au lieu de 30
            //For each bit
            for (int j=0; j<16; j++)
                import_gate_bootstrapping_ciphertext_fromFile(modelParameters_data, &modelParametersCiphertexts[v][i][j], params);
    fclose(modelParameters_data);


    //-|-|-Homomorphic computation-|-|-
    printf("doing the homomorphic computation...\n");
    printf("operations : from deltas to isInK\n");
    //printf("TEST-1\n");

    time_t start_timeComputation = clock(); //TIME
    //printf("TEST0\n");

    ///1-Build delta list

    //Declaration
    LweSample* deltaList[10];
    //printf("TEST1\n");
    
    //Initialisation
    //For each vector
    for(int i = 0;i<numberOfVectors;i++){ //2 sur 10 (essai)
        deltaList[i] = new_gate_bootstrapping_ciphertext_array(/*32*/16, params);
        //printf("TEST2 %d\n", i);
        for(int j=0;j<16;j++)
            bootsCONSTANT(&deltaList[i][j], 0, bk);
    }
    printf("TEST 1\n");

    time_t start_timeDeltaList = clock(); //TIME

    //Computation
    //For each vector
    for(int i = 0;i<numberOfVectors;i++){ //1 vecteurs sur 10 (essai)
        //printf("TEST3 %d\n", i);
        //LweSample* euclidianSum = new_gate_bootstrapping_ciphertext_array(16, params);
        LweSample* theSign = new_gate_bootstrapping_ciphertext(params);
        //For each parameters
        for(int j = 0;j<1;j++){ //1 paramètre sur 30 (essai)
            //printf("TEST4 %d %d\n", i, j);
            LweSample* temp1 = new_gate_bootstrapping_ciphertext_array(16, params);
            //printf("TEST5 %d %d\n", i, j);
            LweSample* temp2 = new_gate_bootstrapping_ciphertext_array(16, params);
            LweSample* temp3 = new_gate_bootstrapping_ciphertext_array(16, params);
            LweSample* temp4 = new_gate_bootstrapping_ciphertext_array(16, params);
            //LweSample* temp5 = new_gate_bootstrapping_ciphertext_array(32, params);
            //LweSample* temp6 = new_gate_bootstrapping_ciphertext_array(64, params);
            //printf("TEST6 %d %d\n", i, j);
            //Compute model - source
            vatsaltailorsubtract(temp1, modelParametersCiphertexts[i][j], sourceParametersCiphertexts[j], 16, bk);
            //printf("TEST7 %d %d\n", i, j);
            //signBit(theSign, temp1, 16, bk); //TEST sans
            //printf("TEST8 %d %d\n", i, j);
            reverse(temp2, temp1, 16, bk);
            //printf("TEST9 %d %d\n", i, j);
            vatsaltailorMultiplexer(temp3, temp1, temp2, /*theSign*/&temp1[15], 16, bk);
            //Square
            //printf("TEST10- %d %d\n", i, j);
            //copy(temp5, temp3, 32, bk);
            //printf("TEST11 %d %d\n", i, j);
            /*time_t start_timeSquare = clock(); //TIME
            vatsaltailorMultiply(temp4, temp3, temp3, 16, bk); // TO COMMENT FOR MANHATTAN, TO DECOMMENT FOR EUCLIDEAN
            time_t end_timeSquare = clock(); //TIME
            printf("......Square took: %ld microsecs\n",end_timeSquare-start_timeSquare); //TIME*/
            //Sum
            //printf("TEST12 %d %d\n", i, j);
            copy(temp4, deltaList[i], 16, bk);
            vatsaltailorAdder(deltaList[i], temp4, temp3, 16, bk); //sans le carré
            //vatsaltailorAdder(deltaList[i], deltaList[i], temp4, 32, bk); //avec le carré
            /*//VERIF ON NUMBER
            copy(verifOnNumber, deltaList[i], 16, bk);*/
            printf("COMP 6\n");
            //printf("TEST13 %d %d\n", i, j);

            delete_gate_bootstrapping_ciphertext_array(16, temp1);
            delete_gate_bootstrapping_ciphertext_array(16, temp2);
            delete_gate_bootstrapping_ciphertext_array(16, temp3);
            delete_gate_bootstrapping_ciphertext_array(16, temp4);
        }
        delete_gate_bootstrapping_ciphertext(theSign);
    }
    time_t end_timeDeltaList = clock(); //TIME
    printf("......DeltaList took: %ld microsecs\n",end_timeDeltaList-start_timeDeltaList); //TIME

    printf("TEST 2\n");


    time_t start_timeSort = clock(); //TIME

    ///2-Sort

    /*LweSample* dOriginalRanks[10];
    //LweSample* temp = new_gate_bootstrapping_ciphertext_array(8, params);
    //For each vector
    for(int i=0;i<numberOfVectors;i++){
        dOriginalRanks[i] = new_gate_bootstrapping_ciphertext_array(8, params);
        //For each bit
        for(int j=0;j<8;j++)
            bootsCONSTANT(&dOriginalRanks[i][j], 0, bk);
    }*/

    //Insertion sort
    /*LweSample* t1 = new_gate_bootstrapping_ciphertext_array(16, params); //temp delta
    LweSample* t2 = new_gate_bootstrapping_ciphertext_array(8, params); //temp class
    LweSample* tempSign = new_gate_bootstrapping_ciphertext(params);
    LweSample* flag = new_gate_bootstrapping_ciphertext(params);
    bootsCONSTANT(flag, 0, bk);
    printf("TEST 3\n");

    for (int c = 1 ; c <= 10 - 1; c++) {
        printf("TEST 4\n");
        copy(t1, deltaList[c], 16, bk);
        copy(t2, modelClassesCiphertexts[c], 8, bk);
        printf("TEST 5\n");
        int d = 0;
        for (d = c-1 ; d >= 0; d--){
            printf("TEST 6\n");
            isInfEqual(tempSign, t1, deltaList[d], 16, bk);
            vatsaltailorMultiplexer(deltaList[d+1], deltaList[d], deltaList[d+1], tempSign, 16, bk);
            vatsaltailorMultiplexer(modelClassesCiphertexts[d+1], modelClassesCiphertexts[d], modelClassesCiphertexts[d+1], tempSign, 8, bk);
            printf("TEST 7\n");
        }
        printf("TEST 8\n");
        vatsaltailorMultiplexer(deltaList[d+1], t1, deltaList[d+1], flag, 16, bk);
        vatsaltailorMultiplexer(modelClassesCiphertexts[d+1], t2, modelClassesCiphertexts[d+1], flag, 8, bk);
    }*/

    //Bubble sort
    LweSample* t1 = new_gate_bootstrapping_ciphertext_array(16, params); //temp delta
    LweSample* t2 = new_gate_bootstrapping_ciphertext_array(8, params); //temp class
    LweSample* tempSign = new_gate_bootstrapping_ciphertext(params);
    for (int i=10-1;i>0;i--){
        for (int j=0;j<i;j++){
            isInfEqual(tempSign, deltaList[j+1], deltaList[j], 16, bk);
            copy(t1, deltaList[j], 16, bk);
            copy(t2, modelClassesCiphertexts[j], 8, bk);
            vatsaltailorMultiplexer(deltaList[j], deltaList[j], deltaList[j+1], tempSign, 16, bk);
            vatsaltailorMultiplexer(modelClassesCiphertexts[j], modelClassesCiphertexts[j], modelClassesCiphertexts[j+1], tempSign, 8, bk);
            vatsaltailorMultiplexer(deltaList[j+1], deltaList[j+1], t1, tempSign, 16, bk);
            vatsaltailorMultiplexer(modelClassesCiphertexts[j+1], modelClassesCiphertexts[j+1], t2, tempSign, 8, bk);
        }
    }
    
    time_t end_timeSort = clock(); //TIME
    printf("......Sort took: %ld microsecs\n",end_timeSort-start_timeSort); //TIME

    printf("TEST 9\n");

    time_t start_timeVote = clock(); //TIME

    ///3-Vote
    LweSample* predictedClass = new_gate_bootstrapping_ciphertext_array(8, params);
    LweSample* countB = new_gate_bootstrapping_ciphertext_array(8, params);
    LweSample* countM = new_gate_bootstrapping_ciphertext_array(8, params);
    LweSample* addB = new_gate_bootstrapping_ciphertext(params);//bit
    LweSample* addM = new_gate_bootstrapping_ciphertext(params);//bit
    LweSample* tempcompar = new_gate_bootstrapping_ciphertext(params);
    for(int i=0;i<8;i++){
        bootsCONSTANT(&countB[i], 0, bk);
        bootsCONSTANT(&countM[i], 0, bk);
    }
    //copy(results[0], countB, 8, bk);
    //copy(results[1], countM, 8, bk);

    for(int i=0;i<7;i++){
        isEqual(addB, modelClassesCiphertexts[i], existingClassesCiphertexts[0], 8, bk);
        isEqual(addM, modelClassesCiphertexts[i], existingClassesCiphertexts[1], 8, bk);
        adderManyAndOne(countB, countB, addB, 8, bk);
        adderManyAndOne(countM, countM, addM, 8, bk);
    }
    copy(verifOnNumber, countB, 8, bk);
    copy(verifOnNumber2, countM, 8, bk);
    isInfEqual(tempcompar, countB, countM, 8, bk);
    vatsaltailorMultiplexer(predictedClass, existingClassesCiphertexts[1], existingClassesCiphertexts[0], tempcompar, 8, bk);
    //char predictedClass = (countB<=countM)?'M':'B';

    delete_gate_bootstrapping_ciphertext_array(8, countB);
    delete_gate_bootstrapping_ciphertext_array(8, countM);
    delete_gate_bootstrapping_ciphertext(addB);
    delete_gate_bootstrapping_ciphertext(addM);
    delete_gate_bootstrapping_ciphertext(tempcompar);

    time_t end_timeVote = clock(); //TIME
    printf("......Vote took: %ld microsecs\n",end_timeVote-start_timeVote); //TIME

    //time_t start_timeFindNeighbour = clock(); //TIME

    /*//4-Find neighbour

    printf("TEST 8\n");
    //d-k
    LweSample* dMinusk = new_gate_bootstrapping_ciphertext_array(16, params);
    vatsaltailorsubtract(dMinusk, LweNumberOfVectors, kParam, 16, bk);

    printf("TEST 9\n");
    //Compar
    LweSample* isInk[10];
    //For each vector
    for(int i=0;i<numberOfVectors;i++){
        isInk[i] = new_gate_bootstrapping_ciphertext(params);
        LweSample* temp1 = new_gate_bootstrapping_ciphertext_array(16, params);
        LweSample* theSign = new_gate_bootstrapping_ciphertext(params);
        vatsaltailorsubtract(temp1, dSum[i], dMinusk, 16, bk);
        if(i==9)
            //VERIF ON NUMBER
            copy(verifOnNumber, temp1, 16, bk);
        signBit(theSign, temp1, 16, bk);
        bootsNOT(isInk[i], theSign, bk);
    }
    printf("TEST 10\n");

    time_t end_timeFindNeighbour = clock(); //TIME
    printf("......FindNeighbour took: %ld microsecs\n",end_timeFindNeighbour-start_timeFindNeighbour); //TIME*/

    time_t end_timeComputation = clock(); //TIME
    printf("......computation took: %ld microsecs\n",end_timeComputation-start_timeComputation); //TIME
    
    printf("writing the answer to file...\n");
    
    time_t start_timeExportAnswer = clock(); //TIME

    //Export predicted class
    FILE* predictedClass_data = fopen("predicted-class-KNN.data","wb");
    //For each bit
    for (int j=0; j<8; j++)
        export_gate_bootstrapping_ciphertext_toFile(predictedClass_data, &predictedClass[j], params);
    fclose(predictedClass_data);

    /*//export the numberOfVectors*1 ciphertexts to a file (for the client)
    FILE* answer_data = fopen("answer-KNN.data","wb");
    for(int i = 0;i<numberOfVectors;i++) //2 sur 10 (essai)
        export_gate_bootstrapping_ciphertext_toFile(answer_data, isInk[i], params);
    fclose(answer_data);*/

    /*//TEST1 export the 10*1*16 model parameters ciphertexts to a file (for the client)
    FILE* answerTEST1_data = fopen("answerTEST1-KNN.data","wb");
    //For each vector
    for(int v = 0;v<numberOfVectors;v++)
        //For each parameter
        for(int i = 0;i<1;i++)//1 paramètre au lieu de 30
            //For each bit
            for (int j=0; j<16; j++)
                export_gate_bootstrapping_ciphertext_toFile(answerTEST1_data, &modelParametersCiphertexts[v][i][j], params);
    fclose(answerTEST1_data);*/

    /*//TEST2 export the 10*10*1 matrix to a file (for the client)
    FILE* matrix_data = fopen("matrix-KNN.data","wb");
    //For each line
    for(int i = 0;i<numberOfVectors;i++)//1 paramètre au lieu de 30
        //For each column
        for (int j=0; j<numberOfVectors; j++)
            export_gate_bootstrapping_ciphertext_toFile(matrix_data, deltaMatrix[i][j], params);
    fclose(matrix_data);*/

    //TEST3 export the 10*1*1 deltalist to a file (for the client)
    FILE* deltalist_data = fopen("deltalist-KNN.data","wb");
    //For each vector
    for(int i = 0;i<numberOfVectors;i++)//1 paramètre au lieu de 30
        //For each bit
        for (int j=0; j<16; j++)
            export_gate_bootstrapping_ciphertext_toFile(deltalist_data, &deltaList[i][j], params);
    fclose(deltalist_data);

    //TEST4 export sorted classes
    FILE* sortedClasses_data = fopen("sorted-classes-KNN.data","wb");
    //For each vector
    for(int v = 0;v<numberOfVectors;v++)
        //For each bit
        for (int j=0; j<8; j++)
            export_gate_bootstrapping_ciphertext_toFile(sortedClasses_data, &modelClassesCiphertexts[v][j], params);
    fclose(sortedClasses_data);

    /*//TEST3 export the 10*1*1 dRank to a file (for the client)
    FILE* dRank_data = fopen("dSum-KNN.data","wb");
    //For each vector
    for(int i = 0;i<numberOfVectors;i++)//1 paramètre au lieu de 30
        //For each bit
        for (int j=0; j<8; j++)
            export_gate_bootstrapping_ciphertext_toFile(dRank_data, &dRank[i][j], params);
    fclose(dRank_data);*/

    //VERIF ON NUMBER
    FILE* verifOnNumber_data = fopen("verifOnNumber-KNN.data","wb");
    //For each bit
    for (int j=0; j<16; j++)
        export_gate_bootstrapping_ciphertext_toFile(verifOnNumber_data, &verifOnNumber[j], params);
    fclose(verifOnNumber_data);

    //VERIF ON NUMBER 2
    FILE* verifOnNumber2_data = fopen("verifOnNumber2-KNN.data","wb");
    //For each bit
    for (int j=0; j<16; j++)
        export_gate_bootstrapping_ciphertext_toFile(verifOnNumber2_data, &verifOnNumber2[j], params);
    fclose(verifOnNumber2_data);


    time_t end_timeExportAnswer = clock(); //TIME
    printf("......ExportAnswer took: %ld microsecs\n",end_timeExportAnswer-start_timeExportAnswer); //TIME

    printf("TEST 11\n");

    time_t start_timeCleanPointers = clock(); //TIME

    ///-|-|-Clean up all pointers-|-|-
    //1-Clean model vectors
    //For each vector
    for(int i = 0;i<numberOfVectors;i++){
        delete_gate_bootstrapping_ciphertext_array(8, modelClassesCiphertexts[i]);
        //For each parameter
        for(int j = 0;j<1;j++){ //1 paramètre au lieu de 30
            delete_gate_bootstrapping_ciphertext_array(16, modelParametersCiphertexts[i][j]);
        }
    }
    printf("TEST 12\n");
    //2-Clean source vector
    //For each parameter
    for(int j = 0;j<1;j++){ //1 paramètre au lieu de 30
        delete_gate_bootstrapping_ciphertext_array(16, sourceParametersCiphertexts[j]);
    }
    printf("TEST 13\n");
    //3-Clean temp and answer
    for(int i = 0;i<numberOfVectors;i++){
        //delete_gate_bootstrapping_ciphertext(isInk[i]);
        //delete_gate_bootstrapping_ciphertext_array(16, dSum[i]);
        delete_gate_bootstrapping_ciphertext_array(16, deltaList[i]);
        //for(int j=0;j<numberOfVectors;j++)
            //delete_gate_bootstrapping_ciphertext(deltaMatrix[i][j]);
    }
    //delete_gate_bootstrapping_ciphertext_array(16, dMinusk);
    delete_gate_bootstrapping_ciphertext_array(16, verifOnNumber);
    delete_gate_bootstrapping_ciphertext_array(16, verifOnNumber2);
    printf("TEST 14\n");
    //4-Clean cloud key
    delete_gate_bootstrapping_cloud_keyset(bk);

    time_t end_timeCleanPointers = clock(); //TIME
    printf("......CleanPointers took: %ld microsecs\n",end_timeCleanPointers-start_timeCleanPointers); //TIME

}
