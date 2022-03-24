#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>


const float FPS = 100;  

const int SCREEN_W = 960;
const int SCREEN_H = 540;

const int NAVE_W = 100;
const int NAVE_H = 50;
const int RAIO = 10;	
const int RAIO_I = 30;

const int VELOCIDADE_NAVE = 2;
const int VELOCIDADE_BOLA = 6;
const int VELOCIDADE_INIMIGO = 3;
const int TEMPO_SUPER_TIRO = 150;

int score = 0;


const int NUM_INIMIGOS = 11;


ALLEGRO_COLOR COR_CENARIO;

typedef struct Nave {
	
	int dir_x, dir_y;
	int x, y;
	int vel;
	ALLEGRO_COLOR cor;
	
} Nave;

typedef struct Bloco {
	int x, y;
	ALLEGRO_COLOR cor;
	int w, h;
	
} Bloco;

typedef struct Bola {
	int x, y;
	int raio;
	int ativo;
	int poder;
	int cont;
} Bola;

typedef struct Inimigo {
	
	int x, y;
	int raio;
	int vel_i;
	int ativo;
	ALLEGRO_COLOR cor;
	int imortal;
	
} Inimigo;

void initGlobais() {
	COR_CENARIO = al_map_rgb(rand()%75, rand()%75, rand()%75);
}

int newRecord(int score, int *record) {
	FILE *arq = fopen("recorde.txt", "r");
	*record = -1;
	if(arq != NULL) {
		fscanf(arq, "%d", record);
		fclose(arq);
	}
	if(*record < score ) {
		arq = fopen("recorde.txt", "w");
		fprintf(arq, "%d", score);
		fclose(arq);
		return 1;
	}
	return 0;
}

void initBloco(Bloco *bloco) {
	
	bloco->x = SCREEN_W + 170;
	bloco->y = rand()%(SCREEN_H/4);
	bloco->w = SCREEN_W;
	bloco->h = SCREEN_H/5 + rand()%(SCREEN_H/6);
	bloco->cor = al_map_rgb(rand(), rand(), rand());
}

void initNave(Nave *nave) {
	
	nave->x = 10 + NAVE_W;
	nave->y = SCREEN_H/2;
	nave->dir_x = 0;
	nave->dir_y = 0;
	nave->cor = al_map_rgb(200 + rand()%75, 200 + rand()%75, 200 + rand()%75);
	nave->vel = VELOCIDADE_NAVE;
}

void initBola(Bola *bola, Nave *nave) {
	bola->x = nave->x;
	bola->y = nave->y;
	bola->raio = RAIO;
	bola->ativo = 0;
	bola->poder = 0;
	bola->cont = 0;
}

void initInimigo(Inimigo *inimigo, Bloco *bloco) {
	
	inimigo->x = SCREEN_W + 110 + rand()%190;
	inimigo->y = rand()%(SCREEN_H);
	inimigo->raio = RAIO_I + rand()%65;
	inimigo->vel_i = VELOCIDADE_INIMIGO;
	inimigo->ativo = 0;
	inimigo->imortal = 0;
	if(rand()%2 == 1)
	{
		inimigo->imortal = 1;
	}
	inimigo->cor = al_map_rgb(rand(), rand(), rand());
	
}

void desenhaCenario() {
	
	al_clear_to_color(COR_CENARIO);
}

void desenhaNave(Nave nave) {
	al_draw_filled_triangle(nave.x, nave.y,
							nave.x - NAVE_W, nave.y - NAVE_H/2,
							nave.x - NAVE_W, nave.y + NAVE_H/2,
							nave.cor);
}

void desenhaBloco(Bloco bloco) {
	al_draw_filled_rectangle(bloco.x, bloco.y,
							bloco.x + bloco.w, bloco.y + bloco.h,
							bloco.cor);
							
}

void desenhaBola(Bola bola) {
	al_draw_filled_circle(bola.x, bola.y, bola.raio, al_map_rgb(0, 0, 255));
}


void atualizaBloco(Bloco *bloco) {
	bloco->x -= 1;
	
	if(bloco->x + bloco->w < 0)
		initBloco(bloco);
}

