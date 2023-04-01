import subprocess
import os
import time
import json

numbers_of_movies = [10, 20, 25, 30, 40, 50, 60, 70, 80, 90, 100, 200, 1000, 10000]
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

def generate_output_for_tests():
    print("Generating output for tests...")

    results = []

    if not os.path.exists("data/output"):
        os.makedirs("data/output")
        
    for test in list_of_tests:
        with open (f"./data/input/input-{test[0]}-{test[1]}.txt", ) as file:
            start = time.perf_counter()
        
            proc = subprocess.run(
                ["./heuristica", f"./data/output/output-{test[0]}-{test[1]}.txt"], 
                input = file.read(),
                text=True,
                capture_output=True,
            )

            end = time.perf_counter()

            results.append({
                "n": test[0],
                "k": test[1],
                "time": end - start,
            })

    with open("data/results.json", "w") as f:
        json.dump(results, f, indent=2)


    print("Done! :)")
if __name__ == "__main__":
    generate_input_for_tests()
    generate_output_for_tests()

