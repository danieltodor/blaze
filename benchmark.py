#! /usr/bin/python
import timeit
import subprocess
import sys

NUMBER = 1000
PRECISION = 2


def main(argv):
    command = lambda: subprocess.Popen(
        argv,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.STDOUT
    ).communicate()
    result = timeit.timeit(command, number=NUMBER)
    multi = result
    single = result / NUMBER
    print(
        f'Executing {NUMBER} times took: {round(multi, ndigits=PRECISION)} seconds.\n'
        f'Single execution time: {round(single * 1000, ndigits=PRECISION)} milliseconds.'
    )


if __name__ == '__main__':
    _argv = sys.argv[1:]
    if not _argv:
        _argv = [
            './build/bin/blaze',
            'bash',
            '-s',
            '0',
            '-f',
            '10'
        ]
    main(_argv)