void atualizaNave(Nave *nave) {
	
	nave->y += nave->dir_y * nave->vel;
	nave->x += nave->dir_x * nave->vel;
	
	if(nave->y + NAVE_H/2 > SCREEN_H)
		nave->y = SCREEN_H - NAVE_H/2;
	
	if(nave->y - NAVE_H/2 < 0)
		nave->y = NAVE_H/2;
	
	if(nave->x > SCREEN_W)
		nave->x = SCREEN_W;
	
	if(nave->x - NAVE_W < 0)
		nave->x = NAVE_W;
}

void atualizaBola(Bola *bola, Nave *nave, Bloco *bloco) {
	
	if(bola->x > SCREEN_W) {
		initBola(bola, nave);
	}
	
	if(bola->ativo || bola->poder)
		bola->x += VELOCIDADE_BOLA;
	
	else
	{
		bola->x = nave->x;
		bola->y = nave->y;
	}
}

void liberaInimigo(Inimigo *inimigo) {
	int i;
	for(i=0; i<NUM_INIMIGOS; i++)
	{
		if(!inimigo[i].ativo)
		{
			if(rand()%500 == 0)
			{
				inimigo[i].x = SCREEN_W + 110 + rand()%240;
				inimigo[i].y = 30 + rand()%(SCREEN_H - 60);
				inimigo[i].ativo = 1;
				break;
			}
		}
	}
}

void atualizaInimigo(Inimigo *inimigo, Bloco *bloco) {
	int i;
	for(i=0; i<NUM_INIMIGOS; i++)
	{
		if(inimigo[i].ativo)
		{
			inimigo[i].x = inimigo[i].x - VELOCIDADE_INIMIGO;
		}
		
		if(inimigo[i].x + inimigo[i].raio < 0)
		{
			inimigo[i].ativo = 0;
			initInimigo(&inimigo[i], bloco);
		}
		if(inimigo[i].y - inimigo[i].raio < 0)
		{
			inimigo[i].y = inimigo[i].raio;
		}
		if(inimigo[i].y + inimigo[i].raio > SCREEN_H)
		{
			inimigo[i].y = SCREEN_H - inimigo[i].raio;
		}
	}
}

void desenhaInimigo(Inimigo *inimigo) {
	int i;
	for(i=0; i<NUM_INIMIGOS; i++)
	{
		if(inimigo[i].ativo)
		{
			al_draw_filled_circle(inimigo[i].x, inimigo[i].y, inimigo[i].raio, inimigo[i].cor);
		}
	}
}

int colisaoBolaInimigo(Bola *bola, Inimigo *inimigo, Bloco *bloco, Nave *nave) {
	
	if(bola->poder)
	{
		int i;
		for(i=0; i<NUM_INIMIGOS; i++)
		{
			if(!inimigo[i].imortal)
			{
				int distancia;
				int cateto1;
				int cateto2;
				cateto1 = inimigo[i].x - bola->x;
				cateto2 = inimigo[i].y - bola->y;
				distancia = sqrt(cateto1 * cateto1 + cateto2 * cateto2);
				if(distancia < (inimigo[i].raio + bola->raio) && inimigo[i].raio >= 80)
				{
					score = score + 2;
					initInimigo(&inimigo[i], bloco);
					return 1;
				}
				else if(distancia < (inimigo[i].raio + bola->raio) && inimigo[i].raio < 80)
				{
					score++;
					initInimigo(&inimigo[i], bloco);
					return 1;
				}
			}
			else if(inimigo[i].imortal)
				inimigo[i].cor = al_map_rgb(255, 255, 255);
					
		}
		
	}
	else if(bola->ativo)
	{
		int i;
		for(i=0; i<NUM_INIMIGOS; i++)
		{
			if(!inimigo[i].imortal)
			{
				int distancia;
				int cateto1;
				int cateto2;
				cateto1 = inimigo[i].x - bola->x;
				cateto2 = inimigo[i].y - bola->y;
				distancia = sqrt(cateto1 * cateto1 + cateto2 * cateto2);
				if(distancia < (inimigo[i].raio + bola->raio) && inimigo[i].raio >= 80)
				{
					score = score + 2;
					initBola(bola, nave);
					initInimigo(&inimigo[i], bloco);
					return 1;
				}
				else if(distancia < (inimigo[i].raio + bola->raio) && inimigo[i].raio < 80)
				{
					score++;
					initBola(bola, nave);
					initInimigo(&inimigo[i], bloco);
					return 1;
				}
			}
			else if(inimigo[i].imortal)
				inimigo[i].cor = al_map_rgb(255, 255, 255);
				
		}
	}
	
	return 0;
}


