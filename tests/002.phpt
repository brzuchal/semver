--TEST--
semver_valid() Basic test
--SKIPIF--
<?php
if (!extension_loaded('semver')) {
	echo 'skip';
}
?>
--FILE--
<?php
assert(semver_valid('2') === true); // major
assert(semver_valid('1.2') === true); // minor
assert(semver_valid('1.2.12') === true);
assert(semver_valid('1.2.12-beta.alpha.1.1') === true); // prerelease
assert(semver_valid('1.2.12+20130313144700') === true); // metadata
assert(semver_valid('1.2.12-alpha.1+20130313144700') === true); // prerelease+metadata
assert(semver_valid('2a') === false); // major
assert(semver_valid('1.2a') === false); // minor
assert(semver_valid('1.2.12a') === false);
assert(semver_valid('1.2.12a-beta.$alpha.1.1') === false); // prerelease
assert(semver_valid('1.2.12+201303$13144700') === false); // metadata
assert(semver_valid('1.2.12-alpha.$1+201303$13144700') === false); // prerelease+metadata
?>
--EXPECT--
