--TEST--
semver_bump*() Basic test
--SKIPIF--
<?php
if (!extension_loaded('semver')) {
	echo 'skip';
}
?>
--FILE--
<?php
assert(semver_bump('1.5.8') === '2.0.0');
assert(semver_bump('1.0.0') === '2.0.0');
assert(semver_bump('0.0.0') === '1.0.0');

assert(semver_bump_minor('1.5.8') === '1.6.0');
assert(semver_bump_minor('1.0.0') === '1.1.0');

assert(semver_bump_patch('1.5.8') === '1.5.9');
assert(semver_bump_patch('1.0.0') === '1.0.1');
?>
--EXPECT--
