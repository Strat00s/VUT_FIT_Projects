#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>

#define OUTPUT "proj2.out"

#define SHVAR(var) var = mmap(NULL, sizeof(*var), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)
#define SEMINIT(sem, sem_name) (sem = sem_open(sem_name, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1))
#define WAITFOR(max_time) usleep((rand() % (max_time - 0 + 1) + 0)*1000 )

/*---(SEMAPHORES)---*/
#define SEM_ENTER "/xbasty00-IOS2-sem_enter"
#define SEM_WRITE "/xbasty00-IOS2-sem_write"
#define SEM_REGISTER "/xbasty00-IOS2-sem_register"
#define SEM_CHECKED "/xbasty00-IOS2-sem_checked"
#define SEM_JDG_DONE "/xbasty00-IOS2-sem_jdg_done"
sem_t *sem_enter = NULL;
sem_t *sem_write = NULL;
sem_t *sem_register = NULL;
sem_t *sem_checked = NULL;
sem_t *sem_jdg_done = NULL;

typedef struct {
    int img_max;
    int img_gen_delay;
    int jdg_ent_delay;
    int img_ret_time;
    int jdg_gen_time;
} schedule_t;

/*---(SHARED VARIABLES)---*/
int *action_cnt;
int *img_undecided_cnt;
int *img_registered_cnt;
int *img_inside_cnt;

FILE *file;


int checkForDigit(int argc, char *argv[]){
    for (int i = 1; i < argc; i++){
        for (unsigned int f = 0 ; f < strlen(argv[i]); f++){
            if (!isdigit(argv[i][f])) return 1;
        }
    }
    return 0;
}

int checkArgs(int argc, char *argv[]){
    if (argc != 6){
        fprintf(stderr, "Wrong number of arguments!\n");
        return 1;
    }

    if (checkForDigit(argc, argv)){
        fprintf(stderr, "Invalid arguments!\n");
        return 1;
    }

    if (atoi(argv[1]) < 1){
        fprintf(stderr, "Invalid arguments!\n%d (PI) !>= 1.\n", atoi(argv[1]));
        return 1;
    }

    for (int i = 2; i < argc; i++){
        if (atoi(argv[i]) < 0 || 2000 < atoi(argv[i])){
            fprintf(stderr, "Invalid arguments!\n0=< %d <=2000 is not true!\n", atoi(argv[i]));
            return 1;
        }
    }
    return 0;
}

void imigrant_process(int possition, schedule_t scheduler, FILE *file){

    sem_wait(sem_write);
    fprintf(file, "%d\t\t: IMM %d\t\t: starts\n",++(*action_cnt), possition);
    sem_post(sem_write);

    sem_wait(sem_enter);
    sem_wait(sem_write);
    fprintf(file, "%d\t\t: IMM %d\t\t: enters\t\t\t\t: %d\t\t: %d\t\t: %d\n",++(*action_cnt), possition, ++(*img_undecided_cnt), *img_registered_cnt, ++(*img_inside_cnt));

    sem_trywait(sem_checked);

    sem_post(sem_write);
    sem_post(sem_enter);

    sem_wait(sem_register);
    sem_wait(sem_write);
    fprintf(file, "%d\t\t: IMM %d\t\t: checks\t\t\t\t: %d\t\t: %d\t\t: %d\n",++(*action_cnt), possition, *img_undecided_cnt, ++(*img_registered_cnt), *img_inside_cnt);
    sem_post(sem_write);
    sem_post(sem_register);

    //if everyone is checked, let judge continue
    sem_wait(sem_write);
    if ((*img_undecided_cnt) == (*img_registered_cnt)){
        sem_post(sem_write);

        sem_post(sem_checked);
    }
    else {
        sem_post(sem_write);
    }

    //wait for judge to decide
    sem_wait(sem_write);
    if ((*img_undecided_cnt) != 0 && (*img_registered_cnt) != 0){
        sem_post(sem_write);

        sem_trywait(sem_jdg_done);
        sem_wait(sem_jdg_done);
        sem_post(sem_jdg_done);
    }
    else sem_post(sem_write);

    sem_wait(sem_write);
    fprintf(file, "%d\t\t: IMM %d\t\t: wants certificate\t\t: %d\t\t: %d\t\t: %d\n",++(*action_cnt), possition, *img_undecided_cnt, *img_registered_cnt, *img_inside_cnt);
    sem_post(sem_write);

    
    WAITFOR(scheduler.img_ret_time);

    sem_wait(sem_write);
    fprintf(file, "%d\t\t: IMM %d\t\t: got certificate\t\t: %d\t\t: %d\t\t: %d\n",++(*action_cnt), possition, *img_undecided_cnt, *img_registered_cnt, *img_inside_cnt);
    sem_post(sem_write);

    sem_wait(sem_enter);
    sem_wait(sem_write);
    fprintf(file, "%d\t\t: IMM %d\t\t: leaves\t\t\t\t: %d\t\t: %d\t\t: %d\n",++(*action_cnt), possition, *img_undecided_cnt, *img_registered_cnt, --(*img_inside_cnt));
    sem_post(sem_write);
    sem_post(sem_enter);

    exit(0);
}

