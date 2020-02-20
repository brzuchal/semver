PHP_ARG_ENABLE([semver],
  [whether to enable semver support],
  [AS_HELP_STRING([--enable-semver],
    [Enable semver support])],
  [no])

if test "$PHP_SEMVER" != "no"; then
  AC_DEFINE(HAVE_SEMVER, 1, [ Have semver support ])

  PHP_NEW_EXTENSION(semver, semver.c include/semver.c, $ext_shared)
  PHP_ADD_BUILD_DIR($ext_builddir/include)
fi
