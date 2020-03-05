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
#include "zend_interfaces.h"
#include "zend_types.h"
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

/* {{{ Semver object handlers
 */
static inline semver_object *semver_object_from_zend_object(zend_object *object) {
	return (semver_object*)((char*)(object) - XtOffsetOf(semver_object, std));
}
static zend_object *semver_object_new(zend_class_entry *class_type) /* {{{ */
{
    semver_object *intern = ecalloc(1, sizeof(semver_object) + zend_object_properties_size(class_type));

    zend_object_std_init(&intern->std, class_type);
    object_properties_init(&intern->std, class_type);
    intern->std.handlers = &semver_object_handlers;

    return &intern->std;
}
static void semver_object_free_storage(zend_object *object) /* {{{ */
{
	semver_object *intern = semver_object_from_zend_object(object);

	if (intern->semver_object_ptr != NULL) {
		semver_t *semver_ptr = intern->semver_object_ptr;
		// semver_free(semver_ptr);
		semver_ptr->prerelease = NULL;
		semver_ptr->metadata = NULL;
		intern->semver_object_ptr = NULL;
	}
	zend_object_std_dtor(&intern->std);
}
static void semver_object_ctor(INTERNAL_FUNCTION_PARAMETERS)
{
	semver_object *intern;
	semver_t *semver_object_ptr;

	zend_long major, minor, patch;
	zend_string *prerelease = NULL, *metadata = NULL;
	// zval *cf = return_value;

	ZEND_PARSE_PARAMETERS_START(3,5)
		Z_PARAM_LONG(major)
		Z_PARAM_LONG(minor)
		Z_PARAM_LONG(patch)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(prerelease)
		Z_PARAM_STR(metadata)
	ZEND_PARSE_PARAMETERS_END();

	if (major < 0) {
		INVALID_ARGUMENT_PART("major", "non-negative major version");
	}
	if (minor < 0) {
		INVALID_ARGUMENT_PART("minor", "non-negative minor version");
	}
	if (patch < 0) {
		INVALID_ARGUMENT_PART("patch", "non-negative patch version");
	}
	if (prerelease && (semver_is_valid(ZSTR_VAL(prerelease)) == 0)) {
		INVALID_ARGUMENT_PART("prerelease", "pre-release value");
	}
	if (metadata && (semver_is_valid(ZSTR_VAL(metadata)) == 0)) {
		INVALID_ARGUMENT_PART("metadata", "metadata value");
	}

	semver_object_ptr = (semver_t *) emalloc(sizeof(semver_t));
	semver_object_ptr->major = major;
	semver_object_ptr->minor = minor;
	semver_object_ptr->patch = patch;
	semver_object_ptr->prerelease = NULL;
	semver_object_ptr->metadata = NULL;
	if (prerelease) {
		semver_object_ptr->prerelease = ZSTR_VAL(prerelease);
	}
	if (metadata) {
		semver_object_ptr->metadata = ZSTR_VAL(metadata);
	}
	intern = semver_object_from_zend_object(Z_OBJ_P(return_value));
	intern->semver_object_ptr = semver_object_ptr;
}

#if PHP_VERSION_ID >= 80000
#define handler_object zend_object
#define handler_property zend_string
#define semver_object_from_handler_object(obj) semver_object_from_zend_object(obj)
#else
#define handler_object zval
#define handler_property zval
#define semver_object_from_handler_object(obj) semver_object_from_zend_object(Z_OBJ_P(obj))
#endif

#define PHP_SEMVER_ADD_PROPERTY_LONG(n,f) \
	ZVAL_LONG(&zv, (zend_long)intern->semver_object_ptr->f); \
	zend_hash_str_update(props, n, sizeof(n)-1, &zv);
#define PHP_SEMVER_ADD_PROPERTY_STRING(n,f) \
	if (intern->semver_object_ptr->f == NULL) { \
		ZVAL_NULL(&zv); \
	} else { \
		ZVAL_STRING(&zv, intern->semver_object_ptr->f); \
	} \
	zend_hash_str_update(props, n, sizeof(n)-1, &zv);

static HashTable *semver_object_get_properties_for(handler_object *object, zend_prop_purpose purpose)
{
	HashTable *props = zend_std_get_properties(object);
	semver_object *intern = semver_object_from_handler_object(object);
	zval zv;

	switch (purpose) {
		case ZEND_PROP_PURPOSE_DEBUG:
		case ZEND_PROP_PURPOSE_SERIALIZE:
		case ZEND_PROP_PURPOSE_VAR_EXPORT:
		case ZEND_PROP_PURPOSE_JSON:
		case ZEND_PROP_PURPOSE_ARRAY_CAST:
			break;
		default:
			return zend_std_get_properties_for(object, purpose);
	}

	PHP_SEMVER_ADD_PROPERTY_LONG("major", major);
	PHP_SEMVER_ADD_PROPERTY_LONG("minor", minor);
	PHP_SEMVER_ADD_PROPERTY_LONG("patch", patch);
	PHP_SEMVER_ADD_PROPERTY_STRING("prerelease", prerelease);
	PHP_SEMVER_ADD_PROPERTY_STRING("metadata", metadata);

	return props;
}

