#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "solver_game.h"

int main() {
    srand(time(NULL));

    loadDictionary("words.txt");

    // Mot secret simulé
    char secret[WORD_LEN + 1];
    strcpy(secret, dictionary[rand() % wordCount]);

    printf("🤖 Solver automatique lancé\n");

    for (int attempt = 1; attempt <= 6; attempt++) {
        char guess[WORD_LEN + 1];
        strcpy(guess, dictionary[0]);  // stratégie simple

        int feedback[WORD_LEN];
        computeFeedback(secret, guess, feedback);

        printf("Tentative %d : %s -> ", attempt, guess);
        for (int i = 0; i < WORD_LEN; i++)
            printf("%d ", feedback[i]);
        printf("\n");

        if (!strcmp(guess, secret)) {
            printf("\n🎉 Mot trouvé en %d tentatives : %s\n", attempt, secret);
            return 0;
        }

        filterDictionary(guess, feedback);

        if (wordCount == 0) break;
    }

    printf("\n❌ Échec. Le mot était : %s\n", secret);
    return 0;
}