void imigrantGenerator(schedule_t scheduler, FILE *file){
    //printf("Inside generator\n");
    for (int i = 0; i < scheduler.img_max; i++){
        
        //printf("Inside for loop %d\n", i);
        //usleep((rand() % (scheduler.img_gen_delay - 0 + 1) + 0)*1000 );
        
        WAITFOR(scheduler.img_gen_delay);

        pid_t img_PID = fork();

        if (!img_PID){
            imigrant_process(i+1, scheduler, file);
        }
    }

    int status = 0;
    pid_t wpid = wait(&status);
    while (wpid > 0){
        wpid = wait(&status);
    }

    exit(0);
}

void judge_process(schedule_t scheduler, FILE *file){
    int processed_img = 0;
    while(processed_img < scheduler.img_max){
        //usleep((rand() % (scheduler.jdg_ent_delay - 0 + 1) + 0)*1000 );
        WAITFOR(scheduler.jdg_ent_delay);

        //wants to enter
        sem_wait(sem_write);
        fprintf(file, "%d\t\t: JUDGE\t\t: wants to enter\n",++(*action_cnt));
        sem_post(sem_write);

        //enter, so noby else can enter now, untill he's gone
        sem_wait(sem_enter);

        sem_wait(sem_write);
        fprintf(file, "%d\t\t: JUDGE\t\t: enters\t\t\t\t: %d\t\t: %d\t\t: %d\n",++(*action_cnt), *img_undecided_cnt, *img_registered_cnt, *img_inside_cnt);
        sem_post(sem_write);

        sem_trywait(sem_jdg_done);

        //wait for everyone to check
        sem_wait(sem_write);
        if (*img_undecided_cnt != *img_registered_cnt){
            fprintf(file, "%d\t\t: JUDGE\t\t: waits for imm\t\t\t: %d\t\t: %d\t\t: %d\n",++(*action_cnt), *img_undecided_cnt, *img_registered_cnt, *img_inside_cnt);
            sem_post(sem_write);
            
            sem_trywait(sem_checked);
            sem_wait(sem_checked);
            sem_post(sem_checked);
        }
        else sem_post(sem_write);

        sem_wait(sem_write);
        fprintf(file, "%d\t\t: JUDGE\t\t: starts confirmation\t: %d\t\t: %d\t\t: %d\n",++(*action_cnt), *img_undecided_cnt, *img_registered_cnt, *img_inside_cnt);
        sem_post(sem_write);

        WAITFOR(scheduler.jdg_gen_time);

        sem_wait(sem_write);
        fprintf(file, "%d\t\t: JUDGE\t\t: ends confirmation\t\t: %d\t\t: %d\t\t: %d\n",++(*action_cnt), 0, 0, *img_inside_cnt);
        processed_img += (*img_registered_cnt);
        (*img_undecided_cnt) = (*img_registered_cnt) = 0;
        sem_post(sem_write);

        //let everyone go for certificate
        sem_post(sem_jdg_done);

        WAITFOR(scheduler.jdg_gen_time);

        sem_wait(sem_write);
        fprintf(file, "%d\t\t: JUDGE\t\t: leaves\t\t\t\t: %d\t\t: %d\t\t: %d\n",++(*action_cnt), *img_undecided_cnt, *img_registered_cnt, *img_inside_cnt);
        sem_post(sem_write);

        sem_post(sem_enter);
        
        if (processed_img >= scheduler.img_max ) break;
    }
    sem_wait(sem_write);
    fprintf(file, "%d\t\t: JUDGE\t\t: finishes\n",++(*action_cnt));
    sem_post(sem_write);

    exit(0);
}