static HashTable *semver_object_get_properties(handler_object *object)
{
	semver_object *intern = semver_object_from_handler_object(object);
	HashTable *props = zend_std_get_properties(object);
	zval zv;

	PHP_SEMVER_ADD_PROPERTY_LONG("major", major);
	PHP_SEMVER_ADD_PROPERTY_LONG("minor", minor);
	PHP_SEMVER_ADD_PROPERTY_LONG("patch", patch);
	PHP_SEMVER_ADD_PROPERTY_STRING("prerelease", prerelease);
	PHP_SEMVER_ADD_PROPERTY_STRING("metadata", metadata);

	return props;
}

static zval *semver_object_read_property(handler_object *object, handler_property *name, int type, void **cache_slot, zval *rv)
{
	semver_object *intern = semver_object_from_handler_object(object);
	zval *retval = rv;
#if PHP_VERSION_ID >= 80000
#define GET_LONG_FROM_STRUCT(n,m)            \
	if (strcmp(ZSTR_VAL(name), m) == 0) { \
		ZVAL_LONG(retval, intern->semver_object_ptr->n); \
		break; \
	}
#define GET_STRING_FROM_STRUCT(n,m)            \
	if (strcmp(ZSTR_VAL(name), m) == 0) { \
		ZVAL_STRING(retval, intern->semver_object_ptr->n); \
		break; \
	}
#else
#define GET_LONG_FROM_STRUCT(n,m)            \
	if (strcmp(Z_STRVAL_P(name), m) == 0) { \
		ZVAL_LONG(retval, intern->semver_object_ptr->n); \
		break; \
	}
#define GET_STRING_FROM_STRUCT(n,m)            \
	if (strcmp(Z_STRVAL_P(name), m) == 0) { \
		if (intern->semver_object_ptr->n == NULL) { \
			ZVAL_NULL(retval); \
			break; \
		} \
		ZVAL_STRING(retval, intern->semver_object_ptr->n); \
		break; \
	}
#endif
	do {
		GET_LONG_FROM_STRUCT(major, "major");
		GET_LONG_FROM_STRUCT(minor, "minor");
		GET_LONG_FROM_STRUCT(patch, "patch");
		GET_STRING_FROM_STRUCT(prerelease, "prerelease");
		GET_STRING_FROM_STRUCT(metadata, "metadata");
		/* didn't find any */
		retval = zend_std_read_property(object, name, type, cache_slot, rv);

		return retval;
	} while(0);

	return retval;
}

#define semver_unsupported(thing) \
	zend_throw_error(NULL, "SemverVersion objects are immutable and do not support " thing);

static zval *semver_object_write_property(handler_object *object, handler_property *name, zval *value, void **cache_slot)
{
	semver_unsupported("writing properties");

	return &EG(uninitialized_zval);
}

static int semver_object_has_property(handler_object *object, handler_property *name, int type, void **cache_slot)
{
	zval rv;
	zval *prop;
	int retval = 0;

	prop = semver_object_read_property(object, name, BP_VAR_IS, cache_slot, &rv);

	if (prop != &EG(uninitialized_zval)) {
		if (type == 2) {
			retval = 1;
		} else if (type == 1) {
			retval = zend_is_true(prop);
		} else if (type == 0) {
			retval = (Z_TYPE_P(prop) != IS_NULL);
		}
	} else {
		retval = zend_std_has_property(object, name, type, cache_slot);
	}

	return retval;
}
/* }}} */

static void semver_object_unset_property(handler_object *obj, handler_property *member, void **cache_slot) /* {{{ */
{
	semver_unsupported("unsetting properties");
}
/* }}} */

static zval *semver_object_get_property_ptr_ptr(handler_object *obj, handler_property *member, int type, void **cache_slot) /* {{{ */
{
	semver_unsupported("getting references to properties");
	return NULL;
}
/* }}} */

/* {{{ proto void SemverVersion::__construct( int $major [, int $minor [, int $patch [, string $prerelease [, string $metadata ]]]] )
   Argument constructor */
