# Merge Sort paralelo

## Funcionamiento

* El programa obtiene sus datos de entrada del archivo input.csv
* Dentro del programa se encuentran números separados por ',' sin espacios en blanco entre los números o entre las comas

### Ejemplos:
| Formato      | Validez   |
|--------------|-----------|
| 1,13,54,21   | valido    |
| 1 ,13,54,21  | no valido |
| 1,13,54,21,  | valido    |
| 1, 13,54,21  | valido    |
| 1, 13 ,54,21 | no valido |

## Compilación

```bash
make

```

## Ejecución

```bash
./mergeSort
```

## Limpieza
```bash
make clean
```

