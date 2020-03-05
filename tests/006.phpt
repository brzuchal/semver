--TEST--
SemverVersion::createFromString() tests
--SKIPIF--
<?php
if (!extension_loaded('semver')) {
	echo 'skip';
}
?>
--FILE--
<?php
assert(SemverVersion::createFromString('1.0.0') instanceof SemverVersion);
assert(SemverVersion::createFromString('0.0.0') instanceof SemverVersion);
assert(SemverVersion::createFromString('1.5.8') instanceof SemverVersion);

var_dump(SemverVersion::createFromString('1.0.0'));
var_dump(SemverVersion::createFromString('1.0'));
?>
--EXPECT--
object(SemverVersion)#1 (5) {
  ["major"]=>
  int(1)
  ["minor"]=>
  int(0)
  ["patch"]=>
  int(0)
  ["prerelease"]=>
  NULL
  ["metadata"]=>
  NULL
}
object(SemverVersion)#1 (5) {
  ["major"]=>
  int(1)
  ["minor"]=>
  int(0)
  ["patch"]=>
  int(0)
  ["prerelease"]=>
  NULL
  ["metadata"]=>
  NULL
}