ZEND_METHOD(SemverVersion, __construct)
{
	return_value = getThis();
	semver_object_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto void SemverVersion::createFromString( string $version )
   Named constructor */
ZEND_METHOD(SemverVersion, createFromString)
{
	semver_object *intern;
	semver_t *semver_object_ptr;
	zend_string *zver;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(zver)
	ZEND_PARSE_PARAMETERS_END();

	semver_object_ptr = (semver_t *) emalloc(sizeof(semver_t));
	semver_object_ptr->major = 0;
	semver_object_ptr->minor = 0;
	semver_object_ptr->patch = 0;
	semver_object_ptr->prerelease = NULL;
	semver_object_ptr->metadata = NULL;
    if (semver_parse(ZSTR_VAL(zver), semver_object_ptr) == FAILURE) {
		INVALID_ARGUMENT("version")
	}

	object_init_ex(return_value, semver_ce_ptr);
	intern = semver_object_from_zend_object(Z_OBJ_P(return_value));
	intern->semver_object_ptr = semver_object_ptr;
}
/* }}} */

/* {{{ proto string SemverVersion::__toString()
   ToString method */
ZEND_METHOD(SemverVersion, __toString)
{
	semver_object *intern;
	char rendered[250] = {0};

	ZEND_PARSE_PARAMETERS_NONE();

	intern = semver_object_from_zend_object(Z_OBJ_P(getThis()));
	if (intern->semver_object_ptr == NULL) {
		zend_throw_error(NULL, "Invalid SemverVersion objects internal state");
		RETURN_NULL();
	}

	fixed_semver_render(intern->semver_object_ptr, (char *) rendered);

	RETURN_STRING(rendered);
}
/* }}} */

/* {{{ int semver_numeric( string $version )
 */
PHP_FUNCTION(semver_numeric)
{
	semver_t ver = {0,0,0};
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
	semver_t ver = {0,0,0};
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_semver_object___construct, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, major, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, minor, IS_LONG, 1)
    ZEND_ARG_TYPE_INFO(0, patch, IS_LONG, 1)
    ZEND_ARG_TYPE_INFO(0, prerelease, IS_STRING, 1)
    ZEND_ARG_TYPE_INFO(0, metadata, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_semver_object_createFromString, 0, 1, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, version, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_semver_object___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(semver)
{
    zend_class_entry tmp_ce;
    zend_function_entry semver_object_methods[] = {
        PHP_ME(SemverVersion, __construct,		arginfo_semver_object___construct, 		ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
        PHP_ME(SemverVersion, createFromString,	arginfo_semver_object_createFromString, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
        PHP_ME(SemverVersion, __toString,		arginfo_semver_object___toString, 		ZEND_ACC_PUBLIC)
        ZEND_MALIAS(SemverVersion, 	toString,
									__toString, arginfo_semver_object___toString, ZEND_ACC_PUBLIC)
        PHP_FE_END
    };
    INIT_CLASS_ENTRY(tmp_ce, "SemverVersion", semver_object_methods);
    tmp_ce.create_object 	= semver_object_new;
	tmp_ce.serialize 		= zend_class_serialize_deny;
  	tmp_ce.unserialize 		= zend_class_unserialize_deny;
    semver_ce_ptr = zend_register_internal_class_ex(&tmp_ce, NULL);
    memcpy(&semver_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers)); 
    semver_object_handlers.offset 				= XtOffsetOf(semver_object, std);
	semver_object_handlers.get_properties 		= semver_object_get_properties;
	semver_object_handlers.has_property 		= semver_object_has_property;
	semver_object_handlers.read_property 		= semver_object_read_property;
	semver_object_handlers.write_property 		= semver_object_write_property;
	semver_object_handlers.get_properties_for 	= semver_object_get_properties_for;
	semver_object_handlers.get_property_ptr_ptr = semver_object_get_property_ptr_ptr;
	semver_object_handlers.unset_property       = semver_object_unset_property;
	semver_object_handlers.free_obj = semver_object_free_storage;
	zend_declare_property_long(semver_ce_ptr, "major", sizeof("major")-1, 0, ZEND_ACC_PUBLIC);
	zend_declare_property_long(semver_ce_ptr, "minor", sizeof("minor")-1, 0, ZEND_ACC_PUBLIC);
	zend_declare_property_long(semver_ce_ptr, "patch", sizeof("patch")-1, 0, ZEND_ACC_PUBLIC);
	{
		zend_string *name = zend_string_init("prerelease", sizeof("prerelease") - 1, 1);
		zval val;
		ZVAL_NULL(&val);
		zend_declare_typed_property(semver_ce_ptr, name, &val, ZEND_ACC_PUBLIC, NULL, ZEND_TYPE_ENCODE(IS_STRING, 1));
		zend_string_release(name);
	}
	{
		zend_string *name = zend_string_init("metadata", sizeof("metadata") - 1, 1);
		zval val;
		ZVAL_NULL(&val);
		zend_declare_typed_property(semver_ce_ptr, name, &val, ZEND_ACC_PUBLIC, NULL, ZEND_TYPE_ENCODE(IS_STRING, 1));
		zend_string_release(name);
	}

	return SUCCESS;
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
	PHP_MINIT(semver),			/* PHP_MINIT - Module initialization */
	NULL,						/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(semver),			/* PHP_RINIT - Request initialization */
	NULL,						/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(semver),			/* PHP_MINFO - Module info */
	PHP_SEMVER_VERSION,			/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SEMVER
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(semver)
#endif
