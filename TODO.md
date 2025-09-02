# Proyecto 3: Semáforos

# PREGUNTAR DUDASSSSSSS
- esta bien haberle cambiado el perfil a create_process() para que retorne la task??
- cual semcreate deberiamos llamar sys_semcreate() o semcreate(), y asi con todas etc.
- esta bien que cuando un proceso cree un semaforo, le agreguemos ese semaforo a su lista de semaforos? o esto se deberia hacer cuando el proceso haga un semwait?
- esta bien esto? semcreate(EMPTY, 0);
- el value de un semaforo, es la cota maxima? 
- comentarios (delfi) en semsignal()


### 1

- corregir las direcciones virtuales a las que mapean los procesos.

- usar semaforos (fijarse bien lo que retornan los metodos de los semaforos, algunos retornan null, y no se si deberian retornar -1 ?????)

- corregir todos los "creo que es un bug"

- fijarse todos los "TODO"