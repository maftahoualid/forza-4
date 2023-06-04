#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>


struct stat {
dev_t st_dev; // IDs of device on which file resides.
ino_t st_ino; // I-node number of file.
    // (st_dev + st_ino) identificano un file in un qualunque filesystem 
mode_t st_mode; // File type and permissions.
    // st_mode && S_IFMT -> FILE TYPE
    // costante  macro       descrizione
    // S_IFREG  S ISREG()  Regular file
    // S_IFDIR  S_ISDIR()  Directory
    // S_IFCHR  S_ISCHR()  Character device
    // S_IFBLK  S_ISBLK()  Block device
    // S_IFIFO  S_ISFIFO() FIFO or pipe
    // S_IFSOCK S_ISSOCK() Socket
    // S_IFLNK  S_ISLNK()  Symbolic link
    // FILE TYPE && costante -> valore
    // macro(st_mode) -> valore

    // costante descrizione
    // S_ISUID Set-user-ID
    // S_ISGID Set-group-ID
    // S_ISVTX Sticky
    // S_IRUSR User-read
    // S_IWUSR User-write
    // S_IXUSR User-execute
    // S_IRGRP Group-read
    // S_IWGRP Group-write
    // S_IXGRP Group-execute
    // S_IROTH Other-read
    // S_IWOTH Other-write
    // S_IXOTH Other-execute
    // st_mode & costante -> valore

    // st_mode |= costante -> aggiunge permesso
    // st_mode &= ~costante -> toglie permesso

nlink_t st_nlink; // Number of (hard) links to file.
uid_t st_uid; // User ID of file owner.
gid_t st_gid; // Group ID of file owner.
dev_t st_rdev; // IDs for device special files.
off_t st_size; // Total file size (bytes).
//blksize_t st_blksize; // Optimal block size for I/O (bytes).
blkcnt_t st_blocks; // Number of (512B) blocks allocated.
time_t st_atime; // Time of last file access.
time_t st_mtime; // Time of last file modification.
time_t st_ctime; // Time of last status change.
};


struct dirent {
ino_t d_ino; // File i-node number.
unsigned char d_type; // Type of file.
    // costante descrizone
    // DT_BLK  block device
    // DT_CHR  character device
    // DT_DIR  directory
    // DT_FIFO named pipe (FIFO)
    // DT_LNK  symbolic link
    // DT_REG  regular file
    // DT_SOCK UNIX socket
    // d_type && costante -> valore
char d_name[256]; // Null-terminated name of file.
//...
};


struct semid_ds {
struct ipc_perm sem_perm; /* Ownership and permissions */
time_t sem_otime; /* Time of last semop() */
time_t sem_ctime; /* Time of last change */
unsigned long sem_nsems; /* Number of semaphores in set */
};


struct shmid_ds {
    // posso modificare solo questo campo con shmctl()
struct ipc_perm shm_perm; /* Ownership and permissions */ 
    // questo campo non posso modificarlo con shmctl()
size_t shm_segsz; /* Size of segment in bytes */

time_t shm_atime; /* Time of last shmat() */
time_t shm_dtime; /* Time of last shmdt() */
time_t shm_ctime; /* Time of last change */
pid_t shm_cpid; /* PID of creator */
pid_t shm_lpid; /* PID of last shmat() / shmdt() */
shmatt_t shm_nattch; // Number of currently attached
};


struct msqid_ds {
struct ipc_perm msg_perm; /* Ownership and permissions */ // può essere modificato da msgctl
time_t msg_stime; /* Time of last msgsnd() */
time_t msg_rtime; /* Time of last msgrcv() */
time_t msg_ctime; /* Time of last change */
unsigned long __msg_cbytes; /* Number of bytes in queue */ // può essere modificato da msgctl
msgqnum_t msg_qnum; /* Number of messages in queue */
msglen_t msg_qbytes; /* Maximum bytes in queue */
pid_t msg_lspid; /* PID of last msgsnd() */
pid_t msg_lrpid; /* PID of last msgrcv() */
};


// struttura presente in semid_ds, shmid_ds, msqid_ds
struct ipc_perm {
key_t __key; /* Key, as supplied to ’get’ call */
uid_t uid; /* Owner’s user ID */
gid_t gid; /* Owner’s group ID */
uid_t cuid; /* Creator’s user ID */
gid_t cgid; /* Creator’s group ID */
unsigned short mode; /* Permissions */
unsigned short __seq; /* Sequence number */
};


// semun
#ifndef SEMUN_H
#define SEMUN_H
#include <sys/sem.h>
// definition of the union semun
union semun {
int val;
struct semid_ds * buf;
    // fatta per contenere una copia del semid_ds del processo
unsigned short * array;
};
#endif


// struttura sembuf usata in semop()
struct sembuf {
unsigned short sem_num; /* Semaphore number */
short sem_op; /* Operation to be performed */
short sem_flg; /* Operation flags */
};


// struttura di una message queue
struct mymsg {
long mtype; /* Message type */ // non negative long (per forza)
char mtext[]; /* Message body */ // non è necessario che sia un array di char, può essere qualsiasi cosa
};