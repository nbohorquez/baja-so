BAJA SO
=======

Kernel/scheduler developed for the Baja SAE USB prototype vehicle of 2008.

(Original spanish description)

```
Universidad Simón Bolívar
BAJA SAE USB
Dirección Técnica
División de Electrónica
                                                                           
                SISTEMA OPERATIVO DEL PROTOTIPO 2007-2008 BAJA SO

Alejandro Maita
Andres Arguello
Bruno Pierucci
Daniel Bello
Néstor Bohórquez
                                             Diciembre 2007 - Febrero 2008 
```

---

Caracas, 21 de febrero de 2008 15:37 GMT-4:30

*BajaSO v1.2*

La primera versión completamente funcional del Sistema Operativo esta lista.

Kernel:
-------

* Control de procesos:
  * Multitarea algoritmo de asignacion de procesador por Round Robin.
  * Manejo de colas: Listos, Bloqueados, EnEspera.
  * (TODO) Manejo de excepciones y codigo compartido (semaforos, mutex, etc).
  * (TODO) Comunicacion entre procesos (mensajes).
* Control de memoria:
  * Completamente estatico.
  * (TODO) Moverlo hacia un modulo aparte.
  * (TODO) Control dinamico para asignacion de memoria RAM.
* Control de hardware:
  * Interrupciones y control de los modulos TBM, TIM1, TIM2, ADC, SCI, CPU, PTA, PTB, PTC, PTD. 
  * (TODO) Moverlo hacia un modulo aparte.
  * (TODO) Generalizar mas el uso de los perifericos, es decir, no hacerlos tan dependientes de los 
    procesos que las usan como ahorita ocurre.

Procesos:
---------

* VerificarFrenos.
* MedirBateria.
* MedirGasolina.
* ActualizarTablero.
* ModoDeFuncionamiento.
* MedirVelocidad.
* MedirRPM.
* EnviarPaquete.
* ControlarTablero.
* OsciladorFrenos.

Todavia el compilador del CodeWarrior muestra 12 advertencias por posible perdida
de datos y una condicion que, según él, al evaluarse siempre resulta verdadera.
