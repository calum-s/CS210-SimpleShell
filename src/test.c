#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"

void test_tokenizer(void);

int main() {
    printf("Running tests...\n");

    printf("Testing tokenizer...");
    test_tokenizer();
    printf("OK\n");

    printf("All tests passed!\n");
}

void test_tokenizer(void) {
    char* test1 = "";
    TokenList list = tokenize(test1);
    assert(list.size == 0);

    char* test2 = "ls -l";
    list = tokenize(test2);
    assert(list.size == 2);
    assert(strncmp(list.tokens[0].start, "ls", list.tokens[0].length) == 0);
    assert(strncmp(list.tokens[1].start, "-l", list.tokens[1].length) == 0);
    free_token_list(&list);

    char* test3 = "ls  \t -l | grep test";
    list = tokenize(test3);
    assert(list.size == 4);
    assert(strncmp(list.tokens[0].start, "ls", list.tokens[0].length) == 0);
    assert(strncmp(list.tokens[1].start, "-l", list.tokens[1].length) == 0);
    assert(strncmp(list.tokens[2].start, "grep", list.tokens[2].length) == 0);
    assert(strncmp(list.tokens[3].start, "test", list.tokens[3].length) == 0);
    free_token_list(&list);

    char* test4 = "ls\t-lf;&..>.<..|/\tfksdk";
    list = tokenize(test4);
    assert(list.size == 7);
    assert(strncmp(list.tokens[0].start, "ls", list.tokens[0].length) == 0);
    assert(strncmp(list.tokens[1].start, "-lf", list.tokens[1].length) == 0);
    assert(strncmp(list.tokens[2].start, "..", list.tokens[2].length) == 0);
    assert(strncmp(list.tokens[3].start, ".", list.tokens[3].length) == 0);
    assert(strncmp(list.tokens[4].start, "..", list.tokens[4].length) == 0);
    assert(strncmp(list.tokens[5].start, "/", list.tokens[5].length) == 0);
    assert(strncmp(list.tokens[6].start, "fksdk", list.tokens[6].length) == 0);
    free_token_list(&list);
}
