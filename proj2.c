#include<stdlib.h>
#include<stdio.h>
#include<semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include<unistd.h>
#include<time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>

#define semNAMEanum "/xlamac01_ios_p2_sem_anum"
#define semNAMEhackers "/xlamac01_ios_p2_sem_hackres"
#define semNAMEserfs "/xlamac01_ios_p2_sem_serfs"
#define semNAMEmolo "/xlamac01_ios_p2_sem_molo"
#define semNAMEmembers "/xlamac01_ios_p2_sem_members"
#define semNAMEcpts "/xlamac01_ios_p2_sem_cpts"
#define semNAMEhaQ "/xlamac01_ios_p2_sem_hackQ"
#define semNAMEseQ "/xlamac01_ios_p2_sem_serfQ"
#define semNAMEmem1 "/xlamac01_ios_p2_sem_mem1"
#define semNAMEmem2 "/xlamac01_ios_p2_sem_mem2"
#define semNAMEmem3 "/xlamac01_ios_p2_sem_mem3"
#define LOCKED 0
#define UNLOCKED 1

FILE *output;
int serfsId = 0;
int *serfs = NULL;
int hackersId = 0;
int *hackers = NULL;
int AnumId = 0;
int *Anum = NULL;
int membersId = 0;
int *members = NULL;

sem_t *sem_anum = NULL;
sem_t *sem_serfs = NULL;
sem_t *sem_hackers = NULL;
sem_t *sem_molo = NULL;
sem_t *sem_boatMember = NULL;
sem_t *sem_cptSleeps = NULL;
sem_t *sem_hackerQueue = NULL;
sem_t *sem_serfQueue = NULL;


int Init()
{
    // sdielen promenenne
    serfsId = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    serfs = shmat(serfsId, NULL, 0);
    hackersId = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    hackers = shmat(hackersId, NULL, 0);
    AnumId = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    Anum = shmat(AnumId, NULL, 0);
    membersId = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    members = shmat(membersId, NULL, 0);

    *serfs = 0;
    *hackers = 0;
    *Anum = 0;
    *members = 0;

    // output file    
    output = fopen("proj2.out", "w");
    setbuf(output, NULL);

    // test semaforu
    if ((sem_anum = sem_open(semNAMEanum, O_CREAT | O_EXCL, 0644, UNLOCKED)) == SEM_FAILED)
    {
        return -1;
    }
    if ((sem_serfs = sem_open(semNAMEserfs, O_CREAT | O_EXCL, 0644, UNLOCKED)) == SEM_FAILED)
    {
        return -1;
    }
    if ((sem_hackers = sem_open(semNAMEhackers, O_CREAT | O_EXCL, 0644, UNLOCKED)) == SEM_FAILED)
    {
        return -1;
    }
    if ((sem_molo = sem_open(semNAMEmolo, O_CREAT | O_EXCL, 0644, UNLOCKED)) == SEM_FAILED) 
    {
        return -1;
    }
    if ((sem_cptSleeps = sem_open(semNAMEcpts, O_CREAT | O_EXCL, 0644, LOCKED)) == SEM_FAILED) 
    {
        return -1;
    }
    if ((sem_boatMember = sem_open(semNAMEmembers, O_CREAT | O_EXCL, 0644, LOCKED)) == SEM_FAILED) 
    {
        return -1;
    }
    if ((sem_hackerQueue = sem_open(semNAMEhaQ, O_CREAT | O_EXCL, 0644, LOCKED)) == SEM_FAILED) 
    {
        return -1;
    }
    if ((sem_serfQueue = sem_open(semNAMEseQ, O_CREAT | O_EXCL, 0644, LOCKED)) == SEM_FAILED) 
    {
        return -1;
    }
    
    return 0;
}

void Clean()
{
    // sdilene promenne
    shmctl(serfsId, IPC_RMID, NULL);
    shmctl(hackersId, IPC_RMID, NULL);
    shmctl(AnumId, IPC_RMID, NULL);
    shmctl(membersId, IPC_RMID, NULL);

    // semafory
    sem_close(sem_anum);
    sem_close(sem_hackers);
    sem_close(sem_serfs);
    sem_close(sem_molo);
    sem_close(sem_boatMember);
    sem_close(sem_cptSleeps);
    sem_close(sem_hackerQueue);
    sem_close(sem_serfQueue);
    sem_unlink(semNAMEanum);
    sem_unlink(semNAMEhackers);
    sem_unlink(semNAMEserfs);
    sem_unlink(semNAMEmolo);
    sem_unlink(semNAMEmembers);
    sem_unlink(semNAMEcpts);
    sem_unlink(semNAMEhaQ);
    sem_unlink(semNAMEseQ); 

    // output file
    if (output != NULL)
    {
        fclose(output);
    }
}

int GetNUmber(int lower, int upper) // vygeneruje nahodne cislo v danem rozsahu 
{ 
    int num = (rand() % (upper - lower + 1)) + lower;
    return num; 
} 

