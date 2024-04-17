#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arch_logic.h"

symbol* makeTree(symbol* psym[], int k) {
    symbol* temp = (symbol*)malloc(sizeof(symbol));
    if (k > 1) {
        temp->ch = psym[k - 2]->ch;
        temp->left = psym[k - 2];
        temp->right = psym[k - 1];
        temp->freq = psym[k - 2]->freq + psym[k - 1]->freq;
        sprintf(temp->code, "");
        psym[k - 2] = temp;
        descend_sort(psym, k - 1);
        return makeTree(psym, k - 1);
    }
    else return psym[0];
}

void makeCodes(symbol* root) {
    symbol* left = root->left, * right = root->right;
    if (left != NULL) {
        sprintf(left->code, "%s%c", root->code, '0');
        makeCodes(left);
        free(root);
    }
    if (right != NULL) {
        sprintf(right->code, "%s%c", root->code, '1');
        makeCodes(right);
        free(root);
    }
}

void descend_sort(symbol* psym[], int k) {
    for (int i = 0; i < k; i++) {
        float max = psym[i]->freq;
        int ind = i;
        for (int j = i + 1; j < k; j++) {
            if (psym[j]->freq > max) {
                max = psym[j]->freq;
                ind = j;
            }
        }
        if (i != ind) {
            symbol* tmp;
            tmp = psym[i];
            psym[i] = psym[ind];
            psym[ind] = tmp;
        }
    }
}