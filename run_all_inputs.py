import os
import subprocess
import shutil

# Archivos c++
programa = ["main.cpp", "scanner.cpp", "token.cpp", "parser.cpp", "ast.cpp", "visitor.cpp"]

# Compilar
compile = ["g++"] + programa
print("Compilando:", " ".join(compile))
result = subprocess.run(compile, capture_output=True, text=True)

if result.returncode != 0:
    print("Error en compilación:\n", result.stderr)
    exit(1)

print("Compilación exitosa")

# Carpeta de salida limpia: al final solo quedan los .s y el reporte
# Uso: python3 run_all_inputs.py [carpeta]   (por defecto: inputs -> outputs)
#      python3 run_all_inputs.py inputs_eval1   (-> outputs_eval1)
import sys, re
input_dir = sys.argv[1].rstrip("/") if len(sys.argv) > 1 else "inputs"
output_dir = "outputs" if input_dir == "inputs" else input_dir.replace("inputs", "outputs", 1)
if output_dir == input_dir:
    output_dir = input_dir + "_outputs"
shutil.rmtree(output_dir, ignore_errors=True)
os.makedirs(output_dir)
reporte = []

numeros = sorted(int(m.group(1)) for f in os.listdir(input_dir)
                 if (m := re.fullmatch(r"input(\d+)\.txt", f)))
for i in numeros:
    filename = f"input{i}.txt"
    filepath = os.path.join(input_dir, filename)

    if not os.path.isfile(filepath):
        print(filename, "no encontrado en", input_dir)
        continue

    print(f"Ejecutando {filename}")
    result = subprocess.run(["./a.out", filepath], capture_output=True, text=True)

    # Archivos que genera el compilador en inputs/
    tokens_file = os.path.join(input_dir, f"input{i}_tokens.txt")
    asm_file = os.path.join(input_dir, f"input{i}.s")
    salida_file = os.path.join(input_dir, f"input{i}_salida.txt")

    # Leer el ejercicio y el resultado de su .s
    with open(filepath, encoding="utf-8") as f:
        ejercicio = f.read().strip()
    if result.returncode == 0 and os.path.isfile(salida_file):
        with open(salida_file, encoding="utf-8") as f:
            resultado = f.read().strip()
    else:
        resultado = "ERROR: " + result.stderr.strip()

    reporte.append("=" * 40 + "\n"
                   + f"{filename}\n"
                   + "=" * 40 + "\n"
                   + "Ejercicio:\n"
                   + ejercicio + "\n\n"
                   + f"Resultado de input{i}.s:\n"
                   + resultado + "\n")

    # Conservar solo el .s; borrar tokens y salida
    if os.path.isfile(asm_file):
        shutil.move(asm_file, os.path.join(output_dir, f"input{i}.s"))
    for residuo in (tokens_file, salida_file):
        if os.path.isfile(residuo):
            os.remove(residuo)

# Guardar el reporte
reporte_file = os.path.join(output_dir, "reporte.txt")
with open(reporte_file, "w", encoding="utf-8") as f:
    f.write("\n".join(reporte))
print("Reporte generado en", reporte_file)
