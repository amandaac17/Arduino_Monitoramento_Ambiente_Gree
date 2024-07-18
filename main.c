/*
 * Códigos auxiliares utilizados no projeto Monitoramento Ambiente
 *
 * Temos os códigos para normalizar os arrays originais,
 * transformá-los para binário, modificar os binários e
 * retransformar para o array no formato original.
 *
 *
 * Autora: Amanda Almeida Cardoso
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TOTAL_BYTES 25
#define TOTAL_BITS (TOTAL_BYTES*8)+1
#define TOTAL_ARRAY 419
#define DURATION_SHORT 700

#define HEADER1 9040
#define HEADER2 4452
#define SHORTPAUSE 20000
#define LONGPAUSE 40000
#define TRUE 1
#define FALSE 0


/*
 * PRIMEIRA PARTE: NORMALIZANDO OS ARRAYS
 *
 *
 * Se os valores estiverem dentro dos intervalos de marca e espaço, são transofrmados entre 0 e 1
 * Senão, mantém-se os valores originais (cabeçalhos e pausas)
 *
 */
void *normaliza(int array[], int size){

    int*normalizado = (int*)malloc(sizeof(int)*size);
    for(int i = 0; i < size; i++){
        if(array[i] > 450 && array[i] < 750){
            array[i] = 700;
            normalizado[i] = 700;
        }
        else if(array[i] > 1000 && array[i] < 1650){
            array[i] = 1650;
            normalizado[i] = 1650;
        }else{
            normalizado[i] = array[i];
        }
    }

    for(int i = 0; i < size; i++){
        printf("%d,", array[i]);
        if((i+1)%2==0){
            //printf("\n");
        }
    }

    return normalizado;
}


/*
 *
 * SEGUNDA PARTE: TRANSFORMANDO EM BINÁRIO
 *
 * Aqui temos algumas formas diferentes de fazer isso
 *
 Função para converter o array em binário - OPÇÃO 1
 */
int* converterArray(int array[], int tamanho) {
    int i;
    int idx = 0;

    int *arr = (int*)malloc(sizeof(int)*(tamanho/2));
    if(arr!=NULL) {
        int aux = 0;

        for (i = 0; i < tamanho - 1; i += 2) {
            if (array[i] >= 500 && array[i] <= 750 && array[i + 1] <= -1400 && array[i + 1] >= -2000) {
                //1 se for marca (pulso curto, pulso longo)
                arr[idx] = 1;
                idx++;
            } else if (array[i] >= 500 && array[i] <= 750 && array[i + 1] >= -750 && array[i + 1] <= -450) {
                //0 se for espaço (pulso curto, pulso curto)
                arr[idx] = 0;
                idx++;
            } else {
                i--;
                aux++;

                // Padrão para valores que não correspondem às regras
                //arr[idx] = -1;
                // idx++;

            }
        }
        printf("Foram ignorados %d numeros\n", aux);
        return arr;
    }else{
        printf("Array null");
        return NULL;
    }
}

// Função para converter o array em binário - OPÇÃO 2

int* converterParaBinario(int *array, int tamanho) {
    int* binario = (int*)malloc(tamanho * sizeof(int));
    int idx = 0;

    for (int i = 0; i < tamanho - 1; i++) {
        if (array[i] == 700) {
            if (array[i + 1] == 1650) {
                binario[idx] = 1;
                idx++;
                i++; // Avança para o próximo par
            } else if (array[i + 1] == 700) {
                binario[idx] = 0;
                idx++;
                i++; // Avança para o próximo par
            }else{
                i++;
            }
        }
    }
    for(int i = 0; i < idx; i++){
        printf("%d",binario[i]);
        if((i+1)%8==0){
            printf(" ");
        }
    }

    return binario;
}

/*
 *
 * TERCEIRA PARTE: MODIFICANDO OS ARRAYS BINÁRIOS
 *
 * Também temos formas diferentes de fazer isso
 *
 * Função que modifica os bytes e devolve um array binário criado com as modificações para a temperatura específica */

