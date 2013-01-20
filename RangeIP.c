//
//  Range IP
//
//  Énoncé :
//
//      En C, vous devez écrire un programme qui lit un ensemble de fichiers blacklist et qui fourni en sortie un seul fichier minimal.
//
//      /!\ Les fichiers de blacklist sont à passer en paramètre /!\
//
//      Un range d'ip (plage d'ip) à bannir correspond à une ligne dans un fichier, sous la forme : a.b.c.d-a.b.c.d 
//
//      Exemple :
//
//      Input
//      3.201.0.0-3.255.255.255
//      4.0.25.146-4.0.25.148
//      4.0.26.14-4.0.29.24
//      4.2.144.64-4.2.144.95
//      3.0.0.0-3.200.255.255
//      4.2.144.224-4.2.144.231
//      4.2.144.248-4.2.144.255
//      4.2.145.224-4.2.145.239
//      3.100.255.0-3.110.111.111
//
//      Output
//      3.0.0.0-3.255.255.255
//      4.0.25.146-4.0.25.148
//      4.0.26.14-4.0.29.24
//      4.2.144.64-4.2.144.95
//      4.2.144.224-4.2.144.231
//      4.2.144.248-4.2.144.255
//      4.2.145.224-4.2.145.239
//
//
//  Created by MrAaaah on 19/01/13.
//  Copyright (c) 2013 MrAaaah. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

struct IP {
    int a, b, c, d; // a.b.c.d
};

struct IPRangeList {
    struct IP *startIP;
    struct IP *endIP;
    struct IPRangeList *nextRange;
};

typedef struct IP IP;
typedef struct IPRangeList IPRangeList;

// liste chainée contenant les range
IPRangeList *list = NULL;

void addRange(IP *startRange, IP *endRange);
void displayRanges();
int ipALowerOrEqualThanB(IP *a, IP *b);  // compare deux adress ip retourn 1 si l'adresse a est plus basse ou identique à b
void mergeRanges(); // supprime et merge les plages d'ip
void cleaning(); // suppression des variable dynamiques

int main(int argc, const char * argv[])
{
    if (argc < 2) {
        printf("Erreur d'utilisation : ./RangeIP <blacklistFile ...>");
        return 1;
    }
 
    // parcours des fichiers blacklist pour récupérer les ranges + tri
    for (int i = 1; i < argc; i++) {
        FILE* fp;
        char line[35];
        fp = fopen(argv[i], "r");
        
        while(fgets(line, 35, fp) != NULL)
        {
            IP *startRange = malloc(sizeof(IP)), *endRange = malloc(sizeof(IP));
            sscanf (line, "%i.%i.%i.%i-%i.%i.%i.%i", &startRange->a, &startRange->b, &startRange->c, &startRange->d, &endRange->a, &endRange->b, &endRange->c, &endRange->d);
            addRange(startRange, endRange);
        }
        
        // fermeture
        fclose(fp);
    }
    
    // affichage de la liste triée
    displayRanges();
    
    mergeRanges();

    displayRanges();
    
    cleaning();
    
    return 0;
}

// ajout du range à la liste triée
void addRange(IP *start, IP *end) {
    IPRangeList *temp = malloc(sizeof(IPRangeList));
    temp->startIP = start;
    temp->endIP = end;
    
    // liste vide
    if (list == NULL) {
        list = temp;
    }
    else {
        IPRangeList *iterator = list;
        
        // parcours des ranges pour trouver la bonne place
        do {
            // le range débute plus tôt que celui en tête de liste
            if (ipALowerOrEqualThanB(temp->startIP, iterator->startIP) && (iterator == list)) {
                temp->nextRange = iterator;
                list = temp;
                break;
            }
            // fin de liste (range le plus grand pour le moment)
            else if (iterator->nextRange == NULL){
                iterator->nextRange = temp;
                break;
            }
            // le range débute avant le procahin range et après le courant (insertion après iterateur)
            else if (ipALowerOrEqualThanB(temp->startIP, iterator->nextRange->startIP)) {
                temp->nextRange = iterator->nextRange;
                iterator->nextRange = temp;
                break;
            }
            
            iterator = iterator->nextRange; // parcours
        } while (iterator != NULL);
    }
}

// comparaison de deux ip (a <= b)
int ipALowerOrEqualThanB(IP *a, IP *b) {
    if (a->a < b->a)
        return 1;
    else if (a->a > b->a)
        return 0;
    else
        if (a->b < b->b)
            return 1;
        else if (a->b > b->b)
            return 0;
        else
            if (a->c < b->c)
                return 1;
            else if (a->c > b->c)
                return 0;
            else
                if (a->d <= b->d)
                    return 1;
    
    return 0;
}

// affichage de la liste de plages ip
void displayRanges() {
    IPRangeList *iterator = list;
    
    do {
        printf("%i.%i.%i.%i -> %i.%i.%i.%i\n", iterator->startIP->a, iterator->startIP->b, iterator->startIP->c, iterator->startIP->d, iterator->endIP->a, iterator->endIP->b, iterator->endIP->c, iterator->endIP->d);
        iterator = iterator->nextRange;
    } while (iterator != NULL);
    
    printf("\n");
}

// supprime et merge les plages d'ip
void mergeRanges() {
    IPRangeList *iterator = list;
    
    do {
        // le début du range suivant est avnt la fin du courant ou identique
        if (ipALowerOrEqualThanB(iterator->nextRange->startIP, iterator->endIP)) {
            // la fin du range suivant est plus petit ou égal que la fin du range courant (range suivant appartient au courant)
            if (ipALowerOrEqualThanB(iterator->nextRange->endIP, iterator->endIP)) {
                // suppression du suivant
                IPRangeList *temp = iterator->nextRange;
                
                iterator->nextRange = temp->nextRange;
            }
            // intersection de deux ranges, fusion des deux ranges
            else {
                IPRangeList *temp = iterator->nextRange;
                
                iterator->endIP = temp->endIP;
                iterator->nextRange = temp->nextRange;
            }
        }
        else {
            iterator = iterator->nextRange;
        }
        
    } while (iterator->nextRange != NULL);
}

void cleaning() {
    do {
        IPRangeList *temp = list;

        list = list->nextRange;
        free(temp->startIP);
        free(temp->endIP);
        free(temp);
    } while (list != NULL);
    free(list);
}
