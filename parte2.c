#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA  600
#define RAIO_JOGADOR   20.0f
#define TOTAL_MOEDAS   15

typedef enum {
    MOEDA_BRONZE,
    MOEDA_PRATA,
    MOEDA_OURO,
    MOEDA_DIAMANTE // Adicionada nova raridade no enum
} TipoMoeda;

typedef struct {
    Vector2   pos;
    float     raio;
    TipoMoeda tipo;
    int       valor;
    bool      coletada;
    float     tempoColeta; // Campo para armazenar o tempo exato da coleta
} Moeda;

Color corDaMoeda(TipoMoeda tipo) {
    switch (tipo) {
        case MOEDA_BRONZE:   return (Color){160, 90, 40, 255};
        case MOEDA_PRATA:    return (Color){190, 190, 190, 255};
        case MOEDA_OURO:     return GOLD;
        case MOEDA_DIAMANTE: return SKYBLUE; // EXERCÍCIO 2: Cor personalizada para a moeda de diamante
        default:             return WHITE;
    }
}

int valorDaMoeda(TipoMoeda tipo) {
    switch (tipo) {
        case MOEDA_BRONZE:   return 5;
        case MOEDA_PRATA:    return 10;
        case MOEDA_OURO:     return 25;
        case MOEDA_DIAMANTE: return 50; //Definido o valor de 50 pontos para diamante
        default:             return 0;
    }
}

Moeda *criarMoedas(int quantidade) {
    Moeda *moedas = (Moeda *)malloc(quantidade * sizeof(Moeda));
    if (moedas == NULL) return NULL;

    for (int i = 0; i < quantidade; i++) {
        Moeda *m = (moedas + i);
        m->pos   = (Vector2){ GetRandomValue(30, LARGURA_JANELA - 30),
                              GetRandomValue(30, ALTURA_JANELA - 30) };
        m->raio  = 10.0f;
        
        // Regra de probabilidade para tornar a moeda de diamante mais rara (1 em 10)
        if (GetRandomValue(0, 9) == 0) { //
            m->tipo = MOEDA_DIAMANTE; //
        } else { //
            m->tipo = (TipoMoeda)GetRandomValue(MOEDA_BRONZE, MOEDA_OURO); //
        } //

        m->valor    = valorDaMoeda(m->tipo);
        m->coletada = false;
    }
    return moedas;
}

bool tentarColetar(Moeda *m, Vector2 posJogador, float raioJogador) {
    if (m->coletada) return false;

    float dx = m->pos.x - posJogador.x;
    float dy = m->pos.y - posJogador.y;
    float distancia = (dx * dx + dy * dy);
    float somaRaios = (m->raio + raioJogador) * (m->raio + raioJogador);

    if (distancia <= somaRaios) {
        m->coletada = true;
        m->tempoColeta = GetTime(); //Salva o tempo atual em segundos no momento da coleta
        return true;
    }
    return false;
}

void desenharMoeda(Moeda *m) {
    if (m->coletada) return;
    DrawCircleV(m->pos, m->raio, corDaMoeda(m->tipo));
}

int main(void) {
    srand((unsigned int)time(NULL));

    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "Atividade 2 - Simplificada");
    SetTargetFPS(60);

    Vector2 jogador = { LARGURA_JANELA / 2.0f, ALTURA_JANELA / 2.0f };
    int pontuacao = 0;

    Moeda *moedas = criarMoedas(TOTAL_MOEDAS);

    while (!WindowShouldClose()) {

        float vel = 250.0f * GetFrameTime();
        if (IsKeyDown(KEY_RIGHT)) jogador.x += vel;
        if (IsKeyDown(KEY_LEFT))  jogador.x -= vel;
        if (IsKeyDown(KEY_UP))    jogador.y -= vel;
        if (IsKeyDown(KEY_DOWN))  jogador.y += vel;

        for (int i = 0; i < TOTAL_MOEDAS; i++) {
            Moeda *m = (moedas + i);

            // Lógica para fazer a moeda reaparecer em nova posição após 3 segundos
            if (m->coletada && (GetTime() - m->tempoColeta >= 3.0f)) { //
                m->pos = (Vector2){ GetRandomValue(30, LARGURA_JANELA - 30), //
                                    GetRandomValue(30, ALTURA_JANELA - 30) }; //
                m->coletada = false; //
            } //

            if (tentarColetar(m, jogador, RAIO_JOGADOR)) {
                pontuacao += m->valor;
            }
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            for (int i = 0; i < TOTAL_MOEDAS; i++) {
                desenharMoeda(moedas + i);
            }

            DrawCircleV(jogador, RAIO_JOGADOR, BLUE);

            DrawText(TextFormat("Pontuacao: %d", pontuacao), 10, 10, 22, DARKGRAY);
            DrawText("Setas movem o jogador | ESC sai", 10, ALTURA_JANELA - 25, 16, GRAY);

        EndDrawing();
    }

    free(moedas);
    CloseWindow();
    return 0;
}