int* modificarBytes(int binArray[],int temperatura) {

    unsigned char byte1, byte2, byte3, byte8, byte9, byte10, byte12, byte17, byte21, byte25;


    unsigned char byte2Valores[15] = {
            0b00000000, 0b10000000, 0b01000000, 0b11000000,
            0b00100000, 0b10100000, 0b01100000, 0b11100000,
            0b00010000, 0b10010000, 0b01010000, 0b11010000,
            0b00110000, 0b10110000, 0b01110000
    };

    unsigned char byte9Valores[15] = {
            0b11110011, 0b00010011, 0b00010011, 0b10010011,
            0b10010011, 0b01010011, 0b01010011, 0b11010011,
            0b11010011, 0b00110011, 0b00110011, 0b10110011,
            0b10110011, 0b01110011, 0b01110011
    };

    unsigned char byte10Valores[15] = {
            0b01000000, 0b01010000, 0b01001000, 0b01011000,
            0b01000100, 0b01010100, 0b01001100, 0b01011100,
            0b01000010, 0b01010010, 0b01001010, 0b01011010,
            0b01000110, 0b01010110, 0b01001110
    };

    unsigned char byte17Valores[15] = {
            0b00001000, 0b00101000, 0b00011000, 0b00111000,
            0b00000100, 0b00100100, 0b00010100, 0b00110100,
            0b00001100, 0b00101100, 0b00011100, 0b00111100,
            0b00000000, 0b00100000, 0b00010000
    };


    // Cria uma cópia do array binário original
    int *modificado = (int*)malloc(TOTAL_BITS * sizeof(int));
    if (modificado == NULL) {
        printf("Erro ao alocar memória\n");
        return NULL;
    }

    memcpy(modificado, binArray, TOTAL_BITS * sizeof(int));
    //BYTE 1:
    byte1 = 0b10011010;
    //modificado [0] 000000001
    //modificado [1] = 000000000
    //Quando usamos a operação AND (& 1) após um deslocamento de bits,estamos isolando apenas o bit menos significativo do resultado do deslocamento.
    for (int i = 0; i < 8; i++) {
        modificado[0 + i] = (byte1 >> (7 - i)) & 1;
    }

    // BYTE 2: Mapeamento direto dos valores fornecidos
    byte2 = byte2Valores[temperatura - 16];
    for (int i = 0; i < 8; i++) {
        modificado[8 + i] = (byte2 >> (7 - i)) & 1;
    }


    //BYTE 3: Muda para ON
    byte3 = 0b00000110;
    for(int i = 0; i < 8; i++){
        modificado[16 + i] = (byte3 >> (7 - i)) & 1;
    }


    // BYTE 8: Paridade
    if (temperatura % 2 == 0) {
        byte8 = 0b00000001;
    } else {
        byte8 = 0b00000000;
    }
    for (int i = 0; i < 8; i++) {
        modificado[56 + i] = (byte8 >> (7 - i)) & 1;
    }


    // BYTE 9: Valores específicos para cada temperatura
    byte9 = byte9Valores[temperatura - 16];
    for (int i = 0; i < 8; i++) {
        modificado[64 + i] = (byte9 >> (7 - i)) & 1;
    }


    // BYTE 10: Incrementos de 8
    byte10 = byte10Valores[temperatura - 16];
    for (int i = 0; i < 8; i++) {
        modificado[72 + i] = (byte10 >> (7 - i)) & 1;
    }


    //BYTE 12: Muda para ON
    byte12 = 0b11000001;
    for (int i = 0; i < 8; i++) {
        modificado[88 + i] = (byte12 >> (7 - i)) & 1;
    }


    //BYTE 17: Valores específicos com reset em 28 e 30
    byte17 = byte17Valores[temperatura - 16];
    for (int i = 0; i < 8; i++) {
        modificado[128 + i] = (byte17 >> (7 - i)) & 1;
    }


    //BYTE 21
    if(temperatura == 28){
        byte21 = 0b00101010;
        for(int i = 0; i < 8; i++){
            modificado[160+i] = (byte21>>(7-i)) & 1;
        }
    }


    // BYTE 25: Valores específicos para temperatura 28
    if (temperatura == 28) {
        byte25 = 0b00000101;
    } else {
        byte25 = 0b00000010;
    }
    for (int i = 0; i < 8; i++) {
        modificado[192 + i] = (byte25 >> (7 - i)) & 1;
    }

    // Bit extra: 1 ou 0
    if(temperatura == 28){
        modificado[200] = 0;
    }else{
        modificado[200] = 1;
    }


    // Imprime o binário modificado
    printf("\nBinário:\n");
    for (int i = 0; i < TOTAL_BITS; i++) {
        printf("%d", modificado[i]);
        if ((i + 1) % 8 == 0) {
            printf(" ");
        }
    }
    printf("\n");

    return modificado;
}



