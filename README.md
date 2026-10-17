# Compiladores (CS3402) · Semana 10: Generación de código x86-64

Compilador de un lenguaje pequeño escrito en C++: scanner → parser → AST → `GencodeVisitor`, que genera ensamblador x86-64 (`.s`) y lo ensambla con `gcc`.

## Estructura
| Archivo | Qué contiene |
|---|---|
| `token.h/.cpp`, `scanner.h/.cpp` | Análisis léxico (tokens) |
| `parser.h/.cpp` | Análisis sintáctico (descenso recursivo) |
| `ast.h/.cpp` | Nodos del árbol de sintaxis abstracta |
| `visitor.h/.cpp` | `GencodeVisitor`: AST → ensamblador x86-64 |
| `main.cpp` | Lee el programa, genera el `.s`, lo ensambla y ejecuta |
| `inputs/`, `outputs/` | Programas de prueba y sus `.s` generados |
| `inputs_eval1/` | Programas de prueba de la Evaluación Continua 1 |

## Cómo correrlo (Linux)
```bash
python3 run_all_inputs.py      # compila y corre inputs/input1..5
# o uno solo:
g++ main.cpp scanner.cpp token.cpp parser.cpp ast.cpp visitor.cpp -o compilador
./compilador inputs/input1.txt
```
