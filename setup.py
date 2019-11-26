"""The setup script for installing and distributing the nes-py package."""
from glob import glob
import os
from distutils.core import setup, Extension
import subprocess

# read the contents from the README file
with open('README.md') as README_file:
    README = README_file.read()

# TODO: use cpython modules... Need to add more modules
DE_module = Extension('DE',
                      sources=['common.c', 'DE.c'],
                      extra_compile_args=['-fPIC', '-shared'])

setup(
    name='devo',  # temporary name
    version='0.1',
    description='Adaptive Differential Evolution',
    packages=['devo'],
    ext_modules=[DE_module],
)
