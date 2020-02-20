--TEST--
semver_compare() Basic test
--SKIPIF--
<?php
if (!extension_loaded('semver')) {
	echo 'skip';
}
?>
--FILE--
<?php
assert(semver_compare('1', '0') === 1);
assert(semver_compare('1', '1') === 0);
assert(semver_compare('1', '3') === -1);
assert(semver_compare('1.5', '0.8') === 1);
assert(semver_compare('1.5', '1.3') === 1);
assert(semver_compare('1.2', '2.2') === -1);
assert(semver_compare('3.0', '1.5') === 1);
assert(semver_compare('1.5', '1.5') === 0);
assert(semver_compare('1.0.9', '1.0.0') === 1);
assert(semver_compare('1.0.9', '1.0.9') === 0);
assert(semver_compare('1.1.5', '1.1.9') === -1);
assert(semver_compare('1.2.2', '1.1.9') === 1);
assert(semver_compare('1.2.2', '1.2.9') === -1);
?>
--EXPECT--