void CheckArgs (int argc, char const *argv[]) // provadi kontrolu argumentu
{
    char *ptr;
    if (argc < 7) // Kontrola poctu argumentu
    {        
        fprintf(stderr, "Bylo zadano malo argumnetu\n");
        exit(1);
    }

    if (argc > 7) // Kontrola poctu argumentu
    {        
        fprintf(stderr, "Bylo zadano prilis mnoho argumnetu\n");
        exit(1);
    }

    if ( strtol(argv[1], &ptr, 10) < 2 || ((strtol(argv[1], &ptr, 10) % 2) != 0))    // Kontola P
    {        
        fprintf(stderr, "Argument P nevyhpvuje\n");
        exit(1);
    }

    if (strtol(argv[2], &ptr, 10) < 0 || (strtol(argv[2], &ptr, 10) > 2000))    // Kontola H
    {        
        fprintf(stderr, "Argument H nevyhpvuje\n");
        exit(1);
    }

    if (strtol(argv[3], &ptr, 10) < 0 || (strtol(argv[3], &ptr, 10) > 2000))    // Kontola S
    {        
        fprintf(stderr, "Argument S nevyhpvuje\n");
        exit(1);
    }

    if (strtol(argv[4], &ptr, 10) < 0 || (strtol(argv[4], &ptr, 10) > 2000))    // Kontola R
    {        
        fprintf(stderr, "Argument R nevyhpvuje\n");
        exit(1);
    }

    if (strtol(argv[5], &ptr, 10) < 20 || (strtol(argv[5], &ptr, 10) > 2000))    // Kontola W
    {        
        fprintf(stderr, "Argument H nevyhpvuje\n");
        exit(1);
    }  
    
    if (strtol(argv[6], &ptr, 10) < 5)    // Kontola C
    {        
        fprintf(stderr, "Argument C nevyhpvuje\n");
        exit(1);
    }
}

