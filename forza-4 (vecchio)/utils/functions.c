#include <stdio.h>
#include <sys/resource.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "struct.c"

// FUNDAMENTAL CONCEPTS //

/**
 * @brief numero di errore restituito da una system call fallita
 *
 */
errno;

/**
 * @brief ottiene la piorità (nice number) di un processo, gruppo o user
 *
 * @param which { "PRIO_PROCESS", "PRIO_PGRP", or "PRIO_USER" }
 * @param who è il process_id, group_id o user_id
 * @return (int) nice number
 *
 * @example nice = getpriority(PRIO_PROCESS, pid);
 * @include <sys/resource.h>
 */
int getpriority(int which, id_t who);

/**
 * @brief imposta la piorità (nice number) di un processo, gruppo o user
 *
 * @param which  { "PRIO_PROCESS", "PRIO_PGRP", o "PRIO_USER" }
 * @param who è il process_id, group_id o user_id
 * @param value il nice number da impostare
 * @return (int) 0 o -1
 *
 * @example ret = setpriority(PRIO_PROCESS, pid, -20);
 * @include <sys/resource.h>
 */
int setpriority(int which, id_t who, int value);

/**
 * @brief stampa un messaggio di sistema sull'errore
 *
 * @param s testo da stampare prima del messaggio
 * @example perror("[function3 error]")
 * @include <stdio.h>
 */
void perror(const char *s);

/**
 * @brief ritorna la stringa che descrive l'errore associato a errnum
 *
 * @param errnum numero di errno
 * @return char* stringa di errore
 * @example printf("%s", strerror(errno));
 * @include <string.h>
 */
char *strerror(int errnum);

// FILESYSTEM //

/**
 * @brief apre (o crea) un file e ritorna il suo file descriptor
 *
 * @param pathname nome del file da aprire/creare
 * @param flags { O_RDONLY (sola lettura),
				  O_WRONLY (sola scrittura),
				  O_RDWR (leggi e scrivi). }
				{ O_CREAT (crea il file se non esiste),
				  O_EXCL (dai errore se il file non esiste). }
				{ O_APPEND (scrivi senza cancellare il contenuto),
				  O_TRUNCATE (cancella il contenuto, poi scrivi). }
 * @param mode : {S_<R (read),W (write),X (execute),RWX (all)>_<USR (user),GRP (group),OTH (others)>}
				 { 0 (user) (group) (others) : (read:+4, write:+2, exec:+1) }
 * @example file_descriptor = open("./file.txt", O_CREAT | 0777 ); /*
 * @include <fcntl.h>
 * @return int file descriptor del file aperto (-1 se fallisce -> vedi errno)
 */
int open(const char *pathname, int flags, mode_t mode); 

/**
 * @brief legge da un file descriptor
 *
 * @param fd file descriptor (STDIN_FILENO: legge da standard input (tastiera), STDOUT_FILENO: legge da standard output (terminale))
 * @param buf puntatore a dove salvare la lettura
 * @param count quanti byte leggere
 * @return ssize_t quanti byte letti (-1 se fallisce)
 * @example ret = read(STDIN_FILENO, &buf, 50);
 * @include <unistd.h>
 */
ssize_t read(int fd, void *buf, size_t count);	
		 
/**
 * @brief scrive su un file descriptor
 *
 * @param fd file descriptor (STDOUT_FILENO: legge da standard output (terminale))
 * @param buf puntatore a da dove prendere il contenuto da scrivere
 * @param count quanti byte scrivere
 * @return ssize_t quanti byte scritti (-1 se fallisce)
 * @example ret = write(STDOUT_FILENO, &buf, 50); /*
 * @include <unistd.h>
 */
ssize_t write(int fd, void *buf, size_t count);	
		
/**
 * @brief sposta il puntatore di lettura/scrittura in un file di "offset" a partire da "whence"
 *
 * @param fd file descriptor
 * @param offset di quanto spostarti dal puntatore dato da whence
 * @param whence dove metterti inizialmente: {SEEK_SET:inizio, SEEK_CUR:posizione attuale, SEEK_END:fine}
 * @return off_t offset (-1 se fallisce)
 */
off_t lseek(int fd, off_t offset, int whence);			
		
/**
 * @brief chiude un file descriptor
 *
 * @param fd file descriptor
 * @return int 0 o -1
 */
int close(int fd);

////////////////////////////////////////
////////////////////////////////////////
////////////////////////////////////////

/**
 * @brief rimuove un puntatore a file, se è l'ultimo, rimuove il file
 * 
 * @param pathname percorso file
 * @return int 
 */
int unlink(const char *pathname);



/*
pathname: nome file
statbuf: struttura dove salvare le info
ritorna una struttura di info relative a un file
*/
int stat(const char *pathname, struct stat *statbuf);

