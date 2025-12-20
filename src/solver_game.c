#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "solver_game.h"

char dictionary[MAX_WORDS][WORD_LEN + 1];
int wordCount = 0;

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

void computeFeedback(const char *secret, const char *guess, int fb[WORD_LEN]) {
    int used[WORD_LEN] = {0};

    for (int i = 0; i < WORD_LEN; i++) {
        if (guess[i] == secret[i]) {
            fb[i] = GREEN;
            used[i] = 1;
        } else {
            fb[i] = GRAY;
        }
    }

    for (int i = 0; i < WORD_LEN; i++) {
        if (fb[i] == GRAY) {
            for (int j = 0; j < WORD_LEN; j++) {
                if (!used[j] && guess[i] == secret[j]) {
                    fb[i] = YELLOW;
                    used[j] = 1;
                    break;
                }
            }
        }
    }
}

int isWordValid(const char *word, const char *guess, int fb[WORD_LEN]) {
    int tmp[WORD_LEN];
    computeFeedback(word, guess, tmp);
    for (int i = 0; i < WORD_LEN; i++)
        if (tmp[i] != fb[i]) return 0;
    return 1;
}

void filterWords(const char *guess, int fb[WORD_LEN]) {
    int k = 0;
    for (int i = 0; i < wordCount; i++) {
        if (isWordValid(dictionary[i], guess, fb)) {
            strcpy(dictionary[k++], dictionary[i]);
        }
    }
    wordCount = k;
}