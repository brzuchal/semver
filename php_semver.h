/*
  +----------------------------------------------------------------------+
  | Semantic Versioning                                                  |
  +----------------------------------------------------------------------+
  | Copyright (c) Michał Brzuchalski 2020                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: brzuchal <brzuchal@php.net>                                  |
  +----------------------------------------------------------------------+
 */

#ifndef PHP_SEMVER_H
# define PHP_SEMVER_H

#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "include/semver.h"

extern zend_module_entry semver_module_entry;
# define phpext_semver_ptr &semver_module_entry

# define PHP_SEMVER_VERSION "0.1.0"

# define INVALID_ARGUMENT(argname) \
    zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, \
        "Invalid argument, %s is not valid version", argname); \
    return;

static void concat_num (char * str, int x, char * sep);
static void concat_char (char * str, char * x, char * sep);
void fixed_semver_render (semver_t *x, char *dest);

# if defined(ZTS) && defined(COMPILE_DL_SEMVER)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_SEMVER_H */
