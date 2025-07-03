


// Nodo individual de la cola
struct wait_queue_entry {
    struct task *task;
    struct wait_queue_entry *next;
};

// Cola de espera completa
struct wait_queue {
    struct wait_queue_entry *head;
    struct wait_queue_entry *tail;
    spinlock_t lock;  // Protección contra condiciones de carrera
};

// Funciones de inicialización
void init_waitqueue(struct wait_queue *q);
void init_waitqueue_entry(struct wait_queue_entry *entry, struct task *task);

// ========================
// OPERACIONES DE COLA
// ========================

/**
 * add_wait_queue - Agrega una entrada al final de la cola
 * @q: Cola de espera
 * @entry: Entrada a agregar
 */
void add_wait_queue(struct wait_queue *q, struct wait_queue_entry *entry);

/**
 * add_wait_queue_exclusive - Agrega una entrada exclusiva
 * @q: Cola de espera
 * @entry: Entrada a agregar
 */
void add_wait_queue_exclusive(struct wait_queue *q, struct wait_queue_entry *entry);

/**
 * remove_wait_queue - Remueve una entrada específica de la cola
 * @q: Cola de espera
 * @entry: Entrada a remover
 */
void remove_wait_queue(struct wait_queue *q, struct wait_queue_entry *entry);

/**
 * waitqueue_active - Verifica si la cola tiene entradas
 * @q: Cola de espera
 * Return: true si hay entradas, false en caso contrario
 */
static inline bool waitqueue_active(struct wait_queue *q);