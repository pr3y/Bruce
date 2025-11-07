"""
This script is a SCons pre-build script for PlatformIO projects.

It generates a header file `include/current_year.h` that contains the current year.
"""
import datetime
import os


def generate_build_header():
    """
    Generates a header file with the current year.
    """
    year = datetime.datetime.now().year
    header_content = f'#pragma once\n#define CURRENT_YEAR {year}\n'

    output_path = os.path.join("include", "current_year.h")
    os.makedirs("include", exist_ok=True)

    with open(output_path, "w") as f:
        f.write(header_content)


def before_build(source, target, env):
    """
    This function is executed before the build process.
    """
    generate_build_header()


# Hook for PlatformIO
def on_pre_build(env):
    """
    This function is a hook for PlatformIO that is executed before the build process.
    """
    before_build(None, None, env)


# Register the pre-build hook
Import("env")
on_pre_build(env)
