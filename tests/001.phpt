--TEST--
semver_numeric() Basic tests
--SKIPIF--
<?php
if (!extension_loaded('semver')) {
	echo 'skip';
}
?>
--FILE--
<?php
assert(semver_numeric('0.5.9') == 59);
assert(semver_numeric('1.3.6') == 136);
assert(semver_numeric('1.3.6-beta+123456789') == 1025);
try {
    semver_numeric('1.3.6-be$ta+12@3456789');
} catch (InvalidArgumentException $exception) {
	echo $exception->getMessage();
}
?>
--EXPECT--
Invalid argument, version is not valid version