/*
pathname: nome file
statbuf: struttura dove salvare le info
ritorna una struttura di info relative a un link
*/
int lstat(const char *pathname, struct stat *statbuf);

/*
pathname: nome file
statbuf: struttura dove salvare le info
ritorna una struttura di info relative a un file descriptor
*/
int fstat(int fd, struct stat *statbuf);

/*
pathname: nome file
mode: {F_OK (esiste?), R_OK (si legge?), W_OK (si scrive?), X_OK(si esegue?)}
*/
int access(const char *pathname, int mode);

/*
pathname: nome file
mode: (<struct-stat-name>.st_mode | <permessi da aggiungere>) & ~<permessi da togliere>
*/
int chmod(const char *pathname, mode_t mode);

/*
fd: nome file descriptor
mode: (<struct-stat-name>.st_mode | <permessi da aggiungere>) & ~<permessi da togliere>
*/
int fchmod(int fd, mode_t mode);

/*
pathname: nome cartella
mode: permessi
*/
int mkdir(const char *pathname, mode_t mode);

/*
pathname: nome cartella
*/
int rmdir(const char *pathname);

/*
pathname: nome directory
crea uno stream directory per permettere la lettura della cartella
*/
DIR *opendir(const char *pathname);

/*
dirp: directory pointer
chiude lo stream directory
*/
int closedir(DIR *dirp);

/*
dirp : directory pointer
per ogni call legge il prossimo file/firectory dello stream
*/
struct dirent *readdir(DIR *dirp);

// Processes

/*
ritorna il process id
*/
pid_t getpid(void);

/*
ritorna il user id
*/
uid_t getuid(void);

/*
ritorna il effective user id
*/
uid_t geteuid(void);

/*
ritorna il group id
*/
gid_t getgid(void);

/*
ritorna il effective group id
*/
gid_t getegid(void);

/*
ritorna il valore di una variabile di ambiente
*/
char *getenv(const char *name);

/*
setta il valore di una variabile di ambiente
*/
int setenv(const char *name, const char *value, int overwrite);

/*
resetta il valore di una variabile di ambiente
*/
int unsetenv(const char *name);


/*
ritorna la directory corrente
*/
char *getcwd(char *cwdbuf, size_t size);

/*
cambia current directory
*/
int chdir(const char *pathname);

/*
cambia current directory da file descriptor
*/
int fchdir(int fd);

/*
duplica file descriptor associato a un file
*/
int dup(int oldfd);

/*
termina un processo con codice status
*/
void _exit(int status);

/*
call exit handlers
flush stdio
call _exit()
*/
void exit(int status);

/*
esempio di exit handler
*/
int atexit(void (*func)(void));

/*
crea un nuovo processo clone e ritorna il suo id al chiamante
il processo figlio viene eseguito dal return della fork
nel figlio la fork restituisce 0
*/
pid_t fork(void);

/*
ritorna il pid del processo chiamante
*/
pid_t getppid(void);

/*
sospende il processo corrente fino alla terminazione di un suo figlio
ritorna -1 se non ci sono processi figli
*/
pid_t wait(int *status);

/*
status:
{ WIFEXITED(status) : ritorna 1 se figlio termina normalmente
	WEXITSTATUS(status) : ritorna lo status di uscita del figlio
	WIFSIGNALED(status) : ritorna 1 se figlio termina per un segnale
	WTERMSIG(status) : ritorna il valore del segnale che ha terminato il figlio
	WIFSTOPPED(status) : ritorna 1 se figlio viene stoppato da un segnale
	WSTOPSIG(status) : ritorna il numero del segnale che ha stoppato il figlio
	WIFCONTINUED(status) : ritorna 1 se il figlio viene riesumato da SIGCONT }

options:
{ WUNTRACED : ritorna quando figlio termina o stoppa per via di un segnale
	WCONTINUED : ritorna quando figlio viene resumato per via di SIGCONT
	WNOHANG : se il figlio pid non ha cambiato stato ritorna subito 0
	0 : attende solo i figli terminati}
*/
pid_t waitpid(pid_t pid, int *status, int options);

/*
stampa il nome del segnale, dato il suo numero
*/
char *strsignal(int sig);

/*
passo lista parametri
passo percorso relativo
*/
int execl(const char *path, const char *arg/*, ... , (char*) 0*/); 

/*
passo lista parametri
passo percorso relativo al PATH
*/
int execlp(const char *path, const char *arg/*, ... , (char*) 0*/);

/*
passo lista parametri
passo environment
*/
int execle(const char *path, const char *arg, /*... , (char*) 0,*/ const char *envp[]);

/*
passo vettore parametri
passo percorso relativo
*/
int execv(const char *path, const char *argv[]);

