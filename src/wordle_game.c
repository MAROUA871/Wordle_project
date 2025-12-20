#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "solver_game.h"

char dictionary[MAX_WORDS][WORD_LEN + 1];
int wordCount = 0;

// Charger les mots
void loadDictionary(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("could not load dictionary");
        exit(1);
    }

    while (fscanf(f, "%5s", dictionary[wordCount]) == 1) {
        wordCount++;
    }
    fclose(f);
}

// Calcul du feedback Wordle
void computeFeedback(const char *secret, const char *guess, int feedback[WORD_LEN]) {
    int used[WORD_LEN] = {0};

    for (int i = 0; i < WORD_LEN; i++) {
        if (guess[i] == secret[i]) {
            feedback[i] = CORRECT_POS;
            used[i] = 1;
        } else {
            feedback[i] = NOT_IN_WORD;
        }
    }

    for (int i = 0; i < WORD_LEN; i++) {
        if (feedback[i] == NOT_IN_WORD) {
            for (int j = 0; j < WORD_LEN; j++) {
                if (!used[j] && guess[i] == secret[j]) {
                    feedback[i] = WRONG_POS;
                    used[j] = 1;
                    break;
                }
            }
        }
    }
}

// Vérifie si un mot est compatible avec un feedback
int isWordCompatible(const char *word, const char *guess, int feedback[WORD_LEN]) {
    int tmp[WORD_LEN];
    computeFeedback(word, guess, tmp);

    for (int i = 0; i < WORD_LEN; i++)
        if (tmp[i] != feedback[i])
            return 0;

    return 1;
}

// Filtrer le dictionnaire
void filterDictionary(const char *guess, int feedback[WORD_LEN]) {
    int k = 0;

    for (int i = 0; i < wordCount; i++) {
        if (isWordCompatible(dictionary[i], guess, feedback)) {
            strcpy(dictionary[k++], dictionary[i]);
        }
    }

    wordCount = k;
}