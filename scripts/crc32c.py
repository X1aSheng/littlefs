#!/usr/bin/env python3

# prevent local imports
__import__('sys').path.pop(0)

import io
import os
import struct
import sys


def openio(path, mode='r', buffering=-1):
    # allow '-' for stdin/stdout
    if path == '-':
        if 'r' in mode:
            return os.fdopen(os.dup(sys.stdin.fileno()), mode, buffering)
        else:
            return os.fdopen(os.dup(sys.stdout.fileno()), mode, buffering)
    else:
        return open(path, mode, buffering)

def crc32c(data, crc=0):
    crc ^= 0xffffffff
    for b in data:
        crc ^= b
        for j in range(8):
            crc = (crc >> 1) ^ ((crc & 1) * 0x82f63b78)
    return 0xffffffff ^ crc


def main(paths, **args):
    # interpret as sequence of hex bytes
    if args.get('hex'):
        print('%08x' % crc32c(bytes(int(path, 16) for path in paths)))

    # interpret as strings
    elif args.get('string'):
        for path in paths:
            print('%08x' % crc32c(path.encode('utf8')))

    # default to interpreting as paths
    else:
        if not paths:
            paths = [None]

        for path in paths:
            with openio(path or '-', 'rb') as f:
                # calculate crc
                crc = 0
                while True:
                    block = f.read(io.DEFAULT_BUFFER_SIZE)
                    if not block:
                        break

                    crc = crc32c(block, crc)

                # print what we found
                if path is not None:
                    print('%08x  %s' % (crc, path))
                else:
                    print('%08x' % crc)


if __name__ == "__main__":
    import argparse
    import sys
    parser = argparse.ArgumentParser(
            description="Calculates crc32cs.",
            allow_abbrev=False)
    parser.add_argument(
            'paths',
            nargs='*',
            help="Paths to read. Reads stdin by default.")
    parser.add_argument(
            '-x', '--hex',
            action='store_true',
            help="Interpret as a sequence of hex bytes.")
    parser.add_argument(
            '-s', '--string',
            action='store_true',
            help="Interpret as strings.")
    sys.exit(main(**{k: v
            for k, v in vars(parser.parse_intermixed_args()).items()
            if v is not None}))
