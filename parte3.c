#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA  600
#define RAIO_JOGADOR   20.0f
#define TOTAL_ITENS    10

typedef struct {
    int absorcao;
} DadosEscudo;

typedef struct {
    float dano;
    int alcance;
} DadosArma;

typedef struct {
    int cura;
} DadosPocao;

typedef union {
    DadosArma   arma;
    DadosPocao  pocao;
    DadosEscudo escudo;
} DadosItem;

typedef enum {
    ITEM_ARMA,
    ITEM_POCAO,
    ITEM_ESCUDO
} TipoItem;

typedef struct {
    Vector2   pos;
    float     raio;
    TipoItem  tipo;
    DadosItem dados;
    bool      coletado;
} Item;

typedef struct {
    Vector2 pos;
    int vida;
    float dano;
    int armadura;
} Jogador;

Item *criarItens(int quantidade) {
    Item *itens = (Item *)malloc(quantidade * sizeof(Item));
    if (itens == NULL) return NULL;

    for (int i = 0; i < quantidade; i++) {
        Item *it = (itens + i);
        it->pos = (Vector2){ GetRandomValue(30, LARGURA_JANELA - 30),
                             GetRandomValue(30, ALTURA_JANELA - 30) };
        it->raio = 12.0f;
        it->coletado = false;
        
        it->tipo = (TipoItem)GetRandomValue(ITEM_ARMA, ITEM_ESCUDO);

        if (it->tipo == ITEM_ARMA) {
            it->dados.arma.dano = (float)GetRandomValue(2, 8);
            it->dados.arma.alcance = GetRandomValue(1, 3);
        } else if (it->tipo == ITEM_POCAO) {
            if (GetRandomValue(0, 9) < 3) {
                it->dados.pocao.cura = -GetRandomValue(10, 25);
            } else {
                it->dados.pocao.cura = GetRandomValue(10, 30);
            }
        } else if (it->tipo == ITEM_ESCUDO) {
            it->dados.escudo.absorcao = GetRandomValue(5, 15);
        }
    }
    return itens;
}

void aplicarItem(Jogador *j, Item *item) {
    if (item->coletado) return;

    switch (item->tipo) {
        case ITEM_ARMA:
            j->dano += item->dados.arma.dano;
            break;

        case ITEM_POCAO:
            j->vida += item->dados.pocao.cura;
            if (j->vida < 0) j->vida = 0;
            break;

        case ITEM_ESCUDO:
            j->armadura += item->dados.escudo.absorcao;
            break;
    }
    item->coletado = true;
}

void desenharItem(Item *item) {
    if (item->coletado) return;

    Color cor = WHITE;
    if (item->tipo == ITEM_ARMA) {
        cor = RED;
    } else if (item->tipo == ITEM_POCAO) {
        cor = (item->dados.pocao.cura < 0) ? PURPLE : GREEN;
    } else if (item->tipo == ITEM_ESCUDO) {
        cor = DARKBLUE;
    }

    DrawCircleV(item->pos, item->raio, cor);
}

bool testarColisao(Vector2 p1, float r1, Vector2 p2, float r2) {
    float dx = p1.x - p2.x;
    float dy = p1.y - p2.y;
    return (dx * dx + dy * dy) <= (r1 + r2) * (r1 + r2);
}

int main(void) {
    srand((unsigned int)time(NULL));

    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "Atividade 3 - Unions e Enums");
    SetTargetFPS(60);

    Jogador jogador = {
        .pos = { LARGURA_JANELA / 2.0f, ALTURA_JANELA / 2.0f },
        .vida = 50,
        .dano = 10.0f,
        .armadura = 0
    };

    Item *itens = criarItens(TOTAL_ITENS);

    while (!WindowShouldClose()) {

        float vel = 250.0f * GetFrameTime();
        if (IsKeyDown(KEY_RIGHT)) jogador.pos.x += vel;
        if (IsKeyDown(KEY_LEFT))  jogador.pos.x -= vel;
        if (IsKeyDown(KEY_UP))    jogador.pos.y -= vel;
        if (IsKeyDown(KEY_DOWN))  jogador.pos.y += vel;

        for (int i = 0; i < TOTAL_ITENS; i++) {
            Item *it = (itens + i);
            if (!it->coletado && testarColisao(jogador.pos, RAIO_JOGADOR, it->pos, it->raio)) {
                aplicarItem(&jogador, it);
            }
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            for (int i = 0; i < TOTAL_ITENS; i++) {
                desenharItem(itens + i);
            }

            DrawCircleV(jogador.pos, RAIO_JOGADOR, BLUE);

            DrawText(TextFormat("Vida: %d | Dano: %.1f | Armadura: %d", jogador.vida, jogador.dano, jogador.armadura), 10, 10, 20, DARKGRAY);
            DrawText("Vermelho: Arma | Verde: Cura | Roxo: Veneno | Azul Escuro: Escudo", 10, ALTURA_JANELA - 25, 16, GRAY);

        EndDrawing();
    }

    free(itens);
    CloseWindow();
    return 0;
}