/*
passo vettore parametri
passo percorso relativo al PATH
*/
int execvp(const char *path, const char *argv[]);

/*
passo vettore parametri
passo environment
*/
int execve(const char *path, const char *argv[], const char *envp[]);


/**
 * @brief ritorna l'id del set di semafori creato/esistente
 * 
 * @param key nome chiave univoca (come "filename") (IPC_PRIVATE : delego al kernel la creazione di una key unica)
 * @param nsems dimensione vettore semafori
 * @param semflg {permessi come nei file,
				  IPC_CREAT : ritorna id se esiste, crea se non esiste
				  IPC_EXCL : fallisce se esiste, crea se non esiste}
 * @param arg 
 * @return int 
 */
int semget(key_t key, int nsems, int semflg);


/*
ritorna una key a partire dal path di un file e un id (ultimi 8 bit non nulli)
*/
key_t ftok(const char *path, int id);

/*
ottiene/cambia una struttura dati del semaforo dal kernel
cmd:
	IPC_RMID : rimuove tutto il set di semafori (processi vengono riesumati con errore EIDRM)
	IPC_STAT : salva in arg.buf la struttura semid_ds associata al set di semafori
	IPC_SET : salva nella struttura semun del semaforo la struttura passata in arg
	SETVAL : prende arg.val e lo mette nel valore del semaforo semnum del set dato da semid
	GETVAL : ritorna il valore del semaforo semnum (potrebbe essere cambiato nel mentre)
	SETALL : valorizza i semafori del set con i valori di arg.array[]
	GETALL : prende tutti i valori dei semafori del set e li salva in arg.array[]
	GETPID : ritorna il pid dell'ultimo processo che ha usato il semaforo semnum
	GETNCNT : ritorna il numero di processi che aspettano che il semaforo diventi positivo
	GETZCNT : ritorna il numero di processi che aspettano che il semaforo diventi zero (chiede all'orale)
*/
int semctl(int semid, int semnum, int cmd, union semun arg);

/*
se elimino il set di semafori : la semop fallisce con codice EINTR
se un'altro processo elimina il set : la semop fallisce con codice EIDRM

	sembuf sops[] : {{<semnum><op><flags>}{...},...}
		semnum: numero del semaforo nel set (parte da 0)
		op : op>0 : incrementa il semaforo semnum di <op>
				op=0 : se ci sono processi che aspettano : blocca il processo fino a che il semaforo non si azzera
					se non ci sono processi che aspettano : (sem a zero) si sblocca e va avanti
				op<0 : se il risultato è positivo/nullo : decrementa il semaforo di <op>
					se il risultato è negativo : blocca il processo finchè non sarà possibile eseguire la sottrazione con risultato non negativo
		flags : { IPC_NOWAIT : se la semop() dovesse bloccarmi, non mi blocco ma ritorna errore EAGAIN }
	nsops : numero di operazioni da eseguire sul set di semafori (dimensione di sops[])
*/
int semop(int semid, struct sembuf *sops, unsigned int nsops);

/*
ritorna SIG_ERR se fallisce

signum : numero segnale o costante
handler : {nome funzione handler da eseguire
			SIG_DFL: handler di default
			SIG_IGN: ignora il segnale }
*/
typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);

/*
mette in pausa un processo finchè non riceve un qualunque segnale
*/
int pause();

/*
mette in pausa un processo per un tot di secondi
seconds: secondi da attendere
ritorna il numero di secondi rimanenti (0 se ha finito)
*/
unsigned int sleep(unsigned int seconds);

/*
pid :
	pid>0 : pid del processo a cui mandare il segnale
	pid=0 : manda il segnale a tutti i processi dello stesso gruppo (incluso sè stesso)
	pid=-1 : manda il segnale a tutti i processi per cui ha i permessi di inviare segnali (eccetto init e sè stesso)
	pid<0 : manda il segnale a tutti i processi del gruppo dato dal valore assoluto di pid
	sig : numero o costante del segnale da inviare
*/
int kill(pid_t pid, int sig);

/*
	manda un segnale SIGALRM a sè stesso dopo seconds secondi
	settare un nuovo alarm sovrascrive quello vecchio
	posso fare un handler del segnale SIGALRM
*/
unsigned int alarm(unsigned int seconds);

/*
crea un set che non contiene segnali
*/
typedef unsigned long sigset_t;
int sigemptyset(sigset_t *set);

/*
crea un set che contiene tutti i segnali
*/
typedef unsigned long sigset_t;
int sigfillset(sigset_t *set);

/*
aggiunge un segnale al set
*/
int sigaddset(sigset_t *set, int sig);

/*
toglie un segnale dal set
*/
int sigdelset(sigset_t *set, int sig);

