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


    detach_shared_memory(address);

    while(1) {
        sleep(1);
    }

    return 0;
}
