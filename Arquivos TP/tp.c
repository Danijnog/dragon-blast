#include <allegro5/bitmap.h>
#include <allegro5/keyboard.h>
#include <allegro5/keycodes.h>
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

const float FPS = 110;  

const int SCREEN_W = 1110;
const int SCREEN_H = 700;

const int RAIO = 10;	
const int RAIO_I = 30;

const int VELOCIDADE_SPRITE = 5;
const int VELOCIDADE_TIRO = 6;
const int VELOCIDADE_INIMIGO = 3;
const int TEMPO_SUPER_TIRO = 150;

int score = 0;

const int NUM_INIMIGOS = 11;


// background
ALLEGRO_BITMAP *background;

typedef struct Bloco {
	int x, y;
	ALLEGRO_COLOR cor;
	int w, h;
	
} Bloco;

typedef struct Tiro {
	int x, y;
	int raio;
	int ativo;
	int poder;
	int cont;

} Tiro;

typedef struct Inimigo {
	int x, y;
	int raio;
	int vel_i;
	int ativo;
	ALLEGRO_COLOR cor;
	int imortal;
	
} Inimigo;

typedef struct Sprite {
	int altura;
	int largura;
	int pos_x;
	int pos_y;
	int current_frame_y;
	float frame;
	int dir_x, dir_y;
	ALLEGRO_BITMAP *imagem;
	int vel;
} Sprite;

