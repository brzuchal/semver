--TEST--
SemverVersion::__construct() tests
--SKIPIF--
<?php
if (!extension_loaded('semver')) {
	echo 'skip';
}
?>
--FILE--
<?php
assert((new SemverVersion(1, 0, 0)) instanceof SemverVersion);
assert((new SemverVersion(0, 0, 0, "alpha")) instanceof SemverVersion);
assert((new SemverVersion(1, 5, 8, "beta", "20200305")) instanceof SemverVersion);

$ver = new SemverVersion(1, 5, 8);
var_dump($ver->major);
var_dump($ver->minor);
var_dump($ver->patch);
var_dump($ver->prerelease);
var_dump($ver->metadata);

$ver = new SemverVersion(1, 5, 8, "alpha", "20200305");
var_dump($ver->major);
var_dump($ver->minor);
var_dump($ver->patch);
var_dump($ver->prerelease);
var_dump($ver->metadata);
?>
--EXPECT--
int(1)
int(5)
int(8)
NULL
NULL
int(1)
int(5)
int(8)
string(5) "alpha"
string(8) "20200305"
