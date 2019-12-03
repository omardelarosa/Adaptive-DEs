"""The setup script for installing and distributing the nes-py package."""
from glob import glob
import os
from distutils.core import setup, Extension
import subprocess

# read the contents from the README file
with open('README.md') as README_file:
    README = README_file.read()

# Index of all supported C modules
C_MODULE_FILE_NAMES = [
    ('DE', 'DE.c'),
    ('LSHADE', 'LSHADE.c'),
    ('SHADE', 'SHADE.c'),
    ('jDE', 'jDE.c'),
    ('JADE', 'JADE.c'),
    ('CoDE', 'CoDE.c'),
]

MODULES = []

for PYTHON_MODULE_NAME, C_MODULE_FILE_NAME in C_MODULE_FILE_NAMES:
    module = Extension(PYTHON_MODULE_NAME,
                       sources=['common.c', C_MODULE_FILE_NAME],
                       extra_compile_args=['-fPIC', '-shared'])
    MODULES.append(module)


setup(
    name='devo',  # temporary name
    version='0.1',
    description='Adaptive Differential Evolution',
    packages=['devo'],
    ext_modules=MODULES,
)