void comparaDoisVetores(int *v1, int *v2){
    int stat = TRUE;
    for(int i = 0; i < TOTAL_BITS; i++){
        if(v1[i]!=v2[i]){
            stat = FALSE;
            break;
        }
    }
    printf("\no status eh %d\n",stat);
}


/*Função que altera o array enviado, não devolve outro array */
void modificaBinArrayEnviado(char cmd_bin[], int temperatura){


    unsigned char byte1, byte2, byte3, byte8, byte9, byte10, byte12, byte17, byte21, byte25;

    unsigned char byte2Valores[15] = {
            0b00000000, 0b10000000, 0b01000000, 0b11000000,
            0b00100000, 0b10100000, 0b01100000, 0b11100000,
            0b00010000, 0b10010000, 0b01010000, 0b11010000,
            0b00110000, 0b10110000, 0b01110000
    };

    unsigned char byte9Valores[15] = {
            0b11110011, 0b00010011, 0b00010011, 0b10010011,
            0b10010011, 0b01010011, 0b01010011, 0b11010011,
            0b11010011, 0b00110011, 0b00110011, 0b10110011,
            0b10110011, 0b01110011, 0b01110011
    };

    unsigned char byte10Valores[15] = {
            0b01000000, 0b01010000, 0b01001000, 0b01011000,
            0b01000100, 0b01010100, 0b01001100, 0b01011100,
            0b01000010, 0b01010010, 0b01001010, 0b01011010,
            0b01000110, 0b01010110, 0b01001110
    };

    unsigned char byte17Valores[15] = {
            0b00001000, 0b00101000, 0b00011000, 0b00111000,
            0b00000100, 0b00100100, 0b00010100, 0b00110100,
            0b00001100, 0b00101100, 0b00011100, 0b00111100,
            0b00000000, 0b00100000, 0b00010000
    };

    //BYTE 1:
    byte1 = 0b10011010;
    cmd_bin[0] = byte1;

    // BYTE 2: Mapeamento direto dos valores fornecidos
    byte2 = byte2Valores[temperatura - 16];
    cmd_bin[1] = byte2;

    //BYTE 3: Muda para ON
    byte3 = 0b00000110;
    cmd_bin[2] = byte3;

    // BYTE 8: Paridade
    if (temperatura % 2 == 0) {
        byte8 = 0b00000001;
    } else {
        byte8 = 0b00000000;
    }
    cmd_bin[7] = byte8;

    // BYTE 9: Valores específicos para cada temperatura
    byte9 = byte9Valores[temperatura - 16];
    cmd_bin[8] = byte9;

    // BYTE 10: Incrementos de 8
    byte10 = byte10Valores[temperatura - 16];
    cmd_bin[9] = byte10;

    //BYTE 12: Muda para ON
    byte12 = 0b11000001;
    cmd_bin[11] = byte12;

    //BYTE 17: Valores específicos com reset em 28 e 30
    byte17 = byte17Valores[temperatura - 16];
    cmd_bin[16] = byte17;

    //BYTE 21
    if (temperatura == 28) {
        byte21 = 0b00101010;
        cmd_bin[20] = byte21;
    }

    // BYTE 25: Valores específicos para temperatura 28
    if (temperatura == 28) {
        byte25 = 0b00000101;
    } else {
        byte25 = 0b00000010;
    }
    cmd_bin[24] = byte25;

    // Bit extra: 1 ou 0
    if (temperatura == 28) {
        cmd_bin[25] = 0;
    } else {
        cmd_bin[25] = 1;
    }


}

/*
 *
 * QUARTA PARTE: DESCONVERTENDO DE BINÁRIO PARA O FORMATO ORIGINAL
 *
 *
 * Função que desconverte de binário para o formato original e devolve um array criado com as modificações*/

