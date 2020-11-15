#!/usr/bin/env python3

import subprocess
import sys
import os

resolutions = [
#    [1280, 1024],
#    [1600, 1200],
#    [1680, 1050],
    [1920, 1080]
]

for row in resolutions:
    x, y = row
    args = [
        '--replay-path', os.path.dirname(os.path.realpath(__file__)) + '/take-screenshot.json',
        '--window-width', str(x),
        '--window-height', str(y),
        '--output-folder', os.path.dirname(os.path.realpath(__file__)) + '/results/',
        '--fullscreen', 'true'
    ]
    subprocess.call(['echo'] + args)
    subprocess.call(['./tools/launch.sh', '--'] + args, cwd=os.path.dirname(os.path.realpath(__file__)) + '/../../')

subprocess.call(['rm', '-vrf', './results/logs'], cwd=os.path.dirname(os.path.realpath(__file__)))
