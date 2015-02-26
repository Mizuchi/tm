def FlagsForFile(filename, **kwargs):
    return {
        'flags': ['-stdlib=libc++', '-std=c++1z', '-Wall',
                  '-Wextra', '-Werror', '-I.'],
        'do_cache': True
    }
