## Proyecto 3: Semáforos

### 1

Diseñar e implementar llamadas al sistema sobre semáforos n-arios.

int semcreate(int id, int init_value)
int semget(int id)
int sem_wait(int id)
int sem_signal(int id)
int sem_close(int id)

El sistema deberá manejar hasta NSEM semáforos y cada proceso hasta NSEM_PROC semáforos (NSEM_PROC < NSEM).
Implementar estas funciones en sem.c.

### 2

Implementar dos procesos de usuario que usen semáforos para acceder en forma sincronizada a un bloque de memoria compartida. Hacer el kernel asigne y mapee una página compartida entre ellos (harcoded en kmain.c).
