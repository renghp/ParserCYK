// Integrantes:
// João Marcos Flach
// Igor Guazzelli
// Renan Guarese

// Observações:
// - Estamos mais preocupados em otimizar o tempo, portanto estamos usando sempre vetores ao invés de listas
// - Os valores das constantes podem ser facilmente modificados para atender à demanda do arquivo de entrada
// - Para deixar o programa mais rápido, não nos preocupamos com a consistência do arquivo de entrada




////////// BIBLIOTECAS:


#include <stdio.h>
#include <string.h>




////////// DEFINIÇÕES DE CONSTANTES:


#define TAM_PALAVRA 20
#define QTD_TERMINAIS 100
#define TAM_TERMINAIS 20
#define TAM_VARIAVEL 3
#define QTD_VARIAVEIS 20
#define QTD_VAR_DIR 2
#define QTD_REGRAS_VARIAVEIS 100
#define QTD_REGRAS_TERMINAIS 100
#define QTD_IN 200
#define TAM_IN 200
#define QTD_ANOT 1000
#define FIM "#"
#define NOME_ARQ "exemplo_gramatica_cyk.txt"




////////// DEFINIÇÕES DE ESTRUTURAS:


typedef char PALAVRA[TAM_TERMINAIS];

typedef char VARIAVEL[TAM_VARIAVEL];

typedef struct{
    int linha;
    int coluna;
    int profundidade;
}
POSICAO;

typedef struct{
    VARIAVEL variavel;
    POSICAO vertical;
    POSICAO diagonal;
}
ANOTACAO;

typedef struct{
    VARIAVEL esquerda;
    PALAVRA direita[QTD_VAR_DIR];
}
REGRA;

typedef struct{
    PALAVRA terminais[QTD_TERMINAIS];
    VARIAVEL variaveis[QTD_VARIAVEIS];
    VARIAVEL inicial;
    REGRA regras_terminais[QTD_REGRAS_TERMINAIS];
    REGRA regras_variaveis[QTD_REGRAS_VARIAVEIS];
}
GRAMATICA;




////////// DEFINIÇÕES DE VARIÁVEIS GLOBAIS:


GRAMATICA gramatica_cyk;
ANOTACAO tabela[QTD_IN][QTD_IN][QTD_ANOT];
PALAVRA base_tabela[QTD_IN];
FILE *arqArvores;




////////// DEFINIÇÕES DE FUNÇÕES:


