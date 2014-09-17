Práctica Modpila
=================================

make clean; make; ./install.sh

cat /dev/pila # Imprime y retira cima (y todos los elementos con el mismo valor)
echo "9;8;8;3;2" > /dev/pila # Inserta los elementos en la pila (NO PONER DELIMITADOR ';' AL FINAL DE LA CADENA)

cd tester_app; make clean; make
./tester_app -k {numero}# consume los "numero" primeros elementos de la pila (no elimina valores coincidentes)
./tester_app -m {numero} # cambia el tamaño de la pila a "numero" elementos (Si hay más de "numero" elementos, no se eliminan)


Índice
------

1) [Funcionalidad] (#funcionalidad)  
2) [Instalación] (#instalacin)  
3) [Uso] (#uso)  

***

Funcionalidad
--------------

Driver de pila que define las siguientes operaciones

 1) Inserción de elementos en pila
 2) Lectura de cima de pila
 3) Consumición de k elementos
 4) Cambio de tamaño de la pila
 
La estructura ```file_operations``` que define las operaciones definidas es la siguiente:



Instalación
------------

1) Compilar el driver

``` /modpila $> make ```

2) Ejecutar el script de instalación (será necesario ser super usuario)

``` /modpila #> ./install.sh ```

Uso
----

Junto con el código del driver se entrega una aplicación de testeo.  
Se encuentra en la carpeta *tester_app*. Una vez en la carpeta de la aplicación compilamos para obtener el ejecutable:

``` /modpila/tester_app $> make ```

### Sobre el programa de pruebas

El programa de testeo está implementado en C.
Permite hacer uso de todas las funciones implementeadas en el driver y ver los resultados de forma clara y directa.

El programa dispone de los siguientes flags

| Flag    | Llamada                | Funcionalidad          |
|:-------:|:-----------------------|:-----------------------|
| r       | ./tester_app -k {num}  | Consume {numero} primeros elementos de la pila. |
| w       | ./tester_app -m {num}  | Cambia el tamaño de la pila, sin eliminar elementos. |


### Ejemplo de uso del programa tester_app

A continucación se muestra un ejemplo sencillo de uso del programa

``` 
 $> echo "4;3;5;4" > /dev/pila
  
 $> cat /dev/pila
4;2

/modpila/tester_app $> ./tester_app  -k 2
5;3

/modpila/tester_app $> ./tester_app -m 7
Nuevo tamanyo pila =>7 

```

***

