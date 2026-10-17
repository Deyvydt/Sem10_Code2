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
| `inputs_eval1/`, `outputs_eval1/` | Programas de la Evaluación Continua 1 y sus `.s` |

## Qué soporta el lenguaje
- Asignación `x = ...` y `print(...)`
- Operadores `+ - * / **` y comparación `<=` (devuelve 1 o 0)
- `if cond then ... [else ...] endif`
- `while cond do ... enddo`
- `do ... while cond` (el cuerpo se ejecuta al menos una vez)
- `break` (sale del ciclo más interno)
- `ifexp(cond, a, b)`: expresión que vale `a` si `cond` no es 0, si no `b`

## Cómo correrlo (Linux)
```bash
python3 run_all_inputs.py                # corre inputs/       -> outputs/
python3 run_all_inputs.py inputs_eval1   # corre inputs_eval1/ -> outputs_eval1/
# o uno solo:
g++ main.cpp scanner.cpp token.cpp parser.cpp ast.cpp visitor.cpp -o compilador
./compilador inputs/input1.txt
```
