/*
 * This file is part of "2º Projeto de Introdução à Computação 2017/2018"
 * (2oPIC1718).
 *
 * Complete!
 *
 * 2oPIC1718 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 2oPIC1718 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 2oPIC1718. If not, see <http://www.gnu.org/licenses/>.
 * */

/**
 * @file
 * This file is an example of: a) how to use the API defined in the
 * showworld.h header (mandatory for the project); and, b) how to use the
 * concrete simple implementation of the API (provided by the
 * showworld_simple.c file).
 *
 * @author Leandro Brás, Hugo Martins, Diana Noia.
 * @date 2018
 * @copyright [GNU General Public License version 3 (GPLv3)](http://www.gnu.org/licenses/gpl.html)
 **/
#include "showworld.h"
#include "agent.h"
#include "world.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "ini.h"

/**
 * Struct de agents usada para guardar todas as informações sobre 
 * todos os agents.
 */
struct agentID {
    int x;
    int y;
    unsigned int id;
    AGENT_TYPE type;
    unsigned int ply;
};

/**
 * Struct que recolhe as variaveis do ficheiro ini.h
 */
typedef struct {
    int xdim;
    int ydim;
    int nzombies;
    int nhumans;
    int nzplayers;
    int nhplayers;
    int maxturns;
} configuration;

/**
 * Função que cria um movimento toroidal na grelha do jogo.
 * Ex: Se um jogador se mover para fora da grelha é colocado do outro lado.
 * @param x
 * @param y
 * @param toro
 * @param na
 */
void toroidal(int *x, int *y, int *toro, int *na, configuration config);

/**
 * Função que funciona como de apoio ha toroidal que serve para descobrir ao certo
 *  para que lado o agent se deve mexer. 
 * @param xN
 * @param yN
 * @param xNovo
 * @param yNovo
 * @param movein
 * @param movedir
 */
void MoveToroidal(int xN, int yN, int xNovo, int yNovo, int *movein, int *movedir, configuration config);

/**
 * Função que verifica a posição de um agente de tipo contrario ao que esta
 * ativo neste turno.
 * @param x
 * @param y
 * @param xNovo
 * @param yNovo
 * @param typeA
 * @param agTypeAnt
 * @param toro
 * @param w
 * @param na
 * @param apagar
 * @param agents
 * @param nagents
 */
void distancia(int *x, int *y, int xNovo, int yNovo, int typeA,
        AGENT_TYPE *agTypeAnt, int toro, WORLD *w, int na, int *apagar,
        struct agentID *agents, int nagents, configuration config);

/* This function is an implementation of the definition provided by the
 * ::get_agent_info_at() function pointer. It only works for AGENT and WORLD
 * example structures defined in this file. */
unsigned int example_get_ag_info(void *w, unsigned int x, unsigned int y);

/**
 * Função que serve para a leitura do ficheiro ini.h 
 * @param user
 * @param section
 * @param name
 * @param value
 * @return 
 */
static int handler(void* user, const char* section, const char* name,
        const char* value);


int na1;




/**
 * This `main` function is only an example of: a) how to use the API defined in
 * the showworld.h header (mandatory for the project); and, b) how to use the
 * concrete simple implementation of the API (provided in the
 * showworld_simple.c file).
 *
 * @return Always zero.
 * */


