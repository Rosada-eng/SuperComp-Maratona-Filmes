import subprocess
import os
import time
import json
import numpy as np

numbers_of_movies = [10, 20, 25, 30, 100, 1000, 10000, 100000, 1000000]
numbers_of_categories = [3, 4, 6, 8, 10]

list_of_tests = []
for n in numbers_of_movies:
    for m in numbers_of_categories:
        list_of_tests.append((n, m))


def generate_input_for_tests():
    print("Generating input for tests...")

    if not os.path.exists("data/input"):
        os.makedirs("data/input")
        
    for test in list_of_tests:
        subprocess.run(
            ["./gerador", str(test[0]), str(test[1]), "./data/input/input-{}-{}.txt".format(test[0], test[1])],
            capture_output=True,
        )

    print("Done! :)")

def generate_output_for(exe_name):
    print("Generating output for tests...")

    results = []

    dir_name = f"./data/output/{exe_name}"
    if not os.path.exists(dir_name):
        os.makedirs(dir_name)
        
    for test in list_of_tests:
        if (exe_name == "exaustiva_omp" and test[0] <= 30) or (exe_name != "exaustiva_omp"):
            with open (f"./data/input/input-{test[0]}-{test[1]}.txt", ) as file:
                start = time.perf_counter()
            
                proc = subprocess.run(
                    [f"./{exe_name}", f"{dir_name}/output-{test[0]}-{test[1]}.txt"], 
                    input = file.read(),
                    text=True,
                    capture_output=True,
                )

                end = time.perf_counter()

                #pega n_selecionados e tempo_tela do proc.stdout
                
                n_selecionados = int(proc.stdout.split("\n")[0])
                tempo_tela = int(proc.stdout.split("\n")[1])


                results.append({
                    "n": str(test[0]),
                    "k": str(test[1]),
                    "time": end - start,
                    "n_selecionados": n_selecionados,
                    "tempo_tela": tempo_tela
                })

        with open(f"{dir_name}/results.json", "w") as f:
            json.dump(results, f, indent=2)


    print("Done! :)")
if __name__ == "__main__":
    # generate_input_for_tests()
    # generate_output_for("heuristica")
    # generate_output_for("aleatorio")
    # generate_output_for("exaustiva_omp")
    generate_output_for("exaustiva_gpu")

