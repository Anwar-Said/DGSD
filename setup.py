# -*- coding: utf-8 -*-

import os
import sys


try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup

setup(
    name='DGSD',
    version='1.0.0',
    author='Anwar Said',
    author_email='anwar.said@itu.edu.pk',
    description='DGSD: Distributed Graph Representation via Graph Statistical Properties',
    packages=['DGSD',],
    url='https://github.com/Anwar-Said/DGSD',
    download_url = 'https://github.com/xgfs/netlsd/archive/0.1.tar.gz',
    license='MIT',
    install_requires=[
        'numpy',
        'scipy',
        'multiprocessing'
    ],
    classifiers=[
        'Development Status :: 3 - Alpha',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3.7',
        'Topic :: Scientific/Engineering :: Graph Theory',
        'Topic :: Scientific/Engineering :: Graph Representation',
    ],
    long_description=open('README.rst').read(),
)