void initGlobais() {
	background = al_load_bitmap("./assets/background.png");
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

void initTiro(Tiro *tiro, Sprite *sprite) {
	tiro->x = sprite->pos_x;
	tiro->y = sprite->pos_y;
	tiro->raio = RAIO;
	tiro->ativo = 0;
	tiro->poder = 0;
	tiro->cont = 0;
}

void initInimigo(Inimigo *inimigo, Bloco *bloco) {
	
	inimigo->x = SCREEN_W + 110 + rand()%190;
	inimigo->y = rand()%(SCREEN_H);
	inimigo->raio = RAIO_I + rand()%65;
	inimigo->vel_i = VELOCIDADE_INIMIGO;
	inimigo->ativo = 0;
	inimigo->imortal = 0;
	if(rand()%8 == 1)
	{
		inimigo->imortal = 1;
	}
	inimigo->cor = al_map_rgb(rand(), rand(), rand());
	
}

void initSprite(Sprite *sprite) {
	sprite->pos_x = 0;
	sprite->pos_y = 0;
	sprite->dir_x = 0;
	sprite->dir_y = 0;
	sprite->altura = 161;
	sprite->largura = 191;
	sprite->imagem = al_load_bitmap("./assets/dragon.png");
	sprite->current_frame_y = 161;
	sprite->frame = 0.f;
	sprite->vel = VELOCIDADE_SPRITE;
}

void desenhaCenario() {
	al_draw_bitmap(background, 0, 0, 0);
}

void desenhaBloco(Bloco bloco) {
	al_draw_filled_rectangle(bloco.x, bloco.y,
							bloco.x + bloco.w, bloco.y + bloco.h,
							bloco.cor);
							
}

void desenhaTiro(Tiro tiro) {
	al_draw_filled_circle(tiro.x, tiro.y, tiro.raio, al_map_rgb(0, 0, 255));
}

void desenhaSprite(Sprite sprite) {
	al_draw_bitmap_region(sprite.imagem, 191 * (int)sprite.frame,
	sprite.current_frame_y, sprite.largura, sprite.altura, sprite.pos_x, sprite.pos_y, 0); //sx = ponto inicial da imagem, sy = ponto inicial da imagem, dx = posicao inicial x do sprite na tela, dy = posicao inicial y do sprite na tela
}

void atualizaSprite(Sprite *sprite) {
	sprite->frame = sprite->frame + 0.02f;
	if(sprite->frame > 3) {
		sprite->frame -= 3;
	}
	sprite->pos_x = sprite->dir_x * sprite->vel; //atualização da posição do sprite quando o usuario tecla, alterando a variavel dir_x
	sprite->pos_y = sprite->dir_y * sprite->vel; //atualização da posição do sprite quando o usuario tecla, alterando a variavel dir_x

	if(sprite->pos_x + sprite->largura > SCREEN_W) {
		sprite->pos_x = SCREEN_W - sprite->largura;
	}

	if(sprite->pos_x < 0) {
		sprite->pos_x = 0;
	}

	if(sprite->pos_y + sprite->altura > SCREEN_H) {
		sprite->pos_y = SCREEN_H - sprite->altura;
	}

	if(sprite->pos_y < 0) {
		sprite->pos_y = 0;
	}
}


void atualizaBloco(Bloco *bloco) {
	bloco->x -= 1;
	
	if(bloco->x + bloco->w < 0)
		initBloco(bloco);
}

void atualizaBola(Tiro *tiro, Sprite *sprite, Bloco *bloco) {
	
	if(tiro->x > SCREEN_W) {
		initTiro(tiro, sprite);
	}
	
	if(tiro->ativo || tiro->poder)
		tiro->x += VELOCIDADE_TIRO;
	
	else
	{
		tiro->x = sprite->pos_x + sprite->largura;
		tiro->y = sprite->pos_y + sprite->altura/2;
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

int colisaoTiroInimigo(Tiro *tiro, Inimigo *inimigo, Bloco *bloco, Sprite *sprite) {
	
	if(tiro->poder)
	{
		int i;
		for(i=0; i<NUM_INIMIGOS; i++)
		{
			if(!inimigo[i].imortal)
			{
				int distancia;
				int cateto1;
				int cateto2;
				cateto1 = inimigo[i].x - tiro->x;
				cateto2 = inimigo[i].y - tiro->y;
				distancia = sqrt(cateto1 * cateto1 + cateto2 * cateto2);
				if(distancia < (inimigo[i].raio + tiro->raio) && inimigo[i].raio >= 80)
				{
					score = score + 2;
					initInimigo(&inimigo[i], bloco);
					return 1;
				}
				else if(distancia < (inimigo[i].raio + tiro->raio) && inimigo[i].raio < 80)
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
	else if(tiro->ativo)
	{
		int i;
		for(i=0; i<NUM_INIMIGOS; i++)
		{
			if(!inimigo[i].imortal)
			{
				int distancia;
				int cateto1;
				int cateto2;
				cateto1 = inimigo[i].x - tiro->x;
				cateto2 = inimigo[i].y - tiro->y;
				distancia = sqrt(cateto1 * cateto1 + cateto2 * cateto2);
				if(distancia < (inimigo[i].raio + tiro->raio) && inimigo[i].raio >= 80)
				{
					score = score + 2;
					initTiro(tiro, sprite);
					initInimigo(&inimigo[i], bloco);
					return 1;
				}
				else if(distancia < (inimigo[i].raio + tiro->raio) && inimigo[i].raio < 80)
				{
					score++;
					initTiro(tiro, sprite);
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


int colisaoSpriteBloco(Sprite *sprite, Bloco *bloco) {	

	if((sprite->pos_x + sprite->largura >= bloco->x) && //pos_x do sprite começa no 0
	(sprite->pos_x < bloco->x + bloco->w) && 
	(sprite->pos_y + sprite->altura >= bloco->y) && //pos_y do sprite começa tbm no 0
	(sprite->pos_y < bloco->y + bloco->h))
	{
		return 1;
	}
	else 
		return 0;
}

int colisaoSpriteInimigo(Sprite *sprite, Inimigo *inimigo) {
	if((sprite->pos_x + sprite->largura >= inimigo->x - inimigo->raio) &&
	(sprite->pos_x < inimigo->x + inimigo->raio) &&
	(sprite->pos_y + sprite->altura >= inimigo->y - inimigo->raio) &&
	(sprite->pos_y < inimigo->y + inimigo->raio))
	{
		return 1;
	}
	else 
		return 0;
}

int colisaoSpriteInimigos(Sprite *sprite, Inimigo *inimigo) {
	int i;
	for(i=0; i < NUM_INIMIGOS; i++)
	{
		if(colisaoSpriteInimigo(sprite, &inimigo[i]))
			return 1; //se eu fizer um else aqui dentro, não funciona direito para todos os inimigos
	}
	return 0;
}


int colisaoTiroBloco(Tiro *tiro, Bloco *bloco, Sprite *sprite) {
	
	if((tiro->x >= bloco->x) &&
	(tiro->x <= bloco->x + bloco->w)&&
	(tiro->y >= bloco->y) &&
	(tiro->y <= bloco->y + bloco->h))
	{
		initTiro(tiro, sprite);
		return 1;
	}
	return 0;
}

void colisaoInimigoBloco(Inimigo *inimigo, Bloco *bloco) {
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

	al_init_image_addon();
	al_install_keyboard();

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();
	al_init_ttf_addon();

	// setei o titulo da janela
	al_set_window_title(display, "Dragon Blast");

	// setei o icone da janela
	ALLEGRO_BITMAP *icon = al_load_bitmap("./assets/icon.png");
	al_set_display_icon(display, icon);

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
	
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("font.ttf", 32, 1);   
	if(size_32 == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}
	
	
	//avisa o allegro que agora eu quero modificar as propriedades da tela
	al_set_target_bitmap(al_get_backbuffer(display));

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

	Sprite sprite;
	initSprite(&sprite);
	
	Bloco bloco;
	initBloco(&bloco);
	
	Tiro tiro;
	initTiro(&tiro, &sprite);
	
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
			
			tiro.cont++;

			
			desenhaCenario();

			desenhaSprite(sprite);
			atualizaSprite(&sprite);
			
			atualizaBloco(&bloco);
			desenhaBloco(bloco);
			
			atualizaBola(&tiro, &sprite, &bloco);
			desenhaTiro(tiro);
			
			int i;
			liberaInimigo(&inimigo[i]);
			atualizaInimigo(&inimigo[i], &bloco);
			desenhaInimigo(&inimigo[i]);
			
			
			colisaoInimigoBloco(&inimigo[i], &bloco);
			colisaoInimigoInimigo(&inimigo[i]);
			colisaoTiroBloco(&tiro, &bloco, &sprite);
			
			
			if(colisaoTiroInimigo(&tiro, &inimigo[i], &bloco, &sprite))
			{
				sprintf(my_score, "Score: %d", score);
			}
			
			//imprime o texto armazenado em my_text na posicao x=40,y=20
			al_draw_text(size_32, al_map_rgb(255, 255, 255), SCREEN_W-1100, 20, 0, my_score);
			
			playing = !colisaoSpriteBloco(&sprite, &bloco) && !colisaoSpriteInimigos(&sprite, inimigo);
			
			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
		
		}
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}

		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch(ev.keyboard.keycode) {
				
				case ALLEGRO_KEY_W:
					sprite.dir_y -= 10;
					sprite.current_frame_y = 0;
					break;
				
				case ALLEGRO_KEY_S:
					sprite.dir_y += 10;
					sprite.current_frame_y = 161 * 2;
					break;
					
				case ALLEGRO_KEY_A:
					sprite.dir_x -= 10;
					sprite.current_frame_y = 161 * 3;
					break;
				
				case ALLEGRO_KEY_D:
					sprite.dir_x += 10;
					sprite.current_frame_y = 161;
					break;	
					
				case ALLEGRO_KEY_SPACE:
					tiro.cont = 0;
					break;
				
			}
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
			switch(ev.keyboard.keycode) {

				case ALLEGRO_KEY_SPACE:
					tiro.ativo = 1;
					if(tiro.cont < TEMPO_SUPER_TIRO)
					{
						tiro.poder = 0;
						tiro.raio = tiro.raio;
					}
					else
					{
						tiro.poder = 1;
						tiro.raio = tiro.raio * 2;
					}
					break;
			}
		}

	} //fim do while
	
	char my_text[20];	
	al_clear_to_color(al_map_rgb(230,240,250));
	
	sprintf(my_text, "Score: %d", score);
	al_draw_text(size_32, al_map_rgb(200, 0, 30), SCREEN_W/3.0, SCREEN_H/2.0, 0, my_text);
	int record;
	if(newRecord(score, &record)) 
	{
		al_draw_text(size_32, al_map_rgb(200, 20, 30), SCREEN_W/3.0, 100+SCREEN_H/2.0, 0, "NEW RECORD!");
	}
	else 
	{
		sprintf(my_text, "Record: %d", record);
		al_draw_text(size_32, al_map_rgb(0, 200, 30), SCREEN_W/3.0, 100+SCREEN_H/2.0, 0, my_text);
	}
	
	//reinicializa a tela
	al_flip_display();	
    al_rest(2);	


	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	al_destroy_bitmap(background);
	al_destroy_bitmap(sprite.imagem);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
   
	return 0;
}