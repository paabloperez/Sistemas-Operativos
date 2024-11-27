#ifndef LISTA_H
#define LISTA_H

#define MAX 1024

typedef struct HistNode {
    char *trozos[MAX];
    struct HistNode *next;
} HistNode;

typedef struct {
    HistNode *head;
    int size;
} HistList;

void CreateHistList(HistList *list);
void InsertHistElement(HistList *list, char *tokens[]);
void PrintHistList(HistList *list);
void PrintLastNCommands(HistList *list, int n);
void FreeHistList(HistList *list);
HistNode* GetCommandByNumber(HistList *list, int n);

#endif