int colisaoNaveBloco(Nave nave, Bloco bloco) {
   
   if(nave.x - NAVE_W > bloco.x + bloco.w)
	   return 0;
   if(nave.x < bloco.x)
	   return 0;
   if(nave.y - NAVE_H/2 > bloco.y + bloco.h)
	   return 0;
   if(nave.y + NAVE_H/2 < bloco.y)
	   return 0;
   else 
	   return 1;
}

int colisaoBolaBloco(Bola *bola, Bloco *bloco, Nave *nave) {
	
	if((bola->x >= bloco->x) &&
	(bola->x <= bloco->x + bloco->w)&&
	(bola->y >= bloco->y) &&
	(bola->y <= bloco->y + bloco->h))
	{
		initBola(bola, nave);
		return 1;
	}
	return 0;
}
 
int colisaoNaveInimigo(Nave *nave, Inimigo *inimigo) {

	if(nave->x - NAVE_W >= inimigo->x + inimigo->raio)
		return 0;
	else if(nave->x + RAIO + 2 <= inimigo->x - inimigo->raio)
		return 0;
	else if(nave->y - NAVE_H/2 >= inimigo->y + inimigo->raio)
		return 0;
	else if(nave->y + NAVE_H/2 <= inimigo->y - inimigo->raio)
		return 0;
	
	return 1;

}

int colisaoNaveInimigos(Nave *nave, Inimigo *inimigos)
{
	int i;
	for(i=0; i<NUM_INIMIGOS; i++)
	{
		if(colisaoNaveInimigo(nave,&inimigos[i]))
			return 1;
	}
	return 0;
}

int colisaoInimigoBloco(Inimigo *inimigo, Bloco *bloco) {
	int i;
	for(i=0; i<NUM_INIMIGOS; i++)
	{
		if(inimigo[i].ativo)
		{
			if((inimigo[i].x + inimigo[i].raio > bloco->x) &&
			(inimigo[i].x - inimigo[i].raio < bloco->x + bloco->w)&&
			(inimigo[i].y + inimigo[i].raio > bloco->y) &&
			(inimigo[i].y - inimigo[i].raio < bloco->y + bloco->h))
			{
				inimigo[i].ativo = 0;
				initInimigo(&inimigo[i], bloco);
			}
		}
	}
}

void colisaoInimigoInimigo(Inimigo *inimigo) {
	int i;
	int j;
	for(i=0; i<NUM_INIMIGOS; i++)
	{
		if(inimigo[i].ativo)
		{
			for(j=0; j<NUM_INIMIGOS; j++)
			{
				if(inimigo[j].ativo)
				{
					if(i != j)
					{
						int distancia;
						distancia = sqrt((pow(inimigo[i].x - inimigo[j].x, 2)) + (pow(inimigo[i].y - inimigo[j].y, 2)));
						if(distancia < inimigo[i].raio + inimigo[j].raio)
						{
							inimigo[i].ativo = 0;
							inimigo[j].ativo = 0;
						}
					}
				}
			}
		}
	}
}