int main(int argc, char **argv) {
    /**Inicialização do world*/
    WORLD *w = NULL;

    configuration config;
    
    /**Inicializar a struct para ler o ficheiro ini*/
    if (ini_parse("config.ini", handler, &config) < 0) {
        printf("N�o foi poss�vel encontrar o ficheiro 'config.ini'\n");
        return 1;
    }
    
    /**Numero total de agentes "Humanos + Zombies"*/
    int nagents = config.nhumans + config.nzombies;
    
    /**Inicialização da struct de agentes*/
    struct agentID agents[nagents];
    
    for (int i = 0; i < nagents; i++) {
        agents[i].x = 0; 
        agents[i].y = 0; 
        agents[i].id = 0; 
        agents[i].type = 0; 
        agents[i].ply = 0; 
    }

    /**
     * Criamos o "showworld", passando-lhe o "x , y" da grelha e a info
     * do agente, chamando tambem a função "showworld_new" que faz um malloc.
     * Depois gera a janela gráfica para o jogo 2D. 
     */
    SHOWWORLD *sw = showworld_new(config.xdim, config.ydim, example_get_ag_info);

    /**
     * Criamos o World dando-lhe o "x, y" definidos no ficheiro Ini.
     */
    w = world_new(config.xdim, config.ydim);

    /** 
     * Damos um srand para randomizar a seed para poder uzar a função rand().
     */
    srand(time(NULL));
    
    /** 
     * Recorremos à variavel nagents que vai ser o numero de agentes na
     * grelha e guardamos os na grelha e na struct de agentes.
     */
    for (int i = 0; i < nagents; i++) {
        int x, y;
        AGENT_TYPE at;

        /**Damos um valor aleatorio a "x, y" para os colocar na grelha*/
        x = (rand() % config.xdim);
        y = (rand() % config.ydim);

        /**
         * Caso não exita um agente no "x, y" da grelha, colocamos lá um
         * agente.
         */
        if (world_get(w, x, y) == NULL) {
            /**
             * Recolhemos apartir do ficheiro ini.h, o numero de
             * Humanos e Zombies que iremos ter na gerlha.
             */
            if (i >= 0 && i < config.nhumans) {
                at = Human;
            } else if (i >= config.nhumans && i < nagents) {
                at = Zombie;
            }
            
            /**
            * Criamos uma unsigned int "playable" que ira ditar se o
            * agente vai ser controlavel ou se é controlado po AI
            */
            unsigned int playable = (rand() % 10 == 0);
            
            /**
             * Criamos um novo agente passando-lhe o "tipo (se é Humano ou 
             * Zombie), Id e se é Jogavel ou Ai"
             */
            AGENT *a = agent_new(at, i, playable);
            
            /**
             * Colocamos o agent no world passando-lhe "x, y" e agent, 
             * realizamos esta passagem como um item para que seja uma 
             * função mais genérica.
             */
            world_put(w, x, y, (ITEM *) a);
            
            /**
             * Passamos-lhe as informações do agente contidas na struct,
             * duplicando a informação para mais tarde podermos alterar 
             * valores sem causar erros.
             */
            agents[i].id = i;
            agents[i].x = x;
            agents[i].y = y;
            agents[i].type = at;
            agents[i].ply = playable;
        } else {
            i--;
        }

    }

    /** 
     * Fazemos um update ao world para podermos observar o primeiro
     *  turno e como começa a partida.
     */
    showworld_update(sw, w);
    printf("Pressione ENTER para o seguinte turno...");
    getchar();
    printf("\n");

    /** 
     * Criamos um sistema de turnos que vai de 0 até ao valor maximo
     *  definido no ficheiro Ini.
     */
    for (int turn = 0; turn < config.maxturns; turn++) {

        /**
         * Faz o shuffle da struct onde cada turno, vai a ordenar
         * aleatoriamente os elementos da stuct.
         */
        for (int i1 = nagents - 1; i1 > 0; i1--) {
            int index = rand() % i1;
            //troca
            int temp1 = agents[index].x;
            agents[index].x = agents[index].x;
            agents[index].x = temp1;

            int temp2 = agents[index].y;
            agents[index].y = agents[index].y;
            agents[index].y = temp2;

            int temp3 = agents[index].id;
            agents[index].id = agents[index].id;
            agents[index].id = temp3;

            AGENT_TYPE temp4 = agents[index].type;
            agents[index].type = agents[index].type;
            agents[index].type = temp4;

            int temp5 = agents[index].ply;
            agents[index].ply = agents[index].ply;
            agents[index].ply = temp5;
        }

        // Imprime o Shuffle
        printf("Shuffle: ");
        for (int i2 = 0; i2 < nagents - 1; i2++) {
            printf("id = %d ", agents[i2].id);
        }
        printf("\n\n");

        /**
         * Inicialização de variaveis
         * A variavel "move" vai servir para realizar a vizinhança de moore.
         */
        int move;
        /** 
         * "x/yPrincipal" são variaveis que servem para guardar as posições 
         * iniciais dos agentes antes de os mesmos se mexerem de posicao.
         */
        int xPrincipal = 0;
        int yPrincipal = 0;
        /**
         * "x/yNovo" são variaveis que servem para guardar a posição para onde 
         * iremos mexer o agent.
         */
        int xNovo = 0;
        int yNovo = 0;
        /** 
         * A variavel "aNovo" serve para pesquisar pelos agentes que 
         * encontramos usando a vizinhança de moore.
         */
        int aNovo = 0;

        /** 
         * Pesquizamos os agents na nossa struct para encontrar o "x, y" dos 
         * agentes na posicao "x, y" da grelha.
         */
        for (int na = 0; na < nagents - 1; na++) {
            /**
             * Actualizamos as variaveis com as posições que precisamos 
             * para realizar os movimentos.
             */
            xPrincipal = agents[na].x;
            yPrincipal = agents[na].y;
            xNovo = agents[na].x;
            yNovo = agents[na].y;
            /**
             * "typeA" é uma variavel usada para guardar o tipo dos agentes
             *  encontrados na struct. (0 = Humano; 1 = Zombie;).
             */
            int typeA = 0;
            /**
             * Inicializamos a variavel "toro" para usala mais tarde na função 
             * que realiza o movimento toroidal.
             */
            int toro = 0;
            
            int apagar = 0;
            /** 
             * "move" é iniciada com o valor 3 para iniciar a pesquisa na 
             * vizinhança de moore na coordenada em "y" superior a onde o 
             * agent que pesquisa se encontra na grelha.
             */
            move = 3;
            /**
             * "seta", serve para guardar as teclas primidas que iremos usar 
             * para mexer os agents que são "playable".
             */
            int seta;
            /**
             * "x/yMexe" são as variaveis que são usadas para nos mexermos com 
             * as teclas.
             */
            int xMexe = 0;
            int yMexe = 0;

            /** Entra no if caso o agent seja "playable".*/
            if (agents[na].ply == 1) {
                
                /**
                 * Iniciamos um loop while que so ira parar quando uzamos as 
                 * teclas do numpad correspondentes às de movimento.
                 */
                while (seta != 1 || seta != 2 || seta != 3 || seta != 4 || seta != 6 || seta != 7 || seta != 8 || seta != 9) {

                    /**
                     * Idicamos ao jogador para realizar um input de maneira a 
                     * mover o ajent na grid. Tendo logo de seguida um scanf 
                     * para ler o tal input.
                     */
                    printf("Usa as setas do pad para moverte\n");
                    scanf("%d", &seta);
                    printf("Usa as setas do pad para moverte %d\n", seta);
                    /** 
                     * Igualamos a posição principal do agente que iremos mover, 
                     * com a de "x/yMexe". Para mais tarde podermos verificar se 
                     * o movimento pode ser realizado sem obstruções antes de 
                     * realmente mexermos o agent na grid.
                     */
                    xMexe = agents[na].x;
                    yMexe = agents[na].y;
                    /**
                     * Temos um switch para "seta" que consoante o seu valor 
                     * move em uma determinada direção.
                     */
                    switch (seta) {
                        case 1:
                            xMexe--;
                            yMexe++;

                            break;
                        case 2:
                            yMexe++;
                            break;
                        case 3:
                            xMexe++;
                            yMexe++;
                            break;
                        case 4:
                            xMexe--;
                            break;
                        case 6:
                            xMexe++;
                            break;
                        case 7:
                            xMexe--;
                            yMexe--;
                            break;
                        case 8:
                            yMexe--;
                            break;
                        case 9:
                            xMexe++;
                            yMexe--;
                            break;
                    }
                    
                    /**
                     * Chamamos a função "toroidal" para verificar se o nosso 
                     * agent se esta a mover para fora da grid fazendo-o assim 
                     * ir para o lado contrario da grid.
                     */
                    toroidal(&xMexe, &yMexe, &toro, &na, config);

                    /**
                     * Se a posição para onde queremos mover o nosso agent 
                     * estiver vazia passamos as cordenadas de "x/yMexe" para 
                     * o "x, y" do nosso agent, fazendo com que ele se mova na 
                     * grid.
                     */
                    if (world_get(w, xMexe, yMexe) == NULL) {
                        agents[na].x = xMexe;
                        agents[na].y = yMexe;
                        apagar = 1;

                    } else {
                        /**
                         * Caso contrario entramos num ciclo for para 
                         * determinar o tipo de agent que se encontra no 
                         * nosso caminho.
                         */
                        for (aNovo = 0; aNovo < nagents; aNovo++) {
                            /**
                             * Perguntamos se o tipo de agente encontrado é 
                             * diferente do nosso.
                             */
                            if (xMexe == agents[aNovo].x && yMexe == agents[aNovo].y 
                                && agents[na].type != agents[aNovo].type) {
                                
                                /**
                                 * Caso sejamos um Zombie e tenhamos encontrado 
                                 * um Humano, procedemos há "infecção".
                                 */
                                if (agents[aNovo].type == Human && agents[na].type == 2) {
                                    /**Mudamos o tipo de Humano para Zombie*/
                                    agents[aNovo].type = 2;
                                    /**
                                     * Chamamos o agent que iremos mudar para Zombie 
                                     * na posição "x, y" da grelha.
                                     */
                                    AGENT *a2 = world_get(w, agents[aNovo].x, agents[aNovo].y);
                                    /**
                                     * Chamamos a função para mudar o agent type 
                                     * dando-lhe o tipo e agent para mudar.
                                     */
                                    mudar_agent_type(agents[aNovo].type, (AGENT *) a2);
                                    /** Colocamos o agent alterado no world*/
                                    world_put(w, agents[aNovo].x, agents[aNovo].y, (ITEM *) a2);
                                    free(a2);

                                }
                            }

                        }

                    }

                    /**
                     * Quando realizamos o movimento com "playable", 
                     * usamos um "goto" para sair do ciclo for.
                     */
                    goto movela;

                }
            } else {
                /** Caso o agent não seja "playable" inicializamos o Ai.*/
                /**
                * iniciamos uma "função" de pesquisa em caracol (sem pasar pela mesma
                * posição duas vezes), para achar o agent de tipo diferente mais perto.
                */
                /**
                * No primeiro for temos i que ira definir o valor maximo de range de 
                * pesquisa, mais tarde definindo tambem quantas "casas" anda a pesquisa 
                * antes de alterar a direção.
                */
                for (int i = 1; i <= ((config.xdim+config.ydim)/4); i++) {
                    /**
                     * Aqui inicializamos outro for que so ira correr duas vezes, 
                     * isto porque em uma pesquisa em caracol andamos sempre a mesma 
                     * distancia duas vezes antes de aumentar em 1.
                     */
                    /**
                     * Ex: Da nossa casa inicial andamos 1 para cima e 1 para a direita 
                     * depois andamos 2 para baixo e 2 para a esquerda... (3,3)..(7,7)...
                     */
                    for (int j = 1; j <= 2; j++) {
                        /** 
                        * Move é usado como um case para saber se a verificação block a block vai se mover
                        * para baixo para cima para a direita ou para baixo.
                        **/
                        if (move == 4) {
                            move = 1;
                        } else move += 1;
                        /**
                         * Inicializamos outro ciclo for que ira andar de casa em casa 
                         * usando o "i" para saber quantas casas deve andar.
                         */
                        for (int block = 1; block <= i; block++) {
                            switch (move) {
                                case 1:
                                    // Mexe para a direita i vezes
                                    xNovo += 1;
                                    break;
                                case 2:
                                    // Mexe para baixo i vezes
                                    yNovo += 1;
                                    break;
                                case 3:
                                    // Mexe para a esquerda i vezes
                                    xNovo -= 1;
                                    break;
                                case 4:
                                    // Mexe para cima i vezes
                                    yNovo -= 1;
                                    break;
                            }
                            /**
                             * Chamamos a função "toroidal" para verificar tambem 
                             * no lado contrario da grid caso ele esteja perto de 
                             * uma das paredes.
                             */
                            toroidal(&xNovo, &yNovo, &toro, &na, config);
                            /**
                             * Se a posição que estivermos a pesquisar for diferente 
                             * de "NULL" Sabemos então que la se encontra um agent.
                             */
                            if (w->grid[yNovo * w->xdim + xNovo] != NULL) {
                                /**
                                 * Iniciamos um ciclo for que percorre a struct de 
                                 * agents até encontrar o agent que se encontra na 
                                 * posição que estavamos a pesquisar.
                                 */
                                for (aNovo = 0; aNovo < nagents; aNovo++) {
                                    /**
                                     * Caso o agent type seja diferente do nosso 
                                     * iremos ter que realizar movimentos diferentes 
                                     * para Humano e Zombie.
                                     */
                                    if (xNovo == agents[aNovo].x && yNovo == agents[aNovo].y && 
                                        agents[na].type != agents[aNovo].type) {
                                        /**
                                        * Como já tinhamos inicializado a var "typeA" 
                                        * a 0 (0 = Humano;) so precisamos de passa-la 
                                        * para 1 caso o nosso agent seja Zombie.
                                        */
                                        if (agents[na].type == Zombie) {
                                            typeA = 1;
                                        }
                                        /**
                                        * Ao fim de realizar todas as operações usamos 
                                        * um "goto" para sair dos ciclos for.
                                        */
                                        goto movela;

                                    }

                                }
                            }
                        }
                    }
                }
            }
            /**
             * "movela" é onde o "goto" usado anteriormente vai parar caso tenha 
             * tudo corrido bem.
             */
movela:
            /**
            * Perguntamos se o agent não é "playable".
            */
            if (agents[na].ply == 0) {
                /**
                 * Caso o agent não seja "playable" chamamos a função "distancia" 
                 * que ira realizar todos os movimentos do Ai.
                 */
                distancia(&agents[na].x, &agents[na].y, agents[aNovo].x, agents[aNovo].y, typeA, &agents[na].type, toro, w, na, &apagar, agents, nagents, config);
            }
            /**
             * Uma vez que todos os movimentos foram realizados 
             * modificamos o agent que metemos no world com novas 
             * posições e types.
             */
            AGENT *a1 = agent_new(agents[na].type, agents[na].ply, agents[na].id);
            /**
             * Caso "apagar" seja igual a 1 quer dizer que o agent se vai mover, 
             * então teremos que apagar a sua posição anterior para que o agent 
             * não seja dulicado.
             */
            if (apagar == 1) {
                world_apagar(w, xPrincipal, yPrincipal);
            }
            /** Colocamos o agent na nova posição do world.*/
            world_put(w, agents[na].x, agents[na].y, (ITEM *) a1);
            printf("Pressione ENTER para o seguinte turno...");
            getchar();
            /** Actualizamos o ecrã que mostra o jogo (terminal/Livraria)*/
            showworld_update(sw, w);

        }
    }
    /** Libertamos a memoria de "showworld", dos items de world e world*/
    showworld_destroy(sw);
    world_destroy_full(w, (void (*)(ITEM *))agent_destroy);
    return 0;
}

