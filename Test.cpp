#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#define PORT 3490

int sock1 = 0, sock2 = 0;

void signal_handler(int sig) {
	if(sig == SIGINT) {
		printf("terminating test\n");
        send(sock1, "EXIT", sizeof("EXIT"), 0);
		exit(0);
	}
}

int main() {

    signal(SIGINT, &signal_handler);

    int valread;
	struct sockaddr_in serv_addr;
	char buffer[1024] = { 0 };
	if ((sock1 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
	}
    if ((sock2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
		<= 0) {
		printf(
			"\nInvalid address/ Address not supported \n");
	}

	if (connect(sock1, (struct sockaddr*)&serv_addr,
				sizeof(serv_addr))
		< 0) {
		printf("\nConnection Failed \n");
	}
    sleep(0.5);
    if (connect(sock2, (struct sockaddr*)&serv_addr,
				sizeof(serv_addr))
		< 0) {
		printf("\nConnection Failed \n");
	}

    char text1[1025];
    char text2[1025];

    printf("STARTING TESTS\n");

    send(sock1, "PUSH hello world!", sizeof("PUSH hello world!"), 0);
    sleep(1);
    send(sock2, "PUSH hi bob", sizeof("PUSH hi bob"), 0);
    sleep(1);
    send(sock1, "TOP", sizeof("TOP"), 0); //top should be "hi bob"
    sleep(1);
    recv(sock1, text1, 1025, 0);
    assert(strcmp(text1, "hi bob") == 0);
    printf("TEST 1 DONE\n");
    send(sock1, "PUSH second line in stack", sizeof("PUSH second line in stack"), 0); 
    sleep(1);
    send(sock2, "TOP", sizeof("TOP"), 0); //top should be "second line in stack"
    sleep(1);
    recv(sock2, text2, 1025, 0);
    assert(strcmp(text2, "second line in stack") == 0);
    printf("TEST 2 DONE\n");
    send(sock1, "POP", sizeof("POP"), 0); 
    sleep(1);
    send(sock2, "POP", sizeof("POP"), 0); 
    sleep(1);
    send(sock1, "TOP", sizeof("TOP"), 0); //top should be "hello world"
    sleep(1);
    recv(sock1, text1, 1025, 0);
    assert(strcmp(text1, "hello world!") == 0);
    printf("TEST 3 DONE\n");
    send(sock2, "POP", sizeof("POP"), 0);   //nothing should be in stack
    sleep(1);
    send (sock1, "TOP", sizeof("TOP"), 0); //should return error
    sleep(1);
    recv(sock1, text1, 1025, 0);
    assert(strcmp(text1, "<ERROR: stack is empty>") == 0);
    printf("TEST 4 DONE\n");
    send(sock1, "POP", sizeof("POP"), 0); // popping empty stack
    sleep(1);
    send(sock1, "PUSH she sells seashells by the sea shore", sizeof("PUSH she sells seashells by the sea shore"), 0);
    sleep(1);
    send(sock1, "PUSH three smart fellows, they felt smart", sizeof("PUSH three smart fellows, they felt smart"), 0);
    sleep(1);
    send(sock1, "PUSH the itsy bitsy spider", sizeof("PUSH the itsy bitsy spider"), 0);    
    sleep(1);
    send(sock2, "TOP", sizeof("TOP"), 0); //top should  be "the itspy bitsy spider"
    sleep(1);
    recv(sock2, text2, 1025, 0);
    assert(strcmp(text2, "the itsy bitsy spider") == 0);
    printf("TEST 5 DONE\n");
    send(sock2, "POP", sizeof("POP"), 0); 
    sleep(1);
    send(sock1, "TOP", sizeof("TOP"), 0); //top should be "three smart fellows, they felt smart"
    sleep(1);
    recv(sock1, text1, 1025, 0);
    assert(strcmp(text1, "three smart fellows, they felt smart") == 0);
    printf("TEST 6 DONE\n");
    send(sock1, "POP", sizeof("POP"), 0); 
    sleep(1);
    send(sock2, "TOP", sizeof("TOP"), 0); //top should be "she sells seashells by the sea shore"
    sleep(1);
    recv(sock2, text2, 1025, 0);
    assert(strcmp(text2, "she sells seashells by the sea shore") == 0);
    printf("TEST 7 DONE\n");
    send(sock1, "EXIT", sizeof("EXIT"), 0);
    sleep(1);
    send(sock2, "EXIT", sizeof("EXIT"), 0);
    close(sock1);

    printf("DONE\n");

    return 0;
}