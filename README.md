# Projet IOT Alarme - Capteurs

## Spécifications de l'api :
> Pour communiquer avec la base

GET /log
```json
  {
    "events": [
      {
        "opening": Boolean,
        "time": Long
      },
      {
        ...
      }
    ],
    "up": true
  }
```

GET /state
```json
  {
    "open": Boolean
  }
```

## Description

On utilisera 3 capteurs à effet Hall, chacun sur un ESP32 en fonctionnement indépendant, alimentés par des sources externes. Chacun sera situé sur un point d'entrée (porte, fenêtre) avec un aimant.

Les ESP32 envoient une requête à la base dans le cas de détéction d'un changement d'état.

Afin de préserver les evenements en cas de problème de communication, les ESP32 stockent interieurement un log d'événements de changement d'état. Une fois le message a bien été transmis à la base ou récuperé par la base il est supprimé du log.

Les ESP32 exposent des endpoints

-   un edpoint "status" pour collecter le log (fonctionnant comme ping)
    
-   un endpoint "state" pour fournir l'état actuel du capteur
    

Au démarrage l'ESP attend 20 secondes avant de se mettre en mode online. ( ça sert à eviter que en resetant le capteur on puise eviter la détection ).