/**
 * Função que serve para ler as variaveis do ficheiro ini.h
 * @param user
 * @param section
 * @param name
 * @param value
 * @return 
 */
static int handler(void* user, const char* section, const char* name,
        const char* value) {
    configuration* pconfig = (configuration*) user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("dim", "xdim")) {
        pconfig->xdim = atoi(value);
    } else if (MATCH("dim", "ydim")) {
        pconfig->ydim = atoi(value);
    } else if (MATCH("nInit", "nzombies")) {
        pconfig->nzombies = atoi(value);
    } else if (MATCH("nInit", "nhumans")) {
        pconfig->nhumans = atoi(value);
    } else if (MATCH("nControl", "nzplayers")) {
        pconfig->nzplayers = atoi(value);
    } else if (MATCH("nControl", "nhplayers")) {
        pconfig->nhplayers = atoi(value);
    } else if (MATCH("turns", "maxturns")) {
        pconfig->maxturns = atoi(value);
    } else {
        return 0; /* unknown section/name, error */
    }
    return 1;
}

/**
 * This function is an implementation of the ::get_agent_info_at() function
 * definition. It only works for ::AGENT and ::WORLD structures defined in this
 * example.
 *
 * @param w Generic pointer to object representing the simulation world.
 * @param x Horizontal coordinate of the simulation world from where to fetch
 * the agent information.
 * @param y Vertical coordinate of the simulation world from where to fetch
 * the agent information.
 * @return An integer containing bit-packed information about an agent, as
 * follows: bits 0-1 (agent type), bit 2 (is agent playable), bits 3-18 (agent
 * ID). Bits 19-31 are available for student-defined agent extensions.
 * */
