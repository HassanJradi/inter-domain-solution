# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def options(opt):
#     pass

# def configure(conf):
#     conf.check_nonfatal(header_name='stdint.h', define_name='HAVE_STDINT_H')

def build(bld):
    module = bld.create_ns3_module('secure-mobility-solution', ['internet', 'network', 'core'])
    module.source = [
    'model/basic-net-device.cc',
	'model/mobile-node-net-device.cc',
	'model/media-access-gateway-net-device.cc',
	'model/home-authentication-server-net-device.cc',
	'model/visited-authentication-server-net-device.cc',
	'model/utils.cc',
    'helper/basic-net-device-helper.cc',
    ]

    module_test = bld.create_ns3_module_test_library('secure-mobility-solution')
    module_test.source = [
        'test/secure-mobility-solution-test-suite.cc',
        ]
    # Tests encapsulating example programs should be listed here
    if (bld.env['ENABLE_EXAMPLES']):
        module_test.source.extend([
        #    'test/secure-mobility-solution-examples-test-suite.cc',
             ])

    headers = bld(features='ns3header')
    headers.module = 'secure-mobility-solution'
    headers.source = [
	'model/basic-net-device.h',
	'model/mobile-node-net-device.h',
	'model/media-access-gateway-net-device.h',
	'model/home-authentication-server-net-device.h',
	'model/visited-authentication-server-net-device.h',
	'model/utils.h',
    'helper/basic-net-device-helper.h'
    ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')

    # bld.ns3_python_bindings()