int carregar_arquivo(){
    ///////////////////// DEFINIÇÃO DE VARIAVEIS /////////////////////

    char buffer[1024];
    char *line;
    char *token;
    FILE *fstream;
    int i = 0, j;

    VARIAVEL esquerda;
    PALAVRA direita;

    //////////////////////////////////////////////////////////////////


    if((fstream = fopen(NOME_ARQ,"r")) == NULL)
        return 0;


    ///////////////////// JOGAR A PRIMEIRA LINHA FORA /////////////////////

    fgets(buffer,sizeof(buffer),fstream);

    ///////////////////////////////////////////////////////////////////////





    printf("Terminais:\n");

    ///////////////////// ARMAZENAR A SEGUNDA LINHA /////////////////////

    line = strtok(fgets(buffer,sizeof(buffer),fstream), "{}");

    j = 0;

    token = strtok(line, ", ");

    do
    {
        strcpy(gramatica_cyk.terminais[j], token);
        printf("\n%s", gramatica_cyk.terminais[j]);
        j++;
    }while ((token = strtok(NULL, ", ")) != NULL);

    strcpy(gramatica_cyk.terminais[j], FIM);

    //////////////////////////////////////////////////////////////////////





    ///////////////////// JOGAR A TERCEIRA LINHA FORA /////////////////////

    fgets(buffer,sizeof(buffer),fstream);

    ///////////////////////////////////////////////////////////////////////





    printf("\n\n\nVariaveis:\n");

    ///////////////////// ARMAZENAR A QUARTA LINHA /////////////////////

    line = strtok(fgets(buffer,sizeof(buffer),fstream), "{}");

    j = 0;

    token = strtok(line, ", ");

    do
    {
        strcpy(gramatica_cyk.variaveis[j], token);
        printf("\n%s", gramatica_cyk.variaveis[j]);
        j++;
    }while ((token = strtok(NULL, ", ")) != NULL);

    strcpy(gramatica_cyk.variaveis[j], FIM);

    /////////////////////////////////////////////////////////////////////





    ///////////////////// JOGAR A QUINTA LINHA FORA /////////////////////

    fgets(buffer,sizeof(buffer),fstream);

    /////////////////////////////////////////////////////////////////////





    printf("\n\n\nVariavel Inicial:\n");

    ///////////////////// ARMAZENAR A SEXTA LINHA /////////////////////

    line = strtok(fgets(buffer,sizeof(buffer),fstream), "{} ");

    j = 0;

    strcpy(gramatica_cyk.inicial, line);

    printf("\n%s", gramatica_cyk.inicial);

    ///////////////////////////////////////////////////////////////////





    ///////////////////// JOGAR A SETIMA LINHA FORA /////////////////////

    fgets(buffer,sizeof(buffer),fstream);

    /////////////////////////////////////////////////////////////////////





    printf("\n\n\nRegras:\n");

    ///////////////////// ARMAZENAS DA OITAVA LINHA EM DIANTE /////////////////////

    j = 0;
    i = 0;

    while((line = fgets(buffer,sizeof(buffer),fstream)) != NULL)
    {
        strtok(line, "{}");

        strcpy(esquerda, strtok(line, "{ >"));

        strcpy(direita, strtok(NULL, "> ,"));

        if((token = strtok(NULL, ", }\n")) != NULL)
        {
            strcpy(gramatica_cyk.regras_variaveis[i].esquerda, esquerda);
            strcpy(gramatica_cyk.regras_variaveis[i].direita[0], direita);
            strcpy(gramatica_cyk.regras_variaveis[i].direita[1], token);
            printf("\nRegra Variavel: %s > %s %s", gramatica_cyk.regras_variaveis[i].esquerda, gramatica_cyk.regras_variaveis[i].direita[0], gramatica_cyk.regras_variaveis[i].direita[1]);
            i++;
        }
        else
        {
            strcpy(gramatica_cyk.regras_terminais[j].esquerda, esquerda);
            strcpy(gramatica_cyk.regras_terminais[j].direita[0], direita);
            strcpy(gramatica_cyk.regras_terminais[j].direita[1], FIM);
            printf("\nRegra Terminal: %s > %s %s", gramatica_cyk.regras_terminais[j].esquerda, gramatica_cyk.regras_terminais[j].direita[0], gramatica_cyk.regras_terminais[j].direita[1]);
            j++;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    strcpy(gramatica_cyk.regras_terminais[j].esquerda, FIM);
    strcpy(gramatica_cyk.regras_variaveis[i].esquerda, FIM);

    return 1;
}

void zeraTudo(int tamanho_entrada_real){
    //base_tabela[i];
    int i, j, z;
    for (i = 0; i<tamanho_entrada_real; i++)
        strcpy(base_tabela[i], "0");

    for(i = 0; i < tamanho_entrada_real; i++)
    {
        for(j = 0; j < tamanho_entrada_real - i; j++)
        {
            for (z = 0; z < QTD_ANOT; z++)
                strcpy(tabela[i][j][z].variavel, "0");
        }
    }

}

int inicia_tabela(){
    int j = 0;
    int i = 0;
    int z = 0;

    //zeraTudo(tamanhoEntrada);

    while(strcmp(base_tabela[j], FIM))
    {
        while(strcmp(gramatica_cyk.regras_terminais[i].esquerda, FIM))
        {
            //printf("procurando %s  esq = %s  terminal = %s\n", base_tabela[j], gramatica_cyk.regras_terminais[i].esquerda, gramatica_cyk.regras_terminais[i].direita[0]);

            if(!strcmp(gramatica_cyk.regras_terminais[i].direita[0], base_tabela[j]))
            {
                //printf("\n>> ACHOU <<\n");
                strcpy(tabela[0][j][z].variavel, gramatica_cyk.regras_terminais[i].esquerda);
                tabela[0][j][z].vertical.linha = -1;
                tabela[0][j][z].vertical.coluna = j;

                z++;
            }
            i++;
        }

        strcpy(tabela[0][j][z].variavel, FIM);

        if(z == 0)
            return 0;

        i = 0;
        z = 0;
        j++;
    }

    return 1;
}

int coleta_entrada(){
    char entrada[TAM_IN];
    char* token;
    int i = 0;

    printf("Digite a entrada: ");
    fgets (entrada, TAM_IN, stdin);
   // printf("foi");

    token = strtok(entrada, " ");

    do
    {
        strcpy(base_tabela[i], token);
        i++;
    } while((token = strtok(NULL, " \n")) != NULL);

    strcpy(base_tabela[i], FIM);

    return i;
}

void imprime_tabela(int tamanho_entrada_real){
    int i = 0;
    int j = 0;
    int k = 0;
    int flag = 0;

    FILE *arqTabela;

    arqTabela = fopen("tabela CYK.txt", "w");


    for(i=0; i<QTD_ANOT; i++)
    {
        flag = 0;
        fprintf(arqTabela, "\nProfundidade %d da tabela: \n\n", i);
        for(j=tamanho_entrada_real-1; j>=0; j--)
        {
            fprintf(arqTabela,"%d: ", j);
            for(k=0; k<tamanho_entrada_real - j; k++)
            {
                if (strcmp(tabela[j][k][i].variavel, FIM) && strlen(tabela[j][k][i].variavel) > 0)
                {
                    fprintf(arqTabela,"%s (%d,%d,%d), (%d,%d,%d)\t", tabela[j][k][i].variavel, tabela[j][k][i].vertical.linha, tabela[j][k][i].vertical.coluna, tabela[j][k][i].vertical.profundidade,
                                                                                                                           tabela[j][k][i].diagonal.linha, tabela[j][k][i].diagonal.coluna, tabela[j][k][i].diagonal.profundidade);
                    flag = 1;
                }
                else
                    fprintf(arqTabela, "###################\t");
            }
            fprintf(arqTabela,"\n");
        }
        fprintf(arqTabela,"\n\n\n");
        if (flag == 0)
            break;
    }
}

void analiza_dois(int linha_1, int coluna_1, int linha_2, int coluna_2, POSICAO *onde_salvar){

    int i = 0;
    int j = 0;
    int k = 0;

    while(strcmp(tabela[linha_1][coluna_1][i].variavel, FIM))
    {
        while(strcmp(tabela[linha_2][coluna_2][j].variavel, FIM))
        {
            //printf("Realizando a busca com o simbolo 1 = %s\n", tabela[linha_1][coluna_1][i].variavel);
           // printf("Realizando a busca com o simbolo 2 = %s\n", tabela[linha_2][coluna_2][j].variavel);
            //printf("\n");
            while(strcmp(gramatica_cyk.regras_variaveis[k].esquerda, FIM))   // percorrendo as regras
            {
                //printf("\npercurando as regra, na pos %d\n", k);
                //printf("procurando %s  dir = %s\n", tabela[i][coluna][z].variavel, gramatica_cyk.regras_variaveis[k].direita[0]);
                //printf("procurando %s  dir = %s\n", tabela[linha-1-i][coluna+1+i][z].variavel, gramatica_cyk.regras_variaveis[k].direita[1]);

                if(!strcmp(gramatica_cyk.regras_variaveis[k].direita[0], tabela[linha_1][coluna_1][i].variavel) && !strcmp(gramatica_cyk.regras_variaveis[k].direita[1], tabela[linha_2][coluna_2][j].variavel))
                {
                    //printf("\n>> ACHOU a regra %s > %s %s e ta salvando em %d %d\n", gramatica_cyk.regras_variaveis[k].esquerda, gramatica_cyk.regras_variaveis[k].direita[0],
                           //gramatica_cyk.regras_variaveis[k].direita[1], onde_salvar->linha, onde_salvar->coluna);

                    strcpy(tabela[onde_salvar->linha][onde_salvar->coluna][onde_salvar->profundidade].variavel, gramatica_cyk.regras_variaveis[k].esquerda);

                    tabela[onde_salvar->linha][onde_salvar->coluna][onde_salvar->profundidade].vertical.linha = linha_1;
                    tabela[onde_salvar->linha][onde_salvar->coluna][onde_salvar->profundidade].vertical.coluna = coluna_1;
                    tabela[onde_salvar->linha][onde_salvar->coluna][onde_salvar->profundidade].vertical.profundidade = i;

                    tabela[onde_salvar->linha][onde_salvar->coluna][onde_salvar->profundidade].diagonal.linha = linha_2;
                    tabela[onde_salvar->linha][onde_salvar->coluna][onde_salvar->profundidade].diagonal.coluna = coluna_2;
                    tabela[onde_salvar->linha][onde_salvar->coluna][onde_salvar->profundidade].diagonal.profundidade = j;

                    onde_salvar->profundidade++;
                }
                k++;
            }
            k = 0;
            j++;
        }
        j = 0;
        i++;
    }
    //onde_salvar->profundidade = 0;

}

void anota(int linha, int coluna){
    int i = 0;
    POSICAO pos_celula_atual;

    pos_celula_atual.linha = linha;
    pos_celula_atual.coluna = coluna;
    pos_celula_atual.profundidade = 0;

    //printf("\n\n>>VIN DEOS: L: %d C: %d\n", linha, coluna);

    for(i=0; i<linha; i++)
    {
        //printf("\n\n>>MANDEI ANALIZAR DOIS: L1: %d C1: %d L2: %d C2: %d\n", i, coluna, linha - i - 1, coluna + i + 1);
        analiza_dois(i, coluna, linha - i - 1, coluna + i + 1, &pos_celula_atual);
    }

    strcpy(tabela[linha][coluna][pos_celula_atual.profundidade].variavel, FIM);
}

void bloco_de_anotacoes(int tamanho_entrada_real){
    int i;
    int j;

    for(i = 1; i < tamanho_entrada_real; i++)
    {
        for(j = 0; j < tamanho_entrada_real - i; j++)
        {
            //printf("\n anotando pos %d %d tam = %d\n", i, j, tamanho_entrada_real);
            anota(i,j);
        }
    }
}

void imprime_espacos(int n){
    int i;

    for(i=0; i<n; i++)
        fprintf(arqArvores,"\t");
}

void imprime_arvore(POSICAO roots, int contador){

    POSICAO direita;
    POSICAO esquerda;

    imprime_espacos(contador);

    fprintf(arqArvores,"%s -> {", tabela[roots.linha][roots.coluna][roots.profundidade].variavel);  // caminhamento dos brows



    if(tabela[roots.linha][roots.coluna][roots.profundidade].vertical.linha != -1)
    {
        fprintf(arqArvores,"\n");

        esquerda.linha = tabela[roots.linha][roots.coluna][roots.profundidade].vertical.linha;
        esquerda.coluna = tabela[roots.linha][roots.coluna][roots.profundidade].vertical.coluna;
        esquerda.profundidade = tabela[roots.linha][roots.coluna][roots.profundidade].vertical.profundidade;


        direita.linha = tabela[roots.linha][roots.coluna][roots.profundidade].diagonal.linha;
        direita.coluna = tabela[roots.linha][roots.coluna][roots.profundidade].diagonal.coluna;
        direita.profundidade = tabela[roots.linha][roots.coluna][roots.profundidade].diagonal.profundidade;


        imprime_arvore(esquerda, contador + 1);
        //imprime_espacos(contador);
        imprime_arvore(direita, contador + 1);
        imprime_espacos(contador);
        fprintf(arqArvores, "      }\n");
    }
    else
    {
         fprintf(arqArvores," %s }\n", base_tabela[roots.coluna]);
    }

}

int constroi_arvores(int tamanho_entrada_real){
    int cont = 0;
    int z = 0;
    //int nome = 1;
    //int nome_do_pai = 0;
    POSICAO pos_roots;
    int contador = 0;

    while (strcmp (tabela[tamanho_entrada_real-1][0][z].variavel, FIM))
    {
        if (!strcmp (tabela[tamanho_entrada_real-1][0][z].variavel, gramatica_cyk.inicial))
        {
            fprintf(arqArvores,"\nArvore %d:\n", cont+1);

            pos_roots.linha = tamanho_entrada_real - 1;
            //printf("\nfoi1");
            pos_roots.coluna = 0;
            pos_roots.profundidade = z;

            imprime_arvore(pos_roots, contador);

            cont++;
        }
        z++;
         fprintf(arqArvores,"\n\n\n");
    }

    return cont;
}




////////// MAIN:


int main(){

    int i=0;

    arqArvores = fopen("arvores de derivacao.txt", "w");

    int tamanho_entrada_real=1;
    //char continua;

    //tamanho_entrada_real = coleta_entrada();

    printf("Gramatica: \n\n\n");

    if(carregar_arquivo())
        printf("\n\nArquivo carregado com sucesso!\n\n\n");
    else
        printf("\nErro na abertura do arquivo.\n");

    tamanho_entrada_real = coleta_entrada();

    system("cls");

    fflush(stdin);

    if(!inicia_tabela())
        printf("A palavra nao foi aceita pela linguagem, pois um ou mais terminais nao pertence ao lexico da gramatica!\n\n");
    else
    {
        bloco_de_anotacoes(tamanho_entrada_real);

        imprime_tabela(tamanho_entrada_real);

        i = constroi_arvores(tamanho_entrada_real);

        if (i == 0)
        {
             printf("A palavra nao foi aceita pela linguagem!\n\n");
             printf("A tabela CYK esta no arquivo homonimo criado na  pasta do programa.");
        }
        else
        {
            if (i ==1)
                printf("A palavra de entrada foi aceita pela linguagem com UMA arvore de derivacao!\n\n");
            else
                printf("A palavra de entrada foi aceita pela linguagem com %d arvores de derivacao!\n\n", i);

            printf("A tabela CYK e as arvores de derivacao estao nos arquivos homonimos criados na  pasta do programa.");
        }
    }

    getch();

    system("cls");

    printf("\n\n\n\n\n\n\n\n\n\n\n\t\t     So long and thanks for all the fish!\n\n\n\n\n\n\n\n\n\n\n\n\n");

    getch();

    return 0;
}