unsigned int example_get_ag_info(void *w, unsigned int x, unsigned int y) {

    /* The agent information to return. */
    unsigned int ag_info = 0;

    /* Convert generic pointer to world to a WORLD object. */
    WORLD *my_world = (WORLD *) w;

    /* Check if the given (x,y) coordinates are within bounds of the world. */
    if ((x >= my_world->xdim) || (y >= my_world->ydim)) {

        /* If we got here, then the coordinates are off bounds. As such we will
           report that the requested agent is of unknown type. No need to
           specify agent ID or playable status, since the agent is unknown. */
        ag_info = Unknown;

    } else {

        /* Given coordinates are within bounds, let's get and pack the request
           agent information. */

        /* Obtain agent at specified coordinates. */
        AGENT *ag = (AGENT *) world_get(my_world, x, y);

        /* Is there an agent at (x,y)? */
        if (ag == NULL) {

            /* If there is no agent at the (x,y) coordinates, set agent type to
               None. No need to specify agent ID or playable status, since
               there is no agent here. */
            ag_info = None;

        } else {

            /* If we get here it's because there is an agent at (x,y). Bit-pack
               all the agent information as specified by the get_agent_info_at
               function pointer definition. */
            ag_info = (ag->id << 3) | (ag->playable << 2) | ag->type;

        }

    }

    /* Return the requested agent information. */
    return ag_info;

}

