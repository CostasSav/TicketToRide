#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include "TicketToRideAPI.h"
#include "move.h"
#include "game.h"

#define INF 5000
#define N 36

int shortestDist(int D[N], int visited[N]){ //algorithme pour trouver la ville la pls proche 												mais qui n'a pas encore été visitée
	int min = INF;
	int indice_min;
	for (int i=0; i<36; i++){
		if(visited[i]==0 && D[i]<min){
			min=D[i];
			indice_min=i;
		}
	}
	return indice_min;
}

void algoDijkstra(int start, int dest, int Prec[N], int G[N][N]){  //algorithme qui donne la 																	plus courte distance entre start 																 et i, et la ville qui precede i 																 dans le chemin
	int visited[N];
	int D[N];
	int u=start;
	
	for(int i=0; i<N; i++){
		D[i]=INF;
		visited[i]=0;
	}
	D[start]=0;
	while(u!=dest){
		u=shortestDist(D, visited);
		visited[u]=1;
		for(int v=0; v<36; v++){
			if(visited[v]==0 && G[u][v]!=INF && D[u]+G[u][v]<D[v]){
				D[v]=D[u]+G[u][v];
				Prec[v]=u;
			}
		}
	}
}


int main(){
	connectToServer("li1417-56.members.linode.com", 1234, "CostasBot");
	char gameName[20];
	int nbCities;
	int nbTracks;
	int* arrayTracks;		
	t_color faceUp[5];
	t_color ourCards[4];
	int player;
	t_return_code retCode;
	int replay = 0;
	t_move move;
	t_color lastMove = NONE;
	int objTurn=0; //On fait en sorte de choisir des objectifs au premier tour, et que une fois
	
	int start;
	int dest;
	int color;
	int nbCards=4; 
	
	
	int Prec[N];
	
	
	waitForT2RGame("TRAINING DO_NOTHING timeout=10000", gameName, &nbCities, &nbTracks);
	arrayTracks = malloc(5*nbTracks*sizeof(int));
	player = getMap(arrayTracks, faceUp, ourCards);
	
	
	int G[N][N]; //Initialisation du graphe
	for (int p=0; p<N; p++){
		for (int n=0; n<N; n++){
			if (p==n){
				G[p][n]=0;
			}
			else{
				G[p][n]=INF;
			}
		}
	}
	for (int i=0; i<5*nbTracks; i=i+5){ //Remplissage du graphe
		G[arrayTracks[i]][arrayTracks[i+1]]=arrayTracks[i+2];
		G[arrayTracks[i+1]][arrayTracks[i]]=arrayTracks[i+2];
	}
	
	
	
	do{  //boucle de tour de jeu
		if (!replay)
			printMap();

		if (player == 1){

			retCode = getMove(&move, &replay);
			if (move.type == CLAIM_ROUTE){ //si l'adversaire prend un track, le rendre 											inaccessible dans le graphe
				G[move.claimRoute.city1][move.claimRoute.city2]=INF;
				algoDijkstra(start, dest, Prec, G);
					
			//if (retCode == NORMAL_MOVE && !replay)
				//player = !player;

			}
		}
		else {
			if(objTurn==0){ //premier tour
				move.type = DRAW_OBJECTIVES;
				retCode = playOurMove(&move, &lastMove);

				move.type = CHOOSE_OBJECTIVES;
				move.chooseObjectives.chosen[0]=1;
				move.chooseObjectives.chosen[1]=1;
				move.chooseObjectives.chosen[2]=0;
				retCode = playOurMove(&move, &lastMove);
				
				printf("Je gardes les objectifs 0 et 1 !");
				//printf("Je gardes %d et %d !", move.drawObjectives.objectives[0].city1, move.drawObjectives.objectives[1].city1);
				start=move.drawObjectives.objectives[0].city1;
				dest=move.drawObjectives.objectives[0].city2;
				algoDijkstra(start, dest, Prec, G);
				
				objTurn++;
				//player = 1;
			}
			else{
				int colorC=0;   //compteur pour voir combien de cartes de la meme couleur du 					track voulu on possede
				for (int m=0; m<5*nbTracks; m=m+5){  //on trouve la couleur du track qu'on veut
					if ((arrayTracks[m] == Prec[dest] && arrayTracks[m+1] == dest) || (arrayTracks[m+1] == Prec[dest] && arrayTracks[m] == dest)){
						color = arrayTracks[m+3];
						printf("c=%d",color);
						break;
					}
				}
				if (color==9){  //je n'arrive pas à utiliser les tracks multicolors
					for(int i=0; i<nbCards; i++){
						if (ourCards[i]==3){
							colorC++;
						}
					}
				}
				else{
					for(int i=0; i<nbCards; i++){
						if (ourCards[i]==color){
							colorC++;
						}
					}
				}
				printf("G=%d",G[Prec[dest]][dest]);
				printf("C=%d",colorC);
				
				if (colorC<G[Prec[dest]][dest]){
				//if (claimRoute(Prec[dest], dest, color, 0)==LOOSING_MOVE){ //<-je ne crois pas que cette formulation marche, meme si elle serait plus efficace
					int f=0;
					for(int c=0; c<5; c++){
						if (faceUp[c]==color){
							printf("f %d ", faceUp[c]);
							f++;
						}
					}
					if (f>0){  //si il y a au moins une carte de la bonne couleur face up
						move.type = DRAW_CARD;
						move.drawCard.card = color;
						retCode = playOurMove(&move, &lastMove);
						nbCards++;
						ourCards[nbCards]=color;
						//replay = needReplay(&move, lastMove);
						if(move.drawCard.card!=9){
							move.type = DRAW_BLIND_CARD;	//au lieu de replay, le bot repioche 								 une BLIND_CARD
							retCode = playOurMove(&move, &lastMove);
							nbCards++;
							ourCards[nbCards]=move.drawBlindCard.card;
						}
					}
					else{ //si il n'y a pas de carte voulue, on pioche en blind
						move.type = DRAW_BLIND_CARD;
						retCode = playOurMove(&move, &lastMove);
						nbCards++;
						ourCards[nbCards]=move.drawBlindCard.card;
						//replay = needReplay(&move, lastMove);
						if(move.drawBlindCard.card!=9){
							move.type = DRAW_BLIND_CARD; //au lieu de replay, le bot repioche 								 une BLIND_CARD
							retCode = playOurMove(&move, &lastMove);
							nbCards++;
							ourCards[nbCards]=move.drawBlindCard.card;
						}
					}
					//if (retCode == NORMAL_MOVE && !replay)
						//player = 1;
				}
				else{
					move.type = CLAIM_ROUTE; //si possible, on claim le track voulu
					move.claimRoute.city1 = Prec[dest];
					move.claimRoute.city2 = dest;
					if(color==9){
						move.claimRoute.color = 3;
					}
					else{
						move.claimRoute.color = color;
					}
					move.claimRoute.nbLocomotives = 0;
					retCode = playOurMove(&move, &lastMove);
					nbCards=nbCards-G[Prec[dest]][dest];
					G[Prec[dest]][dest] = 0; //la distance entre les deux villes deviens alors 0
					dest = Prec[dest];	//on passe a la ville d'apres
					//if (retCode == NORMAL_MOVE && !replay)
						//player = 1;
				}
			}
		}
		if (retCode == NORMAL_MOVE && !replay)
				player = !player;

	} while (retCode == NORMAL_MOVE);

	if ((retCode == WINNING_MOVE && player == 0) || (retCode == LOOSING_MOVE && player == 1))
		printf("gg ez\n");
	else
		printf("J'ai du ping bro\n");
	
	closeConnection();
	return 0;
}