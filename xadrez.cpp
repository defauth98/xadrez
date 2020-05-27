#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#define WIDTH 640
#define HEIGTH 480


SDL_Window * window;
SDL_Renderer * render;
SDL_Texture *texture = NULL;
SDL_Surface *IMG_Load(const char *file);   
SDL_Event evento;
    
// Matriz do jogo de xadrez
// T - Torre preta
// C - Cavalo preto
// B - Bispo preto
// Q - Rainha preta
// K - Rei Preto
// P - Peao preto

// t - Torre branca
// c - Cavalo branco
// b - Bispo branco
// q - Rainha branca
// k - Rei branco
// p - Peao branco

// Matriz principal do jogo
char xadrez[8][8] = {
    {'T','C','B','Q','K','B','C','T'},
    {'P','P','P','P','P','P','P','P'},
    {' ',' ',' ',' ',' ',' ',' ',' ',},
    {' ',' ',' ',' ',' ',' ',' ',' ',},
    {' ',' ',' ',' ',' ',' ',' ',' ',},
    {' ',' ',' ',' ',' ',' ',' ',' ',},
    {'p','p','p','p','p','p','p','p'},
    {'t','c','b','q','k','b','c','t'}
};

//Vetor com as imagens 
SDL_Texture * imagens[12];

//Matriz com os nomes das imagens
char nomesImagens [][50] = {"bb.png", "bp.png", "cb.png","cp.png","kb.png", "kp.png","rb.png","rp.png","pb.png","pp.png","tb.png","tp.png"};

//Vetor com a letra da peça na mesma sequencia do vetor com o nome
char pecas[] = {'b', 'B', 'c', 'C', 'k', 'K', 'q', 'Q', 'p','P','t', 'T'};

int linhaOrigem = -1;
int colunaOrigem = -1;

//Funcao para mover a peca
int moverPeca(int linhaOrigem, int colunaOrigem, int linhaDestino, int colunaDestino){
	int mover = 0;
	char peca;

	//Variavel para armazenar o deslocamento vertical
	int desloVertical = abs(linhaDestino - linhaOrigem);
	int desloHorizontal = abs(colunaDestino - colunaOrigem);
	//Verifica se a linha e a coluna de origem esta entre 0 e 8
	if((linhaOrigem >= 0 && linhaOrigem < 8 && colunaOrigem >= 0 && colunaOrigem < 8)
	&& (linhaDestino >= 0 && linhaDestino < 8 && colunaDestino >= 0 && colunaDestino < 8) && (
    (desloVertical + desloHorizontal) != 0
  )
	){
		//Variavell peca na matriz xadrez na linha e coluna de origem
		peca = xadrez[linhaOrigem][colunaOrigem];

		//Testa o movimento da torre
		//Verifica se o deslocamento vertical ou deslocamento horizontal e igual a 0, por que se
		// caso for o movimento nao sera em diagona e vai ser permito fazer o movimento
		if((peca == 'T' || peca == 't') && (desloVertical == 0 || desloHorizontal == 0)){
			mover = 1;
		}

		//Testa o movimento do bispo
		//Verifica se o deslocamento horizontal e vertical s�o igual, se caso for o movimento
		//Vai ser em diagonal, ou seja, � permitido o bispo fazer esse movimento
		if((peca == 'B' || peca == 'b') && (desloVertical ==  desloHorizontal)){
			mover = 1;
		}

		//Testa o movimento do cavalo
		//Verifica se o deslocamento horizontal � igual a 1 ou 2, e o deslocamento vertial � igual
		//a 1 ou 2
		if((peca == 'C' || peca == 'c') && ((desloVertical == 1 && desloHorizontal == 2 ) || (desloVertical == 2 && desloHorizontal == 1))){
		  mover = 1;
		}

		//Testa o movimento da rainha
		//Verifica usando a mesmas l�gicas usadas para o bispo e torre
		if((peca == 'Q' || peca == 'q') && ((desloVertical == desloHorizontal ) || (desloVertical == 0) || (desloHorizontal == 0))){
		  mover = 1;
		}

		//Testa o movimento do rei
		//Verifica se o deslocamento vertical ou horizontal � igual 1 ou 0
		if((peca == 'K' || peca == 'k') && ((desloVertical >= 0 && desloVertical <=1 ) && (desloHorizontal >= 0 && desloHorizontal <= 1))){
		  mover = 1;
		}


		//Testa o movimento do peao branco
		//Calcula o deslocamento e verifica se n�o houve descolamento horizontal
		if((peca == 'P') && ((linhaDestino - linhaOrigem) == 1) && (desloHorizontal == 0)){
			mover = 1;
		}

		//Testa o movimento do peao preto
		if((peca == 'p') && ((linhaOrigem - linhaDestino) == 1) && (desloHorizontal == 0)){
			mover = 1;
		}

		if(mover){
		  // Copia o valor da posicao de origem para a posicao de destino
		  xadrez[linhaDestino][colunaDestino] = xadrez[linhaOrigem][colunaOrigem];

		  //Coloca ' ' na linha e coluna de origem da matriz xadrez
		  xadrez[linhaOrigem][colunaOrigem] = ' ';
		} else{
			return 9;
		}
		return 1;
	}else {
		return 0;
	}
}
//Funcao que vai carregar as imagens
int iniciarImagens(){
  if(IMG_Init(IMG_INIT_PNG) == IMG_INIT_PNG){
    int i;
    for(i = 0; i < 12;i++){
      char nomeImg[50] = "imagens/";
      strcat(nomeImg,nomesImagens[i]);
      
      printf("\nCarregando imagem -> %s", nomeImg);
      SDL_Surface * surImage = IMG_Load(nomeImg);
      imagens[i] = SDL_CreateTextureFromSurface(render, surImage);
    	}
    }
    else{
      printf("Nao foi possivel iniciar as imagens");
    } 
}