/**
 * Função toroidal que serve para mexer o agent ou o block de pesquisa para 
 * o lado contrario da grid caso ele passe dos limites da grid.
 * 
 * @param x 
 * @param y
 * @param toro
 * @param na
 */
void toroidal(int *x, int *y, int *toro, int *na, configuration config) {
    int ax = *x;
    int ay = *y;
    int xSize = config.xdim;
    int ySize = config.ydim;

    /**
     * Aqui temos 2 if com mais 2 else if que server para fazer a verificação 
     * em x e em y caso o agent ou o block de pesquisa outropassem os limites 
     * da grid.
     *
     * É tambem alterada a var "toro" de 0 para 1 caso ele passe para o outro 
     * lado da grid, e de 1 para 0 caso ele regresse para o lado original.
     */
    if (ax > xSize) {
        ax = 0;
        if (na1 != *na) {
            if (*(toro) == 0) {
                *(toro) = 1;
            } else {
                *(toro) = 0;
            }
        }
        na1 = *na;
    } else if (ax < 0) {
        ax = xSize;
        if (na1 != *na) {
            if (*(toro) == 0) {
                *(toro) = 1;
            } else {
                *(toro) = 0;
            }
        }
        na1 = *na;
    }
    if (ay < 0) {
        ay = ySize;
        if (na1 != *na) {
            if (*(toro) == 0) {
                *(toro) = 1;
            } else {
                *(toro) = 0;
            }
        }
        na1 = *na;
    } else if (ay > ySize) {
        ay = 0;
        if (na1 != *na) {
            if (*(toro) == 0) {
                *(toro) = 1;
            } else {
                *(toro) = 0;
            }
        }
        na1 = *na;
    }
    if (ay < 0 && ay > ySize && ax < 0 && ax > xSize) {
        *(toro) = 0;
        na1 -= 1;
    }

    *x = ax;
    *y = ay;
}