int main(int argc, char const *argv[])
{    
    CheckArgs(argc, argv); // kontroluje, zda jsou argumenty validni
    
    char *ptr;
    int IsCpt = 0; // promenna se pouziva jako bool
    int moloCap = strtol(argv[6], &ptr, 10);
    if (Init() == -1) // konrola inicialnize sdilenych promennych a semaforu
    {
        Clean();
        return 1;
    }

    srand(time(0)); // Seed pro generator   

    if (fork() == 0) // generator serfs
    {
        for (int i = 1; i <= strtol(argv[1], &ptr, 10); i++) // indexovani od jednicky 
        {
            if (fork() == 0)
            {
                sem_wait(sem_anum);
                fprintf(output, "%d: SERF %d: starts\n", ++(*Anum), i);
                sem_post(sem_anum);                             

                while (1) // jde k molu
                {
                    sem_wait(sem_molo);
                    sem_wait(sem_anum);
                    sem_wait(sem_hackers);
                    sem_wait(sem_serfs);
                    while(*serfs + *hackers < moloCap  ) // vstup na molo
                    {                       
                        fprintf(output, "%d: SERF %d: waits: %d: %d\n", ++(*Anum), i, *hackers, ++(*serfs) );
                        
                        if (*serfs == 4)
                        {
                            *serfs -= 4;
                            sem_post(sem_serfQueue);
                            sem_post(sem_serfQueue);
                            sem_post(sem_serfQueue);
                            sem_post(sem_serfQueue);
                            IsCpt = 1;
                        }
                        else if (*hackers >= 2 && *serfs == 2)
                        {
                            *hackers -= 2;
                            *serfs -= 2;
                            sem_post(sem_hackerQueue);
                            sem_post(sem_hackerQueue);
                            sem_post(sem_serfQueue);
                            sem_post(sem_serfQueue);
                            IsCpt = 1;
                        }
                        else
                        {
                            sem_post(sem_molo);
                        }

                        
                        sem_post(sem_serfs); // pro manipulaci s promennou serfs
                        sem_post(sem_hackers); // pro manipulaci s promennou hackers
                        sem_post(sem_anum); // pro manipulaci s promennou anum

                        sem_wait(sem_serfQueue);                        

                        if (IsCpt)
                        {
                            *members = 3; // kapitan veze dalsi tri lidi
                            sem_wait(sem_anum);
                            fprintf(output, "%d: SERF %d: boards: %d: %d\n", ++(*Anum), i, *hackers, *serfs);
                            sem_post(sem_anum);
                            usleep(GetNUmber(0, strtol(argv[4], &ptr, 10))*1000);

                            sem_post(sem_cptSleeps);
                            sem_post(sem_cptSleeps);
                            sem_post(sem_cptSleeps);

                            sem_wait(sem_boatMember);
                            sem_wait(sem_anum);
                            fprintf(output, "%d: SERF %d: captain exits: %d: %d\n", ++(*Anum), i, *hackers, *serfs);
                            sem_post(sem_anum);
                            
                            sem_post(sem_molo);
                            IsCpt = 0; 
                            exit(0);                           
                        }
                        else
                        {
                            sem_wait(sem_cptSleeps);
                            sem_wait(sem_anum);
                            fprintf(output, "%d: SERF %d: member exits: %d: %d\n", ++(*Anum), i, *hackers, *serfs);
                            sem_post(sem_anum);

                            if (--(*members) == 0)
                            {
                                sem_post(sem_boatMember);
                            }
                            exit(0);
                        }
                    }                    

                    sem_wait(sem_anum);
                    fprintf(stdout, "%d: SERF %d: leaves queue: %d: %d\n", ++(*Anum), i, *hackers, *serfs);
                    sem_post(sem_anum);
                    usleep(GetNUmber(20, strtol(argv[5], &ptr, 10)));

                    sem_wait(sem_anum);
                    fprintf(stdout, "%d: SERF %d: is back\n", ++(*Anum), i);
                    sem_post(sem_anum);                    

                    sem_post(sem_molo);
                }
                exit(0);                             
            }
            usleep(GetNUmber(0, strtol(argv[3], &ptr, 10))*1000);
        }
        sleep(2);
        exit(0);
    }    
    
    if (fork() == 0) // generator hackers
    {        
        for (int j = 1; j <= strtol(argv[1], &ptr, 10); j++) // indexovani od jednicky
        {           
            if (fork() == 0)
            {
                sem_wait(sem_anum);
                fprintf(output, "%d: HACK %d: starts\n", ++(*Anum), j);
                sem_post(sem_anum);
                                
                while (1) // chodi k molu
                {
                    sem_wait(sem_molo); // vstup na molo
                    sem_wait(sem_anum);
                    sem_wait(sem_hackers);
                    sem_wait(sem_serfs);
                    while(*serfs + *hackers < moloCap  ) // vstup na molo
                    {                        
                        fprintf(output, "%d: HACK %d: waits: %d: %d\n", ++(*Anum), j, ++(*hackers), *serfs );
                        
                        if (*hackers == 4)
                        {
                            *hackers -= 4;
                            sem_post(sem_hackerQueue);
                            sem_post(sem_hackerQueue);
                            sem_post(sem_hackerQueue);
                            sem_post(sem_hackerQueue);
                            IsCpt = 1;
                        }
                        else if (*hackers == 2 && *serfs >= 2)
                        {
                            *hackers -= 2;
                            *serfs -= 2;
                            sem_post(sem_hackerQueue);
                            sem_post(sem_hackerQueue);
                            sem_post(sem_serfQueue);
                            sem_post(sem_serfQueue);
                            IsCpt = 1;
                        }
                        else
                        {
                            sem_post(sem_molo);
                        }

                        sem_post(sem_serfs); // pro manipulaci s promennou serfs
                        sem_post(sem_hackers); // pro manipulaci s promennou hackers
                        sem_post(sem_anum); // pro manipulaci s promennou anum 

                        sem_wait(sem_hackerQueue);

                        if (IsCpt)
                        {
                            *members = 3; // kapitan veze dalsi tri lidi
                            sem_wait(sem_anum);
                            fprintf(output, "%d: HACK %d: boards: %d: %d\n", ++(*Anum), j, *hackers, *serfs);
                            sem_post(sem_anum);

                            usleep(GetNUmber(0, strtol(argv[4], &ptr, 10))*1000);
                            sem_post(sem_cptSleeps);
                            sem_post(sem_cptSleeps);
                            sem_post(sem_cptSleeps);

                            sem_wait(sem_boatMember);
                            sem_wait(sem_anum);
                            fprintf(output, "%d: HACK %d: captain exits: %d: %d\n", ++(*Anum), j, *hackers, *serfs);
                            sem_post(sem_anum);
                            
                            sem_post(sem_molo);
                            IsCpt = 0; 
                            exit(0);                           
                        }
                        else
                        {                            
                            sem_wait(sem_cptSleeps);
                            sem_wait(sem_anum);
                            fprintf(output, "%d: HACK %d: member exits: %d: %d\n", ++(*Anum), j, *hackers, *serfs);
                            sem_post(sem_anum);

                            if (--(*members) == 0)
                            {
                                sem_post(sem_boatMember);
                            }
                            exit(0);
                        }
                    }                    

                    sem_wait(sem_anum);
                    fprintf(output, "%d: HACK %d: leaves queue: %d: %d\n", ++(*Anum), j, *hackers, *serfs);
                    sem_post(sem_anum);
                    usleep(GetNUmber(20, strtol(argv[5], &ptr, 10))*1000);
                    sem_wait(sem_anum);
                    fprintf(output, "%d: HACK %d: is back\n", ++(*Anum), j);
                    sem_post(sem_anum);
                    
                    sem_post(sem_molo);
                }
                
                exit(0);
            }

            usleep(GetNUmber(0, strtol(argv[2], &ptr, 10))*1000);
        }
        sleep(2);
        exit(0);
    } 
    sleep(3);
    
    Clean(); // uklidi po sobe
    return 0;
}