/*
ritorna 1 se sig è nel set
*/
int sigismember(const sigset_t *set, int sig);

/*
aggiunge o toglie segnali al signal mask : un set di segnali bloccati dal kernel per il processo
how : {SIG_BLOCK : passo un set di segnali che verranno aggiunti a quelli bloccati,
		SIG_UNBLOCK : passo un set di segnali che verranno tolti da quelli bloccati,
		SIG_SETMASK : passo un set e i segnali bloccati saranno uguali a quelli del nuovo set,
		(il set contenente i segnali bloccati precedente verrà messo in oldset}
set : il nuovo set di segnali bloccati (se passo NULL, metterà il vecchio set in oldset)
olset: il vecchio set di segnali bloccati
*/
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);

/*
crea una memoria condivisa, da una key
ritorna l'id di una memoria condivisa (creata o pre-esistente)
size : dimensione in byte della memoria condivisa (se voglio l'id di una memoria condivisa, allora size deve essere minore/uguale alla dimensione della memoria)
shmflag : { permessi come nei file,
			IPC_CREAT : crea se non esiste
			IPC_EXCL : se esiste, ritorna errore }
*/
int shmget(key_t key, size_t size, int shmflg);

/*

mappa la memoria condivisa sullo spazio di indirizzamento del processo
(posso fare più di una attach nello stesso processo ognuna con suoi Flag)
i figli di un processo ereditano l'attach della memoria condivisa
shmaddr : (default : NULL) indirizzo di dove salvare il map della memoria condivisa
shmflag : SHM_RND : approssimo il valore di shmaddr
			SHM_RDONLY : mappo la memoria in sola lettura
*/
void *shmat(int shmid, const void *shmaddr, int shmflg);

/*
elimino il link processo-memoria condivisa
(la memoria potrebbe essere attaccata ad altri processi o altri link nello stesso processo)
se faccio una exec, faccio automaticamente il detach di tutti processi da quella memoria
*/
int shmdt(const void *shmaddr);

/*
cmd :
	IPC_RMID : rimuove la memoria condivisa non appena sono staccati tutti i link ad essa
	IPC_STAT : prende la shmid_ds della memoria condivisa e le salva in buf
	IPC_SET : prende buf e lo salva nella shmid_ds della memoria condivisa
buf : puntatore a una struttura di tipo shmid_ds
*/
int shmctl(int shmid, int cmd, struct shmid_ds *buf);

/*
crea una coda di messaggi
msgflg :
	permessi : bitmask di permessi tipo open()
	IPC_CREAT : se non esiste, la crea
	IPC_EXCL : se esiste, torna errore
*/
int msgget(key_t key, int msgflg);

/*
msgp : puntatore al messaggio
msgsz : dimensione in byte della struttura mymsg - la dimensione del mtype (sizeof(long))
msgflg :
	0 : se la coda è piena, blocca finchè non c'è abbastanza spazio
	IPC_NOWAIT : se la coda è piena, torna con errore EAGAIN (try again, later)
*/
int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);

/*
	legge e rimuove messaggi dalla coda
	msgp : struttura dove salvare il messaggio
	msgsz : dimensione del messaggio da leggere (struct mymsg tranne mtype)
	msgtype : long per identificare tipo messaggio
	msgflg :
		IPC_NOWAIT: se non ci sono messaggi da leggere del tipo indicato, ritorna errore ENOMSG
		MSG_NOERROR: legge il messaggio rimuovendolo dalla coda, tronca il suo contenuto a msgsz e lo passa a msgp
*/
ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtype, int msgflg);

/*
	cmd:
		IPC_RMID : rimuove la coda, con annessi i messaggi, i processi bloccati sono sbloccati con errore EIDRM
		IPC_STAT : passa la struttura msqid_ds a buf
		IPC_SET : passa buf alla struttura msqid_ds
*/
int msgctl(int msqid, int cmd, struct msqid_ds *buf);

/*
SEGNALI PER TERMINARE UN PROCESSO
SIGTERM : termina correttamente un processo
SIGINT  : (CTRL-C) termina per via di un interrupt
SIGQUIT : termina richiedendo un core-dump per debugging
SIGKILL : termina sempre (non può essere bloccato, ignorato o gestito)

SEGNALI PER STOPPARE/RIESUMARE UN PROCESSO
SIGSTOP : stoppa sempre un processo (non può essere bloccato, ignorato o gestito)
SIGCONT : riesuma un processo stoppato

SIGPIPE : viene inviato quando un processo prova a leggere/scrivere una PIPE o una FIFO (se non se ne occupa un'altro processo)
SIGALRM : un segnale inviato a un processo che notifica il processo mittente dopo un tot di tempo
SIGUSR1 e SIGUSR2 : segnali definibili dall'utente
*/