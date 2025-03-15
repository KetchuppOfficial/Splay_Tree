import argparse
import subprocess
import sys
from pathlib import Path
from termcolor import colored


def parse_options():
    parser = argparse.ArgumentParser(description =
        "This script is a utility for easy end-to-end testing of splay tree")

    parser.add_argument("--install-dir", help = "path to the directory containing all installed "\
                        "executables produced by the build system", type = str, dest = "dir",
                        metavar = "PATH", required = True)
    parser.add_argument("--tree", help = "the tree to use in tests", required = True,
                        choices = ["splay", "splay+"])
    parser.add_argument("-k", "--keys", help = "the number of random keys in test", type = int,
                        dest = "keys", metavar = "N", required = True)
    parser.add_argument("-q", "--queries", help = "the number of random queries in test", type = int,
                        dest = "queries", metavar = "N", required = True)

    args = parser.parse_args()

    if not Path.exists(Path(args.dir)):
        raise Exception(f"\"{args.dir}\" does not exist")
    if not Path.is_dir(Path(args.dir)):
        raise Exception(f"\"{args.dir}\" does not refer to a directory")
    if args.keys <= 0:
        raise Exception("the number of keys is expected to be a positive number")
    if args.queries <= 0:
        raise Exception("the number of queries is expected to be a positive number")

    return args


def main() -> None:
    args = parse_options()

    generator : str = f"{args.dir}/generator"
    if not Path.exists(Path(generator)):
        raise Exception("no test generator found")

    driver : str = f"{args.dir}/driver"
    if not Path.exists(Path(driver)):
        raise Exception("no driver found")

    script_dir : Path = Path(sys.argv[0]).parent
    data_dir : str = f"{str(script_dir)}/data"
    Path.mkdir(Path(data_dir), exist_ok = True)

    print(colored("Generating test...", "green"))
    generate_test_cmd : list[str] = \
        [generator, "--n-keys", f"{args.keys}", "--n-queries", f"{args.queries}"]
    test_file_name : str = f"{data_dir}/{args.keys}_{args.queries}.test"
    with open(test_file_name, "w+") as test_file:
        subprocess.run(generate_test_cmd, stdout = test_file, check = True)

    print(colored("Generating answer...", "green"))
    generate_ans_cmd : list[str] = [driver, "-a", "--tree", "std::set"]
    ans_file_name : str = f"{data_dir}/{args.keys}_{args.queries}.ans"
    with open(test_file_name, "r") as test_file, open(ans_file_name, "w+") as ans_file:
        subprocess.run(generate_ans_cmd, stdin = test_file, stdout = ans_file, check = True)

    print(colored("Running test...", "green"))
    generate_res_cmd : list[str] = [driver, "-a", "--tree", args.tree]
    res_file_name : str = f"{data_dir}/{args.keys}_{args.queries}.res"
    with open(test_file_name, "r") as test_file, open(res_file_name, "w+") as res_file:
        subprocess.run(generate_res_cmd, stdin = test_file, stdout = ans_file, check = True)

    with open(res_file_name, "r") as res_file, open(ans_file_name, "r") as ans_file:
        res : str = res_file.read()
        ans : str = ans_file.read()
        if res == ans:
            print(colored("Test passed!", "green"))
        else:
            print(colored(f"Test failed...", "red"))
            print(f"Result: {res}")
            print(f"Correct result: {ans}")


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"Caught an instance of type {type(e)}.\nwhat(): {e}")
