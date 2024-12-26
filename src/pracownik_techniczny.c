#include <pracownik_techniczny.h>
#include <semafory.h>
#include <pamiec_dzielona.h>
#include <unistd.h>

int main() {
    int id_sem = create_new_semaphore();
    int id_shm = create_new_shared_memory();
    char * address = join_shared_memory(id_shm);
    int K;

    printf("Podaj liczbe kibicow majacych wejsc na stadion: ");
    scanf("%d", &K);

    if (K <= 0) {
        printf("Podana liczba kibicow jest nieprawidlowa!\n");
        return 1;
    }

    set_semaphore(id_sem, 0, K);
    set_semaphore(id_sem, 1, 1);

    handle_semaphore_p(id_sem, 1);
    * address = K;
    handle_semaphore_v(id_sem, 1);

    pid_t pid = fork();

    switch(pid) {
        case -1:
            printf("Blad utworzenia nowego procesu!\n");
            perror("fork");
            exit(EXIT_FAILURE);
            break;
        case 0:
            execl("./bin/kibic", "kibic", NULL);
            break;
    }

    wait(NULL);

    printf("Kibic zakonczyl swoje dzialanie.\n");

    detach_shared_memory(address);
    delete_shared_memory(id_shm);
    delete_semaphore(id_sem);
    return 0;
}