/**
 * MoveToroidal serve para verificar para que lado é mais rapido se
 * mexer consoante a distancia entre o Humano e Zombie.
 * 
 * @param xN
 * @param yN
 * @param xNovo
 * @param yNovo
 * @param movein
 * @param movedir
 */
void MoveToroidal(int xN, int yN, int xNovo, int yNovo, int *movein, int *movedir, configuration config) {
    int dir = *movedir;
    int xSize = (config.xdim/2);
    int ySize = (config.ydim/2);

    if ((xN - xNovo > xSize) || (xN - xNovo < -(xSize))) {
        *movein = 1;
        dir = 0;
    } else if ((yN - yNovo > ySize) || (yN - yNovo < -(ySize))) {
        *movein = 1;
        dir = 1;
    } else if (((xN - xNovo > xSize) || (xN - xNovo < -(xSize))) && ((yN - yNovo > ySize) || (yN - yNovo < -(ySize)))) {
        *movein = 1;
        dir = 2;
    }

    *movedir = dir;
} 

/**
 * Função que irá realizar todos os movimentos dos agents Ai.
 */
/**
 * Realizando tambem a infecção dos Zombies para Humanos.
 * 
 * @param "x, y" coordenada "x, y" do nosso agente.
 * @param "x/yNovo" coordenada "x, y" do agent pesquisado com vizinhança de moore.
 * @param "typeA" tipo do agent (Humano ou Zombie).
 * @param "agTypeAnt" tipo do agent pesquisado pela vizinhança de moore
 * @param "toro" flag para saber se tamos numa posição em que realizaremos um 
 * movemento toroidal.
 * @param "w" o nosso world.
 * @param "na" numero do agente noso na nossa struct
 * @param "apagar" flag para saber se o nosso agent realizou o movemento e 
 * saber se teremos que apagar a sua antiga posição
 * @param "agents" apontador para a struct
 * @param "nagents" numero de agents para realizar a infecção e mudanca de type
 */
