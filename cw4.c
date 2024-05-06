#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_CONSUMER 4
#define BUFFER_PRODUCER 8

int main() 
{
    int fd[2];
    pid_t pid;

    // Tworzymy potok nienazwany
    if (pipe(fd) == -1) 
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

        
    pid = fork();

    if (pid < 0) 
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) 
    {
        // Proces macierzysty (Producent)

        // Otwieramy plik producenta
        FILE* input = fopen("producent.txt", "r");
        if (input == NULL) 
        {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        // Odczytujemy dane z pliku producenta i umieszczamy je w potoku
        char buffer[BUFFER_PRODUCER];
        size_t bytesRead;
        while ((bytesRead = fread(buffer, sizeof(char), BUFFER_PRODUCER, input)) > 0) 
        {
            if (write(fd[1], buffer, bytesRead) == -1) 
            {
                perror("write pipe");
                exit(EXIT_FAILURE);
            }
            char* message = (char*)malloc(100 * sizeof(char));
            sprintf(message, "Producer: Sent %zu bytes, content: %s\n", bytesRead, buffer);
            write(STDOUT_FILENO, message, strlen(message));
            free(message);
            sleep(rand() % 3); // Losujemy szybkość operacji
        }

        // Zamykamy plik producenta i kończymy zapisywanie do potoku
        fclose(input);
        close(fd[1]);
        wait(NULL);
    } 
    else 
    {
        // Proces potomny (Consumer)

        // Zamykamy zapis do potoku
        close(fd[1]);

        // Otwieramy plik konsumenta
        FILE* output = fopen("konsument.txt", "w");
        if (output == NULL) 
        {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        // Odczytujemy z potoku i zapisujemy do pliku konsumenta
        char buffer[BUFFER_CONSUMER];
        ssize_t bytesRead;
        while ((bytesRead = read(fd[0], buffer, BUFFER_CONSUMER)) > 0) 
        {
            if (fwrite(buffer, sizeof(char), bytesRead, output) != bytesRead) 
            {
                perror("fwrite");
                exit(EXIT_FAILURE);
            }
            // Tworzymy dynamiczny bufor w którym zapisujemy informacje o odczytanych danych a następnie zwalniamy go gdy już wypiszemy jego zawartość na ekran
            char* message = (char*)malloc(100 * sizeof(char));
            sprintf(message, "Consumer: Received %zd bytes, content: %s\n", bytesRead, buffer);
            write(STDOUT_FILENO, message, strlen(message));
            free(message);
            sleep(rand() % 3); // Losujemy szybkość operacji
        }

        // Zamykamy plik konsumenta i odczytujemy z potoku
        fclose(output);
        close(fd[0]);
    }

    return 0;
}