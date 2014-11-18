#!/usr/bin/env python

from setuptools import setup, find_packages

setup(
        name='gpgpass',
        version='0.0.1',
        description='GPG based password manager',
        author='Josh Bressers',
        author_email='josh@bress.net',
        url='https://github.com/joshbressers/gpg-pass',
        license="GPLv3",
        entry_points = {
            'console_scripts' : [
                'get-password = gpgpass.get_password.main:main',
                'set-password = gpgpass.set_password.main:main'
            ]

        },
        packages=find_packages(exclude="test"),
     )