void distancia(int *x, int *y, int xNovo, int yNovo, int typeA,
        AGENT_TYPE *agTypeAnt, int toro, WORLD *w, int na, int *apagar,
        struct agentID *agents, int nagents, configuration config) {
    /** Guardamos a posição do nosso agent que vamos mudar.*/
    int xN = *x;
    int yN = *y;
    /**
     * "movein" irá guardar 1 ou -1 dependendo de se estamos a mover em toroidal ou 
     * não.
     */
    int movein = 1;
    /**
     * "movedir" vai indicar para onde o nosso agent está-se a mover.
     */
    int movedir = 4;

    /**
     * Chamamos a função "MoveToroidal" que irá verificar para que lado é mais rapido 
     * o nosso agent se mexer.
     */
    MoveToroidal(xN, yN, xNovo, yNovo, &movein, &movedir, config);

    /**
     * Caso "movedir" seja igual a (4) quer dizer que não nos estamos a mover em um 
     * sentido toroidal então temos que alterar a var "movein" para (-1).
     */
    if (movedir == 4) {
        movein = -1;
    }
    
    /**
     * Iniciamos um Switch para "movedir" que dependendo do seu valor irá saber 
     * para onde se mexer.
     *
     * @note 0 = toroidal em x
     * @note 1 = toroidal em y
     * @note 2 = toroidal em x e y
     * @note 4 = não é toroidal
     */
    switch (movedir) {
        case 0:
            if (typeA == 1) {
                /**Movemento do Zombie.*/
                if (xN > xNovo && yN > yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN += movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN -= movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN += movein;
                }
            } else {
                /**Movemento do Humano.*/
                if (xN > xNovo && yN > yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN += movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN += movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN -= movein;
                }
            }
            break;
        case 1:
            if (typeA == 1) {
                /**Movemento do Zombie.*/
                if (xN > xNovo && yN > yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN += movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN -= movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN += movein;
                }
            } else {
                /**Movemento do Humano.*/
                if (xN > xNovo && yN > yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN += movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN += movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN -= movein;
                }
            }
            break;
        case 2:
            if (typeA == 1) {
                /**Movemento do Zombie.*/
                if (xN > xNovo && yN > yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN += movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN -= movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN += movein;
                }
            } else {
                /**Movemento do Humano.*/
                if (xN > xNovo && yN > yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN -= movein;
                    yN += 1;
                } else if (xN < xNovo && yN > yNovo) {
                    xN += 1;
                    yN -= movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN += movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN += movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN -= movein;
                }
            }
            break;
        case 4:
            if (typeA == 1) {
                /**Movemento do Zombie.*/
                if (xN > xNovo && yN > yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN += movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN -= movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN += movein;
                }
            } else {
                /**Movemento do Humano.*/
                if (xN > xNovo && yN > yNovo) {
                    xN -= movein;
                    yN -= movein;
                } else if (xN < xNovo && yN < yNovo) {
                    xN += movein;
                    yN += movein;
                } else if (xN > xNovo && yN < yNovo) {
                    xN -= movein;
                    yN += movein;
                } else if (xN < xNovo && yN > yNovo) {
                    xN += movein;
                    yN -= movein;
                } else if (xN < xNovo && yN == yNovo) {
                    xN += movein;
                } else if (xN > xNovo && yN == yNovo) {
                    xN -= movein;
                } else if (yN < yNovo && xN == xNovo) {
                    yN += movein;
                } else if (yN > yNovo && xN == xNovo) {
                    yN -= movein;
                }
            }
            break;
    }
    
    /**
     * Chama-se a função "toroidal" para verificar se o agent saiu for da grid 
     * e caso tenha saido colocamos o no lado oposto.
     */
    toroidal(&xN, &yN, &toro, &na, config);

    /**
     * Se a posição para onde nos queremos mover estiver vazia guardamos o "x, y" 
     * da nova posição
     */
    if (world_get(w, xN, yN) == NULL) {

        *x = xN;
        *y = yN;
        *apagar = 1;

    } else {
        int aNovo;
        /**
         * Caso contrário procuramos por o agent que se encontra nesse espaço.
         */
        for (aNovo = 0; aNovo < nagents; aNovo++) {
            /**
             * Perguntamos se o agent encontrado é de tipo diferente do nosso.
             */
            if (xN == agents[aNovo].x && yN == agents[aNovo].y && (int*) agTypeAnt != (int*) agents[aNovo].type) {
                if (agents[aNovo].type == Human && *agTypeAnt == 2) {
                    /**
                     * Se o noso type for Zombie e o novo for Humano infectamos.
                     */
                    agents[aNovo].type = 2;
                    /**
                     * Tiramos o agent novo na grid do world.
                     */
                    AGENT *a2 = world_get(w, agents[aNovo].x, agents[aNovo].y);
                    /**
                     * Mudamos o type do agent.
                     */
                    mudar_agent_type(agents[aNovo].type, (AGENT *) a2);
                    /**
                     * Mandamos a nova informação do agent para o world.
                     */
                    world_put(w, agents[aNovo].x, agents[aNovo].y, (ITEM *) a2);
                    free(a2);
                }
            }
        }
    }
}