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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_semver.h"

#include "include/semver.h"

#define SLICE_SIZE   50
#define DELIMITER    "."
#define PR_DELIMITER "-"
#define MT_DELIMITER "+"
#define DELIMITERS   DELIMITER PR_DELIMITER MT_DELIMITER

static void
concat_num (char * str, int x, char * sep) {
  char buf[SLICE_SIZE] = {0};
  if (sep == NULL) sprintf(buf, "%d", x);
  else sprintf(buf, "%s%d", sep, x);
  strcat(str, buf);
}

static void
concat_char (char * str, char * x, char * sep) {
  char buf[SLICE_SIZE] = {0};
  sprintf(buf, "%s%s", sep, x);
  strcat(str, buf);
}

void
fixed_semver_render (semver_t *x, char *dest) {
	concat_num(dest, x->major, NULL);
  	concat_num(dest, x->minor, DELIMITER);
    concat_num(dest, x->patch, DELIMITER);
    if (x->prerelease) concat_char(dest, x->prerelease, PR_DELIMITER);
    if (x->metadata) concat_char(dest, x->metadata, MT_DELIMITER);
}

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

/* {{{ int semver_numeric( string $version )
 */
PHP_FUNCTION(semver_numeric)
{
	semver_t ver = {0};
	zend_string *zver;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(zver)
	ZEND_PARSE_PARAMETERS_END();

    if (semver_parse(ZSTR_VAL(zver), &ver) == FAILURE) {
		INVALID_ARGUMENT("version")
	}

	RETURN_LONG(semver_numeric(&ver));

	semver_free(&ver);
}
/* }}}*/

/* {{{ bool semver_valid( string $version )
 */
PHP_FUNCTION(semver_valid)
{
	semver_t ver = {0};
	zend_string *zver;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(zver)
	ZEND_PARSE_PARAMETERS_END();

    if (semver_parse(ZSTR_VAL(zver), &ver) == SUCCESS) {
		semver_free(&ver);
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}}*/

/* {{{ int semver_compare( string $x, string $y )
 */
PHP_FUNCTION(semver_compare)
{
	semver_t ver_x = {0}, ver_y = {0};
	zend_string *zver_x, *zver_y;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(zver_x)
		Z_PARAM_STR(zver_y)
	ZEND_PARSE_PARAMETERS_END();

    if (semver_parse(ZSTR_VAL(zver_x), &ver_x) == FAILURE) {
		INVALID_ARGUMENT("x")
	}
    if (semver_parse(ZSTR_VAL(zver_y), &ver_y) == FAILURE) {
		INVALID_ARGUMENT("y")
	}

	RETURN_LONG(semver_compare(ver_x, ver_y));

	semver_free(&ver_x);
	semver_free(&ver_y);
}
/* }}}*/

/* {{{ bool semver_satisfies( string $s, string $y, string $op )
 */
PHP_FUNCTION(semver_satisfies)
{
	semver_t ver_x = {0}, ver_y = {0};
	zend_string *zver_x, *zver_y, *zop;
	ssize_t ret;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_STR(zver_x)
		Z_PARAM_STR(zver_y)
		Z_PARAM_STR(zop)
	ZEND_PARSE_PARAMETERS_END();

    if (semver_parse(ZSTR_VAL(zver_x), &ver_x) == FAILURE) {
		INVALID_ARGUMENT("x")
	}
    if (semver_parse(ZSTR_VAL(zver_y), &ver_y) == FAILURE) {
		INVALID_ARGUMENT("y")
	}

	ret = semver_satisfies(ver_x, ver_y, ZSTR_VAL(zop));

	semver_free(&ver_x);
	semver_free(&ver_y);

	RETURN_BOOL(ret);
}
/* }}} */

/* {{{ string semver_bump( string $version )
 */
PHP_FUNCTION(semver_bump)
{
	semver_t ver = {0};
	zend_string *zver;
	char rendered[250] = {0};

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(zver)
	ZEND_PARSE_PARAMETERS_END();

    if (semver_parse(ZSTR_VAL(zver), &ver) == FAILURE) {
		INVALID_ARGUMENT("version")
	}

	ver.major++;
	ver.minor = 0;
	ver.patch = 0;
	fixed_semver_render(&ver, (char *) rendered);
	semver_free(&ver);

	RETURN_STRING(rendered);
}
/* }}} */

/* {{{ string semver_bump_minor( string $version )
 */
PHP_FUNCTION(semver_bump_minor)
{
	semver_t ver = {0};
	zend_string *zver;
	char rendered[250] = {0};

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(zver)
	ZEND_PARSE_PARAMETERS_END();

    if (semver_parse(ZSTR_VAL(zver), &ver) == FAILURE) {
		INVALID_ARGUMENT("version")
	}

	ver.minor++;
	ver.patch = 0;
	fixed_semver_render(&ver, (char *) rendered);
	semver_free(&ver);

	RETURN_STRING(rendered);
}
/* }}} */

/* {{{ string semver_patch( string $version )
 */
PHP_FUNCTION(semver_bump_patch)
{
	semver_t ver = {0};
	zend_string *zver;
	char rendered[250] = {0};

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(zver)
	ZEND_PARSE_PARAMETERS_END();

    if (semver_parse(ZSTR_VAL(zver), &ver) == FAILURE) {
		INVALID_ARGUMENT("version")
	}

	ver.patch++;
	fixed_semver_render(&ver, (char *) rendered);
	semver_free(&ver);

	RETURN_STRING(rendered);
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(semver)
{
#if defined(ZTS) && defined(COMPILE_DL_SEMVER)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(semver)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "semver support", "enabled");
	php_info_print_table_row(2, "semver version", PHP_SEMVER_VERSION);
	php_info_print_table_end();
}
/* }}} */

/* {{{ arginfo
 */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_semver_numeric, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_semver_valid, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, version, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_semver_compare, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_semver_satisfies, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, op, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_semver_bump, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, version, IS_STRING, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ semver_functions[]
 */
static const zend_function_entry semver_functions[] = {
	PHP_FE(semver_numeric,		arginfo_semver_numeric)
	PHP_FE(semver_valid,		arginfo_semver_valid)
	PHP_FE(semver_compare,		arginfo_semver_compare)
	PHP_FE(semver_satisfies,	arginfo_semver_satisfies)
	PHP_FE(semver_bump,			arginfo_semver_bump)
	PHP_FE(semver_bump_minor,	arginfo_semver_bump)
	PHP_FE(semver_bump_patch,	arginfo_semver_bump)
	PHP_FE_END
};
/* }}} */

/* {{{ semver_module_entry
 */
zend_module_entry semver_module_entry = {
	STANDARD_MODULE_HEADER,
	"semver",					/* Extension name */
	semver_functions,			/* zend_function_entry */
	NULL,							/* PHP_MINIT - Module initialization */
	NULL,							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(semver),			/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(semver),			/* PHP_MINFO - Module info */
	PHP_SEMVER_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SEMVER
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(semver)
#endif
