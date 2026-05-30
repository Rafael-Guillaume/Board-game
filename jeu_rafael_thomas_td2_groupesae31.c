#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "board.h"

// Codes couleurs ANSI
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define ORANGE "\033[38;5;208m"
#define PURPLE "\033[1;95m"
#define RESET "\033[0m"


void print_board(board game) {
    int picked_line = picked_piece_line(game);
    int picked_col = picked_piece_column(game);
    
    printf("\n");
    printf("                           " PURPLE "NORD" RESET "\n");
    printf("          " PURPLE "╔═════╦═════╦═════╦═════╦═════╦═════╗" RESET "\n");
    
    for (int i = DIMENSION - 1; i >= 0; i--) {
        printf("        %d " CYAN "║" RESET, i + 1);
        
        for (int j = 0; j < DIMENSION; j++) {
            if (picked_line == i && picked_col == j) {
                printf("  " YELLOW "*" RESET "  " CYAN "║" RESET);
            } else {
                size piece = get_piece_size(game, i, j);
                if (piece == ONE) {
                    printf("  " GREEN "1" RESET "  " CYAN "║" RESET);
                } else if (piece == TWO) {
                    printf("  " ORANGE "2" RESET "  " CYAN "║" RESET);
                } else if (piece == THREE) {
                    printf("  " MAGENTA "3" RESET "  " CYAN "║" RESET);
                } else {
                    printf("     " CYAN "║" RESET);
                }
            }
        }
        printf("\n");
        
        if (i != 0) {
            printf("          " PURPLE "╠═════╬═════╬═════╬═════╬═════╬═════╣" RESET "\n");
        }
    }
    
    printf("          " PURPLE "╚═════╩═════╩═════╩═════╩═════╩═════╝" RESET "\n");
    printf("            1     2     3     4     5     6\n");
    printf("                           " CYAN "SUD" RESET "\n\n");
}

void vider_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

bool toutes_pieces_placees(board game) {
    for (player p = SOUTH_P; p <= NORTH_P; p++) {
        for (size s = ONE; s <= THREE; s++) {
            if (nb_pieces_available(game, s, p) > 0) {
                return false;
            }
        }
    }
    return true;
}

direction saisir_direction() {
    char input;
    printf("Direction (n=Nord, s=Sud, e=Est, o=Ouest, g=Goal) : ");
    scanf(" %c", &input);
    vider_buffer();
    
    switch (input) {
        case 'n': case 'N': return NORTH;
        case 's': case 'S': return SOUTH;
        case 'e': case 'E': return EAST;
        case 'o': case 'O': return WEST;
        case 'g': case 'G': return GOAL;
        default: return -1;
    }
}

void afficher_mouvements_possibles(board game) {
    printf(YELLOW "Mouvements possibles : " RESET);
    if (is_move_possible(game, NORTH)) printf("Nord(n) ");
    if (is_move_possible(game, SOUTH)) printf("Sud(s) ");
    if (is_move_possible(game, EAST)) printf("Est(e) ");
    if (is_move_possible(game, WEST)) printf("Ouest(o) ");
    if (is_move_possible(game, GOAL)) printf(GREEN "GOAL(g) " RESET);
    printf("\n");
}



//phase de placement

