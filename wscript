#!/usr/bin/python3
import os

options = lambda cfg: cfg.load('compiler_cxx waf_unit_test')
configure = options

def build(bld):
    bld.add_group()
    for root, dirs, files in os.walk('test'):
        for filename in files:
            if os.path.splitext(filename)[1] == ".cpp":
                arg = dict(
                    features='test',
                    source=os.path.join(root, filename),
                    target=filename + '.test_c++14',
                    includes='.',
                    cxxflags=['-std=c++14', '-Wall', '-Werror'],
                )
                bld.program(**arg)
                if filename.endswith("11.cpp"):
                    arg['target'] = filename + ".test_c++11"
                    arg['cxxflags'] = ['-std=c++11', '-Wall', '-Werror']
                    bld.program(**arg)
    from waflib.Tools import waf_unit_test
    bld.add_post_fun(waf_unit_test.summary)