void cleaner(){

    //close semaphores and unlink them
    sem_close(sem_enter);
    sem_close(sem_write);
    sem_close(sem_register);
    sem_close(sem_checked);
    sem_close(sem_jdg_done);

    sem_unlink(SEM_ENTER);
    sem_unlink(SEM_WRITE);
    sem_unlink(SEM_REGISTER);
    sem_unlink(SEM_CHECKED);
    sem_unlink(SEM_JDG_DONE);

    //remove shared variables
    munmap(action_cnt, sizeof(action_cnt));
    munmap(img_undecided_cnt, sizeof(img_undecided_cnt));
    munmap(img_registered_cnt, sizeof(img_registered_cnt));
    munmap(img_inside_cnt, sizeof(img_inside_cnt));
}

int init(){

    //initialize shared variable
    SHVAR(action_cnt);
    SHVAR(img_undecided_cnt);
    SHVAR(img_registered_cnt);
    SHVAR(img_inside_cnt);

    *action_cnt = 0;
    *img_undecided_cnt = 0;
    *img_registered_cnt = 0;
    *img_inside_cnt = 0;

    int error = 0;

    //open semaphores, if we can't free them
    if (SEMINIT(sem_enter, SEM_ENTER) == SEM_FAILED){
        fprintf(stderr, "Failed to open semafor %s!\n", SEM_ENTER);
        cleaner();
        error++;
    }
    if (SEMINIT(sem_write, SEM_WRITE) == SEM_FAILED){
        fprintf(stderr, "Failed to open semafor %s!\n", SEM_WRITE);
        cleaner();
        error++;
    }
    if (SEMINIT(sem_register, SEM_REGISTER) == SEM_FAILED){
        fprintf(stderr, "Failed to open semafor %s!\n", SEM_REGISTER);
        cleaner();
        error++;
    }
    if (SEMINIT(sem_checked, SEM_CHECKED) == SEM_FAILED){
        fprintf(stderr, "Failed to open semafor %s!\n", SEM_CHECKED);
        cleaner();
        error++;
    }
    if (SEMINIT(sem_jdg_done, SEM_JDG_DONE) == SEM_FAILED){
        fprintf(stderr, "Failed to open semafor %s!\n", SEM_JDG_DONE);
        cleaner();
        error++;
    }
    return error;
}

int main (int argc, char *argv[]){

    if (checkArgs(argc, argv)){
        exit(1);
    }

    //if semaphore initialization fails, exit
    if(init()){
        cleaner();
        exit(1);
    }

    file = fopen(OUTPUT, "w");    //open/create file in write mode
    setbuf(file, NULL);           //we don't want any buffer

    schedule_t scheduler ={atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5])};

    pid_t jdg_PID = fork();

    if (!jdg_PID){
        judge_process(scheduler, file);

    }
    else if (jdg_PID == -1){
        fprintf(stderr, "Failed to fork 'judge'.\n");
        fclose(file);
        exit(1);
    }
    else {
        pid_t generator_PID = fork();
        
        if (!generator_PID){
            imigrantGenerator(scheduler, file);
        }
        else if (generator_PID == -1){
            fprintf(stderr, "Failed to fork 'generator'.\n");
            kill(jdg_PID, SIGTERM);
            cleaner();
            fclose(file);
            exit(1);
        }
        else {
        }
    }

    //TODO - wait for judge to end so that main process can end as well
    //waitpid(jdg_PID, NULL, WUNTRACED);
    int status = 0;
    pid_t wpid = wait(&status);
    while (wpid > 0){
        wpid = wait(&status);
    }
    cleaner();
    fclose(file);
    exit(0);
}