// Função que vai no vetor de peçar para pegar o nome da imagem que vai ser renderizada
SDL_Texture * imagemFromPeca(char peca){
  int i;
  for(i =0; i<12;i++){
    if(pecas[i] == peca){
      return imagens[i];
    }
  }
}

//Funcao imprime o tabuleiro e as peças
int colocarPecas(){
  //Calcula a altura e largura de cada quadrado na tela
  int w = WIDTH / 8;
  int h = HEIGTH / 8;
  int linha, coluna,x,y;

  for(linha = 0; linha < 8;linha++){
    for(coluna = 0; coluna < 8; coluna++){
      if((coluna + linha) % 2 == 0){
        SDL_SetRenderDrawColor(render, 98.8,78.8,63,255);
      }else{
        SDL_SetRenderDrawColor(render,66.7,47.5,33.7,255);
      }
		  //Valor de x e y para posicionar as imagens
		  x = coluna *  w;
	  	y = linha * h;

		  //Retangulo que ira posicionar a imagem
      SDL_Rect r = {x,y,w,h};
      SDL_RenderFillRect(render, &r);

      //Pega a peça no seu devido lugar na matriz principal do jogo
      char p = xadrez[linha][coluna];
      if(p != ' '){
        int wOri, hOri;
        SDL_Texture * img = imagemFromPeca(xadrez[linha][coluna]);
        SDL_QueryTexture(img, NULL, NULL,&wOri,&hOri);
        SDL_Rect rOrig = {0,0,wOri, hOri};
        SDL_RenderCopy(render, img, &rOrig, &r);
        }
      
      if(linhaOrigem == linha && colunaOrigem == coluna){
        SDL_SetRenderDrawColor(render, 0,0,255,255);
        SDL_RenderDrawRect(render, &r);
        }
      }
  }

  SDL_RenderPresent(render);
}

// Função que inicia o SDL e cria uma janela de 640 x 480
int imprimirTela(){
  // Inicia o video
  SDL_Init(SDL_INIT_VIDEO); 

  // Inicia todas as funções do SDL
  if(SDL_Init(SDL_INIT_EVERYTHING) >= 0){
    printf("\n\nInicializando o SDL");
    window = SDL_CreateWindow("Jogo de Xadrez",50,50, WIDTH, HEIGTH, SDL_WINDOW_SHOWN);

    if(window != NULL){
      printf("\nJanela inicializada com sucesso\n");
      render =  SDL_CreateRenderer(window, -1, 0);
     
      if(render != NULL){
      }else{
        printf("\nErro ao criar o render");
      }
    }else{
      printf("\nErro ao criar a janela");
    }

  }else{
      printf("\nErro ao iniciar o SDL \n");
  }
}

//Função que vai capturar o evento do clique do mouse para mover a peça
int capturaEventos(){
  int e = SDL_PollEvent(&evento);
  if(evento.type == SDL_QUIT){
    exit(0);
  }else if(evento.type == SDL_MOUSEMOTION){
   // printf("\nX : %d", evento.motion.x);
  }
  else if(evento.type == SDL_MOUSEBUTTONDOWN){
    int w = WIDTH/8;
    int h = HEIGTH/8;

    int coluna = evento.motion.x / w;
    int linha = evento.motion.y / h;
  
    if(linhaOrigem == -1){
      linhaOrigem = linha;
      colunaOrigem = coluna;
    }else{
      moverPeca(linhaOrigem, colunaOrigem, linha, coluna);
      linhaOrigem = -1;
      colunaOrigem = -1;
    }
    printf("\nLinha: %d , Coluna: %d", linha, coluna);

  }
}

int main(int argc)
{
  imprimirTela(); 
  iniciarImagens();

  //While principal do jogo
	while(1){
    colocarPecas();
    capturaEventos();
    SDL_Delay(5);
  }  
	  
  return 0;
}