int main(int argc, char **argv){
	
	srand(time(NULL));
	
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

	char my_score[20] = "Score: 0";
   
	//----------------------- rotinas de inicializacao ---------------------------------------
    
	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
	
	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}
   
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}
 
	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
	
	//instala o mouse
	if(!al_install_mouse()) {
		fprintf(stderr, "failed to initialize mouse!\n");
		return -1;
	}

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
	
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);   
	if(size_32 == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}
	
	
	//avisa o allegro que agora eu quero modificar as propriedades da tela
	al_set_target_bitmap(al_get_backbuffer(display));
	//colore a tela de preto (rgb(0,0,0))
	al_clear_to_color(COR_CENARIO);   

 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		return -1;
	}
     
	 
	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	//registra na fila os eventos de mouse (ex: clicar em um botao do mouse)
	al_register_event_source(event_queue, al_get_mouse_event_source());  
	
	initGlobais();
	
	Nave nave;
	initNave(&nave);
	
	Bloco bloco;
	initBloco(&bloco);
	
	Bola bola;
	initBola(&bola, &nave);
	
	Inimigo inimigo[NUM_INIMIGOS];
	int i;
	for(i=0; i<NUM_INIMIGOS; i++)
	{
		initInimigo(&inimigo[i], &bloco);
	}

	//inicia o temporizador
	al_start_timer(timer);
	
	int playing = 1;
	while(playing) {
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {
			
			bola.cont++;
			
			desenhaCenario();
			
			atualizaNave(&nave);
			desenhaNave(nave);
			
			atualizaBloco(&bloco);
			desenhaBloco(bloco);
			
			atualizaBola(&bola, &nave, &bloco);
			desenhaBola(bola);
			
			int i;
			liberaInimigo(&inimigo[i]);
			atualizaInimigo(&inimigo[i], &bloco);
			desenhaInimigo(&inimigo[i]);
			
			
			colisaoInimigoBloco(&inimigo[i], &bloco);
			colisaoInimigoInimigo(&inimigo[i]);
			colisaoBolaBloco(&bola, &bloco, &nave);
			
			if(colisaoBolaInimigo(&bola, &inimigo[i], &bloco, &nave))
			{
				sprintf(my_score, "Score: %d", score);
			}
			
			//imprime o texto armazenado em my_text na posicao x=40,y=20
			al_draw_text(size_32, al_map_rgb(255, 0, 0), SCREEN_W-940, 20, 0, my_score);
			
			playing = !colisaoNaveBloco(nave, bloco) && !colisaoNaveInimigos(&nave, inimigo);
			
			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
		
		}
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}
		//se o tipo de evento for um clique de mouse
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch(ev.keyboard.keycode) {
				
				case ALLEGRO_KEY_W:
					nave.dir_y--;
					break;
				
				case ALLEGRO_KEY_S:
					nave.dir_y++;
					break;
					
				case ALLEGRO_KEY_A:
					nave.dir_x--;
					break;
				
				case ALLEGRO_KEY_D:
					nave.dir_x++;
					break;	
					
				case ALLEGRO_KEY_SPACE:
					bola.cont = 0;
					break;
				
			}
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
			switch(ev.keyboard.keycode) {
				
				case ALLEGRO_KEY_W:
					nave.dir_y++;
					break;
				
				case ALLEGRO_KEY_S:
					nave.dir_y--;
					break;
					
				case ALLEGRO_KEY_A:
					nave.dir_x++;
					break;
				
				case ALLEGRO_KEY_D:
					nave.dir_x--;
					break;
					
				case ALLEGRO_KEY_SPACE:
					bola.ativo = 1;
					
					if(bola.cont >= TEMPO_SUPER_TIRO)
					{
						bola.poder = 1;
						bola.raio = bola.raio * 2;
					}
					break;
			}
		}

	} //fim do while
	
	char my_text[20];	
	al_clear_to_color(al_map_rgb(230,240,250));
	
	sprintf(my_text, "Score: %d", score);
	al_draw_text(size_32, al_map_rgb(200, 0, 30), SCREEN_W/3, SCREEN_H/2, 0, my_text);
	int record;
	if(newRecord(score, &record)) 
	{
		al_draw_text(size_32, al_map_rgb(200, 20, 30), SCREEN_W/3, 100+SCREEN_H/2, 0, "NEW RECORD!");
	}
	else 
	{
		sprintf(my_text, "Record: %d", record);
		al_draw_text(size_32, al_map_rgb(0, 200, 30), SCREEN_W/3, 100+SCREEN_H/2, 0, my_text);
	}
	
	//reinicializa a tela
	al_flip_display();	
    al_rest(2);		
   
   
	

	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	
 
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
   
	return 0;
}