#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "card.h"

void build_index();
CARD_T read_card(INDEX_T**);
void print_card(CARD_T);
void free_index(INDEX_T*);

int find_compar(const void* a, const void* b) {
	const char** aptr = (const char**) a;
	INDEX_T** bptr = (INDEX_T**) b;
	return strcmp(*aptr, (*bptr)->name);
}

INDEX_T** indexes = NULL;
size_t total_cards = 0;

int main() {
	char* line = NULL;
	char* usr_str;
	size_t len = 0;
	ssize_t lineSize = 0;
	INDEX_T** card_match = NULL;
	
	// Read everthing from index.bin
	build_index(); 
	
	//runs until q is last char entered
	while(lineSize == 0 || line[lineSize - 2]  != 'q') {
		// User input
		printf(">> ");
		lineSize = getline(&line, &len, stdin);
		usr_str = strndup(line, lineSize - 1);
		if (isatty(0) == 0) printf("%s", line);

		// Sees if any card name matches user input
		card_match = bsearch(&usr_str, indexes, total_cards, sizeof(INDEX_T*), find_compar);

		if(card_match != NULL) {
			CARD_T card = read_card(card_match);
			print_card(card);
			free(card.text);
			free(card.name);
		} else if(line[lineSize - 2]  != 'q') {
			printf("./search: '%s' not found!\n", usr_str);
		}
		
		free(usr_str);
	}
	
	
	for(int i = 0; i < total_cards; ++i) {
		free_index(indexes[i]);
	}
	
	free(indexes);
	
	free(line);

	return 0;
}

void free_index(INDEX_T* i) {
	free(i->name);
	free(i);
}

void build_index() {
	FILE* bin = fopen("index.bin", "rb");
	int name_len;
	
	// Read the number of cards from file
	fread(&total_cards, sizeof(size_t), 1, bin); 
	
	// Loop through and adds all names and offsets to indexes
	for(int i = 0; i < total_cards; i++) {
		indexes = realloc(indexes, (i + 1) * sizeof(INDEX_T*));
		indexes[i] = malloc(sizeof(INDEX_T));  
		
		fread(&name_len, sizeof(int), 1, bin);
		indexes[i]->name = malloc((name_len + 1) * sizeof(char));
		fread(indexes[i]->name, sizeof(char), name_len, bin);
		indexes[i]->name[name_len] = 0;
		fread(&indexes[i]->offset, sizeof(long), 1, bin);
	}
	
	fclose(bin);
}

CARD_T read_card(INDEX_T** sel_card) {
	FILE* cards_bin = fopen("cards.bin", "rb");
	
	CARD_T card;
	int text_len;
	
	card.name = strdup(sel_card[0]->name);
	fseek(cards_bin, sel_card[0]->offset, SEEK_SET);

	// Read a single card's info from cards.bin
	fread(&card.id, sizeof(unsigned), 1, cards_bin); 
	fread(&card.cost, sizeof(unsigned), 1, cards_bin); 
	fread(&card.type, sizeof(unsigned), 1, cards_bin); 
	fread(&card.class, sizeof(unsigned), 1, cards_bin); 
	fread(&card.rarity, sizeof(unsigned), 1, cards_bin);
	fread(&text_len, sizeof(int), 1, cards_bin);
	card.text = malloc((text_len + 1) * sizeof(char));  
	fread(card.text, sizeof(char), text_len, cards_bin); 
	card.text[text_len] = 0;
	fread(&card.attack, sizeof(unsigned), 1, cards_bin); 
	fread(&card.health, sizeof(unsigned), 1, cards_bin); 
	
	fclose(cards_bin);
	
	return card;
}

void print_card(CARD_T card) {
	printf("%-29s %2d\n", card.name, card.cost);
	unsigned length = 15 - strlen(class_str[card.class]);
	unsigned remainder = length % 2;
	unsigned margins = length / 2;
	unsigned left = 0;
	unsigned right = 0;
	if (remainder) {
		left = margins + 2;
		right = margins - 1;
	} else {
		left = margins + 1;
		right = margins - 1;
	}
	printf("%-6s %*s%s%*s %9s\n", type_str[card.type], left, "", class_str[card.class], right, "", rarity_str[card.rarity]);
	printf("--------------------------------\n");
	printf("%s\n", card.text);
	printf("--------------------------------\n");
	printf("%-16d%16d\n\n", card.attack, card.health);
}