int *binDesconverte (int* array_bin){
    int idx = 0;
    int *desconvertido = (int*)malloc(sizeof(int)*TOTAL_ARRAY);
    //as pausas estão nos indices 72,73,   138,139,   140,141 (cabeçalho), 212,213,     278,279       280, 281,    352, 353

    if(desconvertido!=NULL){

        desconvertido[0] = HEADER1;
        desconvertido[1] = HEADER2;
        desconvertido[72] = DURATION_SHORT;
        desconvertido[73] = SHORTPAUSE;
        desconvertido[138] = DURATION_SHORT;
        desconvertido[139] = LONGPAUSE;
        desconvertido[140] = HEADER1;
        desconvertido[141] = HEADER2;
        desconvertido[212] = DURATION_SHORT;
        desconvertido[213] = SHORTPAUSE;
        desconvertido[278] = DURATION_SHORT;
        desconvertido[279] = LONGPAUSE;
        desconvertido[280] = HEADER1;
        desconvertido[281] = HEADER2;
        desconvertido[352] = DURATION_SHORT;
        desconvertido[353] = SHORTPAUSE;

        for(int i = 0; i < TOTAL_BITS; i++){

            while(idx == 0 || idx ==1 || idx == 72 || idx == 73 || idx ==138 || idx == 139 || idx == 140 || idx == 141 || idx == 212 || idx == 213 || idx == 278 || idx == 279 || idx == 280 || idx == 281 || idx == 352 || idx == 353){
                idx++;
            }
            if(array_bin[i] == 1){
                desconvertido[idx] = 700;
                idx++;
                desconvertido[idx] = 1650;
                idx++;
            }else{
                desconvertido[idx] = 700;
                idx++;
                desconvertido[idx] = 700;
                idx++;
            }
        }

    }
    desconvertido[418] = 700;

    printf("\nArray IR: \n");
    for(int i = 0; i < TOTAL_ARRAY;i++){
        printf("%d,",desconvertido[i]);
    }
    return desconvertido;
}

/*Função que recebe o array binário e um array vazio e o popula*/

void binDesconvertePopulaVazio(unsigned char array_bin[], int array_IR[]) {
    int idx = 0;

    // Inicializa os valores fixos no array_IR
    array_IR[0] = HEADER1;
    array_IR[1] = HEADER2;

    array_IR[72] = DURATION_SHORT;
    array_IR[73] = SHORTPAUSE;

    array_IR[138] = DURATION_SHORT;
    array_IR[139] = LONGPAUSE;

    array_IR[140] = HEADER1;
    array_IR[141] = HEADER2;

    array_IR[212] = DURATION_SHORT;
    array_IR[213] = SHORTPAUSE;

    array_IR[278] = DURATION_SHORT;
    array_IR[279] = LONGPAUSE;

    array_IR[280] = HEADER1;
    array_IR[281] = HEADER2;

    array_IR[352] = DURATION_SHORT;
    array_IR[353] = SHORTPAUSE;

    for (int byteIndex = 0; byteIndex < TOTAL_BYTES; byteIndex++) {
        for (int bitIndex = 0; bitIndex < 8; bitIndex++) {
            while (idx == 0 || idx == 1 || idx == 72 || idx == 73 || idx == 138 || idx == 139 || idx == 140 || idx == 141 ||
                   idx == 212 || idx == 213 || idx == 278 || idx == 279 || idx == 280 || idx == 281 || idx == 352 || idx == 353) {
                idx++;
            }

            int bit = (array_bin[byteIndex] >> (7 - bitIndex)) & 1;
            if (bit == 1) {
                array_IR[idx++] = 700;
                array_IR[idx++] = 1650;
            } else {
                array_IR[idx++] = 700;
                array_IR[idx++] = 700;
            }
        }
    }

    array_IR[418] = 700;

    printf("\nArray IR: \n");
    for (int i = 0; i < TOTAL_ARRAY; i++) {
        printf("%d,", array_IR[i]);
    }
    printf("\n");
}

/* FUNÇÕES EXTRAS
 * */

void adicionaVirgulaEmBin(char* bin){
    for(int i = 0; i < 201; i++){
        printf("%c,",bin[i]);
    }
}

int imprimeArrayConvertido(int arr[], int tam, int n){
    int i;
    printf("Array %d convertido:\n",n);
    for (i = 0; i < (tam/2); i++) {
        printf("%d,", arr[i]);
        //if((i+1)%8==0){
        //      printf(" ");
        // }
    }
    printf("\n");

}


int main(){

    return 0;
}