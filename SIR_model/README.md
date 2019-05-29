## Configuración del modelo

Editar archivos ```props/model.props```. Las definiciones por omisión del modelo son:

```
tiempoSimulacion=5000
totalIndividuos = 200
probInfeccion = 0.85
radioInfeccion = 10
probInmunidad = 0.10
tiempoRecuperacion = 200
```

## Ejemplo de ejecución
Universo particionado en 4 procesos.

```
./run.sh > logs/out01.txt 
cd logs
./graph.plt
```


