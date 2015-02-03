#!/usr/bin/env python

from setuptools import setup, find_packages

setup(
        name='gpgpass',
        version='0.0.2',
        description='GPG based password manager',
        author='Josh Bressers',
        author_email='josh@bress.net',
        url='https://github.com/joshbressers/gpg-pass',
        license="GPLv3",
        entry_points = {
            'console_scripts' : [
                'gpgpass = gpgpass.main:main',
            ]

        },
        packages=find_packages(exclude="test"),
     )
