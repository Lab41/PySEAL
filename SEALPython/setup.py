import os, sys

from distutils.core import setup, Extension
from distutils import sysconfig

cpp_args = ['-std=c++11']

ext_modules = [
    Extension(
        'seal',
        ['SEALWrapper.cpp'],
        include_dirs=['/usr/include/python3.5', 'pybind11/include', '/SEAL/SEAL'],
        language='c++',
        extra_compile_args = cpp_args,
        extra_objects=['/SEAL/bin/libseal.a'],
    ),
]

setup(
    name='seal',
    version='2.2',
    author='Todd Stavish',
    author_email='toddstavish@gmail.com',
    description='Python wrapper for SEAL',
    ext_modules=ext_modules,
)
