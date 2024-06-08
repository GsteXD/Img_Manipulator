#include <ncurses/ncurses.h> //inclui uma biblioteca externa para manipula��o do terminal.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

int** read_archive(char *nome_arquivo, int *linha, int *coluna, int *maxVal){
    char buffer[1001];
    char arquivo_leitura[100] = ""; 
    FILE *fp;

    strcpy(arquivo_leitura, nome_arquivo);
    strcat(arquivo_leitura, ".pgm");

    fp = fopen(arquivo_leitura, "r");

    if(!fp){
        printw("\nErro na abertura do arquivo. Aperte ENTER para sair");
        refresh();
        getch();
        exit(-1);
    }

    fgets(buffer, 1000, fp); //realiza a leitura da primeira linha

    if(strstr(buffer, "P2") == NULL){ //Verifica se o valor inicial � diferente de 'P2'
        printw("\nArquivo n�o compat�vel. Aperte ENTER para sair");
        refresh();
        getch();
        exit(-2); //Erro em caso verdadeiro
    }

    fscanf(fp,"%*[^\n]\n%d %d %d",coluna, linha, maxVal); /*Escaneia apenas os 3 primeiros tipos inteiros(decimais) do arquivo.
                                                    O fato de precisarmos disso � porque as linhas 3 e 4 do arquivo ".pgm" nos mostram
                                                    a largura, altura e o valor m�ximo de cor que aquele arquivo pode assumir*/

    int** mat; //cria��o de uma matriz de um ponteiro que aponta para ponteiros inteiros.
    mat = (int**) malloc(*linha * sizeof(int *)); //Informa que a matriz ir� alocar um ponteiro que aponta para ponteiros inteiros, o qual aponta para o valor 
                                                  //armazenado na vari�vel 'linhas' e aloca um espa�o mais ou menos igual a 4 bytes por linha da imagem.
    
    for(int i=0;i<*linha;i++){ //Aloca��o para preenchimento de colunas (isto servir� como um preenchimento da matriz).
        mat[i] = (int *) malloc(*coluna * sizeof(int)); //Para cada posi��o de 'linha', ser� alocado um espa�o equivalente ao que constar na vari�vel 'coluna'.
    }                                                   //O qual continuar� armazenando um espa�o equivalente a 4 bytes por coluna. 

    for(int i=0;i<*linha;i++){
        for(int j=0;j<*coluna;j++){
            fscanf(fp, "%d", &mat[i][j]); //Pega os valores presentes em 'fp' e os guarda em cada posi��o da matriz.
        }
    }
    fclose(fp); //Termina a leitura do arquivo

    printw("\n\nOpera��o realizada com sucesso!");
    refresh();
    Sleep(1000);
    clear(); //limpar o terminal

    return mat; //retorna os valores recebidos 
}

int write_archive(char *nome_arquivo, int **mat,int linha, int coluna, int maxVal){ //Cria um arquivo ASCII informando com valores modificados do arquivo original.
    FILE *fp;
    char arquivo_escrita[100] = "";

    if(mat == NULL){
        printw("Leia a imagem primeiro. Aperte ENTER para continuar");
        refresh();
        getch();
        return 1;
    }

    strcpy(arquivo_escrita, nome_arquivo);
    strcat(arquivo_escrita, "_editado.pgm");

    fp = fopen(arquivo_escrita, "w");

    fprintf(fp, "P2\n");
    fprintf(fp,"#Figura modificada\n");
    fprintf(fp,"%d %d\n", coluna, linha);
    fprintf(fp, "%d\n", maxVal);

    for(int i=0;i<linha;i++){
        for(int j=0;j<coluna;j++){
            fprintf(fp, "%d\n", mat[i][j]);
        }
    }

    clear();
    printw("Figura modificada com sucesso!");
    refresh();
    Sleep(1000);

    fclose(fp);
    return 1;
}

void Clarear(int **mat, int linha, int coluna, int maxVal){
    for(int i=0;i<linha;i++){
        for(int j=0;j<coluna;j++){
            mat[i][j] = (int)(mat[i][j] * 1.2);
            if(mat[i][j] > maxVal){
                mat[i][j] = maxVal;
            }
        }
    }
}

void Escurecer(int **mat, int linha, int coluna, int maxVal){
    for(int i=0;i<linha;i++){
        for(int j=0;j<coluna;j++){
            mat[i][j] = (int)(mat[i][j] * 0.4);
        }
    }
}

void aumentar_Contraste(int **mat, int linha, int coluna, int maxVal){
    for(int i=0;i<linha;i++){
        for(int j=0;j<coluna;j++){
            if(mat[i][j] < 127){
                mat[i][j] = (int)(mat[i][j] * 0.8);
            } else {
                mat[i][j] = (int)(mat[i][j] * 1.2);
                if(mat[i][j] > maxVal){
                    mat[i][j] = maxVal;
                }
            }
        }
    }
}

void diminuir_Contraste(int **mat, int linha, int coluna, int maxVal){
    for(int i=0;i<linha;i++){
        for(int j=0;j<coluna;j++){
            if(mat[i][j] < 127){
                mat[i][j] = (int)(mat[i][j] * 1.2);
            } else {
                mat[i][j] = (int)(mat[i][j] * 0.8);
            }
        }
    }
}