void phase_placement(board game) {
    printf("\n" CYAN "=== PHASE DE PLACEMENT ===" RESET "\n\n");
    
    player joueur_actuel = SOUTH_P;
    
    while (!toutes_pieces_placees(game)) {
        print_board(game);
        
        printf("\n" MAGENTA "Joueur %s" RESET ", placez vos pieces :\n", 
               joueur_actuel == SOUTH_P ? "SUD" : "NORD");
        printf("Pieces restantes : ");
        printf("Taille 1: %d, ", nb_pieces_available(game, ONE, joueur_actuel));
        printf("Taille 2: %d, ", nb_pieces_available(game, TWO, joueur_actuel));
        printf("Taille 3: %d\n", nb_pieces_available(game, THREE, joueur_actuel));
        
        int taille, colonne;
        printf("Entrez la taille de la piece (1-3) : ");
        if (scanf("%d", &taille) != 1) {
            printf(RED "Erreur : Entree invalide !\n" RESET);
            vider_buffer();
            continue;
        }
        
        printf("Entrez la colonne (1-6) : ");
        if (scanf("%d", &colonne) != 1) {
            printf(RED "Erreur : Entree invalide !\n" RESET);
            vider_buffer();
            continue;
        }
        
        if (taille < 1 || taille > 3) {
            printf(RED "Erreur : La taille doit etre entre 1 et 3 !\n" RESET);
            continue;
        }
        
        if (colonne < 1 || colonne > 6) {
            printf(RED "Erreur : La colonne doit etre entre 1 et 6 !\n" RESET);
            continue;
        }
        
        size piece_size = (taille == 1) ? ONE : (taille == 2) ? TWO : THREE;
        return_code res = place_piece(game, piece_size, joueur_actuel, colonne - 1);
        
        if (res == OK) {
            printf(GREEN "Piece placee avec succes !\n" RESET);
            joueur_actuel = next_player(joueur_actuel);
        } else if (res == EMPTY) {
            printf(RED "Erreur : Cette case est deja occupee !\n" RESET);
        } else if (res == FORBIDDEN) {
            printf(RED "Erreur : Vous n'avez plus de pieces de cette taille !\n" RESET);
        } else if (res == PARAM) {
            printf(RED "Erreur : Parametres invalides !\n" RESET);
        }
    }
    
    printf("\n" CYAN "=== PHASE DE PLACEMENT TERMINEE ===" RESET "\n");
    print_board(game);
}



//phase dde jeu

