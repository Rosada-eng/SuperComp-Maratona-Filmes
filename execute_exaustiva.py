import os
import subprocess
import time
import json

n_movies = [10, 20, 25, 30]
k_categories = [3, 4, 6, 8, 10]

list_of_tests = []
for n in n_movies:
    for k in k_categories:
        list_of_tests.append((n, k))


def compare_results_for(exe_path):
    print("Generating output for exaustiva tests...")

    print("Consuming the same input files...")
    results = []

    if not os.path.exists("data/output"):
        os.makedirs("data/output")
        
    for test in list_of_tests:
        with open (f"./data/input/input-{test[0]}-{test[1]}.txt", ) as file:
            start = time.perf_counter()
        
            proc = subprocess.run(
                [exe_path, f"./data/output/output-{test[0]}-{test[1]}.txt"], 
                input = file.read(),
                text=True,
                capture_output=True,
            )

            end = time.perf_counter()
            n_processos = int(proc.stdout.split('\n')[0])
            tempo_tela = int(proc.stdout.split('\n')[1])

            results.append({
                "n": str(test[0]),
                "k": str(test[1]),
                "time": end - start,
                "n_processos": n_processos,
                "tempo_tela": tempo_tela
            })

    with open("data/results2.json", "w") as f:
        json.dump(results, f, indent=2)


    print("Done! :)")