int moldura(int **mat, int linha, int coluna){
    int max_op = 2;
    int selected_op = 0;
    char* options[2] = {"Simples","Degrad�"};

    while(1){
        printw("Informe uma op��o\n\n");

        for(int i=0;i<max_op;i++){
            if(selected_op == i){
                attron(A_REVERSE);
            }
            printw("%s\n", options[i]);
            attroff(A_REVERSE);
        }

        int menu = getch();

        switch(menu){
            case KEY_UP:
                selected_op = (selected_op - 1 + max_op) % max_op;
            break;

            case KEY_DOWN:
                selected_op = (selected_op + 1) % max_op;
            break;

            case 10:
                if(selected_op == 1){
                    float degrade = 1;
                    for(int i=0;i<linha;i++){
                        for(int j=0;j<coluna;j++){
                            if(i<=3 || j<=3 || j >= coluna-4 || i >= linha-4){
                                mat[i][j] = (int)(mat[i][j] * degrade);
                            }
                        }
                        degrade = degrade * 0.99;
                    }
                } else {
                    for(int i=0;i<linha;i++){ //Basica
                        for(int j=0;j<coluna;j++){
                            if(i<=3 || j<=3 || j >= coluna-4 || i >= linha-4){
                                mat[i][j] = (int)(mat[i][j] * 0);
                            }
                        }
                    }
                }
                clear();
            return 0;   
        }
        clear();
        move(0, 0);       
    }
}

int menu(int **valor_matriz){
    int Max_options = 8;

    // Lista de op��es
    char* options[8] = { "Ler imagem", 
                         "Gravar imagem", 
                         "Aumentar brilho",
                         "Diminuir brilho",
                         "Aumentar contraste",
                         "Diminuir contraste",
                         "Desfocar",
                         "Fazer moldura",
                        };
    int selected_option = 0;

    // Loop principal
    while (1) {

        printw("MICRO PHOTOSHOP\n\nUse as teclas para cima e para baixo para navegar. Pressione 'q' para sair.\n\n");
        refresh(); //atualiza o buffer do terminal para receber novas informa��es futuras.

        // Exibe as op��es
            if(!valor_matriz){
                Max_options = 2;

                for(int i = 0; i < Max_options; ++i) {
                    if (i == selected_option) {
                        attron(A_REVERSE); //Destaca a op��o selecionada.
                    }
                    printw("%s\n", options[i]); //Imprime normalmente o resto das op��es.
                    attroff(A_REVERSE); //Desativa depois da impress�o para n�o ocasionar erros.
                }
            } 
            if(valor_matriz != NULL){
                for (int i = 0; i < Max_options; ++i) {
                    if (i == selected_option) {
                        attron(A_REVERSE); //Destaca a op��o selecionada.
                    }
                    printw("%s\n", options[i]); //Imprime normalmente o resto das op��es.
                    attroff(A_REVERSE); //Desativa depois da impress�o para n�o ocasionar erros.
                }
            }

        // L� a entrada do teclado
        int menu = getch();
        
        // Verifica a tecla pressionada
        switch(menu) {
            case 'q':
                endwin(); //finaliza o terminal
                exit(-3); 

            case KEY_UP: // Move a sele��o para cima
                selected_option = (selected_option - 1 + Max_options) % Max_options;
                break;

            case KEY_DOWN: // Move a sele��o para baixo
                selected_option = (selected_option + 1) % Max_options; //O m�dulo ao lado serve para determinar quando o ciclo deve retornar para a primeira posi��o.
                                                                       //Sendo o �nico momento cujo o qual ele � aplicado � no �ltimo elemento da lista, em que retorna o resto 0.
                break;

            case 10: //Executa a op��o sublinhada ao apertar ENTER
                clear();
                refresh();
                return selected_option;
        }   
        // Limpa a tela e posiciona o cursor no in�cio
        clear();
        move(0, 0);
    }
    
    // Nunca dever�amos chegar aqui, mas se chegar, h� a finaliza��o do terminal como forma de seguran�a.
    endwin();
    return 0;
}

int main(){
    initscr(); //incializa o terminal no modo ncurses.

    //Personaliza��o do terminal.
    cbreak(); //Permite o uso de sinalizadores dentro do terminal, como CTRL+C. Uma fun��o oposta a essa seria o raw().
    echo(); //N�o imprime os caracteres digitados pelo usu�rio.
    keypad(stdscr, TRUE); //Permite das setas do teclado para interatividade maior no terminal.
                          //stdscr = standart screen, definida como TRUE na fun��o.

    int linha=0, coluna=0, maxVal=0;
    int **matriz = NULL;

    char nome_arquivo[100];

    while(1){
        switch (menu(matriz)){
            case 0: //Leitura
                clear();
                printw("Informe o nome do arquivo(sem extens�o): ");
                refresh();
                getstr(nome_arquivo);

                matriz = read_archive(nome_arquivo, &linha, &coluna, &maxVal);
                break;

            case 1: //Escrita
                write_archive(nome_arquivo, matriz, linha, coluna, maxVal);
                break;

            case 2: //Aumentar Brilho
                Clarear(matriz, linha, coluna, maxVal);
                break;

            case 3: //Diminuir Brilho
                Escurecer(matriz, linha, coluna, maxVal);
                break;

            case 4:
                aumentar_Contraste(matriz, linha, coluna, maxVal);
                break;

            case 5:
                diminuir_Contraste(matriz, linha, coluna, maxVal);
                break;

            case 6:

                break;

            case 7:
                moldura(matriz, linha, coluna);
                break;

            default:
                break;
        }
    }
    return 0; 
} 