void phase_jeu(board game) {
    printf("\n" CYAN "=== PHASE DE JEU ===" RESET "\n\n");
    
    player joueur_actuel = SOUTH_P;
    
    while (get_winner(game) == NO_PLAYER) {
        print_board(game);
        
        printf("\n" MAGENTA "=== Tour du joueur %s ===" RESET "\n", 
               joueur_actuel == SOUTH_P ? "SUD" : "NORD");
        
        int ligne_a_jouer;
        if (joueur_actuel == SOUTH_P) {
            ligne_a_jouer = southmost_occupied_line(game);
        } else {
            ligne_a_jouer = northmost_occupied_line(game);
        }
        
        printf("Vous devez jouer une piece de la ligne %d\n", ligne_a_jouer + 1);
        
        int ligne, colonne;
        printf("Entrez la ligne de la piece a deplacer (1-6) : ");
        if (scanf("%d", &ligne) != 1) {
            printf(RED "Erreur : Entree invalide !\n" RESET);
            vider_buffer();
            continue;
        }
        
        printf("Entrez la colonne de la piece a deplacer (1-6) : ");
        if (scanf("%d", &colonne) != 1) {
            printf(RED "Erreur : Entree invalide !\n" RESET);
            vider_buffer();
            continue;
        }
        
        return_code res = pick_piece(game, joueur_actuel, ligne - 1, colonne - 1);
        
        if (res != OK) {
            if (res == EMPTY) {
                printf(RED "Erreur : Aucune piece a cet emplacement !\n" RESET);
            } else if (res == FORBIDDEN) {
                printf(RED "Erreur : Vous ne pouvez pas jouer cette piece !\n" RESET);
            } else if (res == PARAM) {
                printf(RED "Erreur : Position invalide !\n" RESET);
            }
            continue;
        }
        
        printf(GREEN "Piece selectionnee : Taille %d a la position (%d, %d)\n" RESET, 
               picked_piece_size(game), picked_piece_line(game) + 1, picked_piece_column(game) + 1);
        
        bool mouvement_termine = false;
        while (!mouvement_termine && get_winner(game) == NO_PLAYER) {
            print_board(game);
            
            printf("\n" YELLOW "Piece en mouvement :" RESET " Position(%d, %d) | Taille: %d | Mouvements restants: %d\n",
                   picked_piece_line(game) + 1, picked_piece_column(game) + 1,
                   picked_piece_size(game), movement_left(game));
            
            if (movement_left(game) == 0) {
                printf(CYAN "\nVotre piece a atterri sur une autre piece !\n" RESET);
                printf("Options :\n");
                printf("  b - Rebondir (continuer le mouvement)\n");
                printf("  s - Swap (echanger et replacer l'autre piece)\n");
                
                char choix;
                printf("Votre choix : ");
                scanf(" %c", &choix);
                vider_buffer();
                
                if (choix == 's' || choix == 'S') {
                    int target_ligne, target_colonne;
                    
                    printf("Entrez la ligne pour la piece echangee (1-6) : ");
                    if (scanf("%d", &target_ligne) != 1) {
                        printf(RED "Erreur : Entree invalide !\n" RESET);
                        vider_buffer();
                        continue;
                    }
                    
                    printf("Entrez la colonne pour la piece echangee (1-6) : ");
                    if (scanf("%d", &target_colonne) != 1) {
                        printf(RED "Erreur : Entree invalide !\n" RESET);
                        vider_buffer();
                        continue;
                    }
                    
                    return_code swap_res = swap_piece(game, target_ligne - 1, target_colonne - 1);
                    if (swap_res == OK) {
                        printf(GREEN "Swap effectue avec succes !\n" RESET);
                        mouvement_termine = true;
                    } else {
                        printf(RED "Erreur : Position invalide pour le swap !\n" RESET);
                    }
                    continue;
                }
            }
            
            afficher_mouvements_possibles(game);
            
            printf("\nCommandes speciales : u=Annuler dernier pas, c=Annuler tout le mouvement\n");
            
            char input;
            printf("Votre choix : ");
            scanf(" %c", &input);
            vider_buffer();
            
            if (input == 'u' || input == 'U') {
                if (cancel_step(game) == OK) {
                    printf(GREEN "Dernier pas annule !\n" RESET);
                } else {
                    printf(RED "Impossible d'annuler !\n" RESET);
                }
                continue;
            }
            
            if (input == 'c' || input == 'C') {
                if (cancel_movement(game) == OK) {
                    printf(GREEN "Mouvement annule !\n" RESET);
                    mouvement_termine = true;
                } else {
                    printf(RED "Impossible d'annuler !\n" RESET);
                }
                continue;
            }
            
            direction dir;
            switch (input) {
                case 'n': case 'N': dir = NORTH; break;
                case 's': case 'S': dir = SOUTH; break;
                case 'e': case 'E': dir = EAST; break;
                case 'o': case 'O': dir = WEST; break;
                case 'g': case 'G': dir = GOAL; break;
                default:
                    printf(RED "Direction invalide !\n" RESET);
                    continue;
            }
            
            return_code move_res = move_piece(game, dir);
            
            if (move_res == OK) {
                printf(GREEN "Mouvement effectue !\n" RESET);
                
                if (get_winner(game) != NO_PLAYER) {
                    mouvement_termine = true;
                }
                
                if (movement_left(game) == -1) {
                    mouvement_termine = true;
                }
            } else if (move_res == FORBIDDEN) {
                printf(RED "Mouvement interdit dans cette direction !\n" RESET);
            } else if (move_res == PARAM) {
                printf(RED "Mouvement hors du plateau !\n" RESET);
            } else {
                printf(RED "Erreur lors du mouvement !\n" RESET);
            }
        }
        
        if (!mouvement_termine) {
            continue;
        }
        
        joueur_actuel = next_player(joueur_actuel);
    }
    
    print_board(game);
    player gagnant = get_winner(game);
    printf("\n" GREEN "╔════════════════════════════════════════╗\n");
    printf("║                                        ║\n");
    printf("║    🎉  LE JOUEUR %s A GAGNE !  🎉    ║\n", 
           gagnant == SOUTH_P ? "SUD " : "NORD");
    printf("║                                        ║\n");
    printf("╚════════════════════════════════════════╝\n" RESET);
}

void clear_screen(){
	printf("\033[2J\033[H");
}

int main() {
    board game = new_game();
    
    printf(CYAN "╔════════════════════════════════════════════════════╗\n");
    printf("║                                                    ║\n");
    printf("║      BIENVENUE DANS LE JEU DE RAFAEL ET THOMAS     ║\n");
    printf("║                                                    ║\n");
    printf("╚════════════════════════════════════════════════════╝\n" RESET);
    
    phase_placement(game);
    
    printf("\n" GREEN "La partie peut maintenant commencer !\n" RESET);
    printf("Appuyez sur Entree pour continuer...");
    getchar();
    
    phase_jeu(game);
    
    destroy_game(game);
    return 0;
}
