#!/usr/bin/env python
# encoding: utf-8
#| Copyright Inria May 2015
#| This project has received funding from the European Research Council (ERC) under
#| the European Union's Horizon 2020 research and innovation programme (grant
#| agreement No 637972) - see http://www.resibots.eu
#|
#| Contributor(s):
#|   - Jean-Baptiste Mouret (jean-baptiste.mouret@inria.fr)
#|   - Antoine Cully (antoinecully@gmail.com)
#|   - Kontantinos Chatzilygeroudis (konstantinos.chatzilygeroudis@inria.fr)
#|   - Federico Allocati (fede.allocati@gmail.com)
#|   - Vaios Papaspyros (b.papaspyros@gmail.com)
#|   - Roberto Rama (bertoski@gmail.com)
#|
#| This software is a computer library whose purpose is to optimize continuous,
#| black-box functions. It mainly implements Gaussian processes and Bayesian
#| optimization.
#| Main repository: http://github.com/resibots/limbo
#| Documentation: http://www.resibots.eu/limbo
#|
#| This software is governed by the CeCILL-C license under French law and
#| abiding by the rules of distribution of free software.  You can  use,
#| modify and/ or redistribute the software under the terms of the CeCILL-C
#| license as circulated by CEA, CNRS and INRIA at the following URL
#| "http://www.cecill.info".
#|
#| As a counterpart to the access to the source code and  rights to copy,
#| modify and redistribute granted by the license, users are provided only
#| with a limited warranty  and the software's author,  the holder of the
#| economic rights,  and the successive licensors  have only  limited
#| liability.
#|
#| In this respect, the user's attention is drawn to the risks associated
#| with loading,  using,  modifying and/or developing or reproducing the
#| software by the user in light of its specific status of free software,
#| that may mean  that it is complicated to manipulate,  and  that  also
#| therefore means  that it is reserved for developers  and  experienced
#| professionals having in-depth computer knowledge. Users are therefore
#| encouraged to load and test the software's suitability as regards their
#| requirements in conditions enabling the security of their systems and/or
#| data to be ensured and,  more generally, to use and operate it in the
#| same conditions as regards security.
#|
#| The fact that you are presently reading this means that you have had
#| knowledge of the CeCILL-C license and that you accept its terms.
#|
#! /usr/bin/env python
# JB Mouret / Inria - 2015

"""
Quick n dirty libcmaes detection
"""

import os, glob, types
from waflib.Configure import conf


def options(opt):
	opt.add_option('--libcmaes', type='string', help='path to libcmaes', dest='libcmaes')


@conf
def check_libcmaes(conf):
	if conf.options.libcmaes:
		includes_check = [conf.options.libcmaes + '/include']
		libs_check = [conf.options.libcmaes + '/lib']
	else:
		includes_check = ['/usr/local/include', '/usr/include']
		libs_check = ['/usr/local/lib', '/usr/lib', '/usr/lib/x86_64-linux-gnu/']

	try:
		conf.start_msg('Checking for libcmaes includes (optional)')
		res = conf.find_file('libcmaes/cmaes.h', includes_check)
		conf.end_msg('ok')
	except:
		conf.end_msg('Not found', 'RED')
		return 1
	conf.start_msg('Checking for libcmaes libs')
	found = False
	for lib in ['libcmaes.so', 'libcmaes.a', 'libcmaes.dylib']:
		try:
			found = found or conf.find_file(lib, libs_check)
		except:
			continue
	if not found:
		conf.end_msg('Not found', 'RED')
		return 1
	else:
		conf.end_msg('ok')
		conf.env.INCLUDES_LIBCMAES = includes_check
		conf.env.LIBPATH_LIBCMAES = libs_check
		conf.env.DEFINES_LIBCMAES = ['USE_LIBCMAES']
		conf.env.LIB_LIBCMAES= ['cmaes']
	return 1
