--TEST--
semver_satisfies() Basic test
--SKIPIF--
<?php
if (!extension_loaded('semver')) {
	echo 'skip';
}
?>
--FILE--
<?php
assert(semver_satisfies('1', '0', '>=') === true);
assert(semver_satisfies('1', '3', '>=') === false);
assert(semver_satisfies('1', '1', '>=') === true);
assert(semver_satisfies('1.5', '0.8', '>=') === true);
assert(semver_satisfies('1.2', '2.2', '>=') === false);
assert(semver_satisfies('3.0', '1.5', '>=') === true);
assert(semver_satisfies('1.0', '1.0', '>=') === true);
assert(semver_satisfies('1.0.9', '1.0.0', '>=') === true);
assert(semver_satisfies('1.1.5', '1.1.9', '>=') === false);
assert(semver_satisfies('1.2.2', '1.2.9', '>=') === false);
assert(semver_satisfies('1.0.0', '1.0.0', '>=') === true);

assert(semver_satisfies('1', '0', '<=') === false);
assert(semver_satisfies('1', '3', '<=') === true);
assert(semver_satisfies('1', '1', '<=') === true);
assert(semver_satisfies('1.5', '0.8', '<=') === false);
assert(semver_satisfies('1.2', '2.2', '<=') === true);
assert(semver_satisfies('3.0', '1.5', '<=') === false);
assert(semver_satisfies('1.0', '1.0', '<=') === true);
assert(semver_satisfies('1.0.9', '1.0.0', '<=') === false);
assert(semver_satisfies('1.1.5', '1.1.9', '<=') === true);
assert(semver_satisfies('1.2.2', '1.2.9', '<=') === true);
assert(semver_satisfies('1.0.0', '1.0.0', '<=') === true);

assert(semver_satisfies('1', '0', '=') === false);
assert(semver_satisfies('1', '3', '=') === false);
assert(semver_satisfies('1', '1', '=') === true);
assert(semver_satisfies('1.5', '0.8', '=') === false);
assert(semver_satisfies('1.2', '2.2', '=') === false);
assert(semver_satisfies('3.0', '1.5', '=') === false);
assert(semver_satisfies('1.0', '1.0', '=') === true);
assert(semver_satisfies('1.0.9', '1.0.0', '=') === false);
assert(semver_satisfies('1.1.5', '1.1.9', '=') === false);
assert(semver_satisfies('1.2.2', '1.2.9', '=') === false);
assert(semver_satisfies('1.0.0', '1.0.0', '=') === true);

assert(semver_satisfies('1', '0', '>') === true);
assert(semver_satisfies('1', '3', '>') === false);
assert(semver_satisfies('1', '1', '>') === false);
assert(semver_satisfies('1.5', '0.8', '>') === true);
assert(semver_satisfies('1.2', '2.2', '>') === false);
assert(semver_satisfies('3.0', '1.5', '>') === true);
assert(semver_satisfies('1.0', '1.0', '>') === false);
assert(semver_satisfies('1.0.9', '1.0.0', '>') === true);
assert(semver_satisfies('1.1.5', '1.1.9', '>') === false);
assert(semver_satisfies('1.2.2', '1.2.9', '>') === false);
assert(semver_satisfies('1.0.0', '1.0.0', '>') === false);

assert(semver_satisfies('1', '0', '<') === false);
assert(semver_satisfies('1', '3', '<') === true);
assert(semver_satisfies('1', '1', '<') === false);
assert(semver_satisfies('1.5', '0.8', '<') === false);
assert(semver_satisfies('1.2', '2.2', '<') === true);
assert(semver_satisfies('3.0', '1.5', '<') === false);
assert(semver_satisfies('1.0', '1.0', '<') === false);
assert(semver_satisfies('1.0.9', '1.0.0', '<') === false);
assert(semver_satisfies('1.1.5', '1.1.9', '<') === true);
assert(semver_satisfies('1.2.2', '1.2.9', '<') === true);
assert(semver_satisfies('1.0.0', '1.0.0', '<') === false);

assert(semver_satisfies('1', '0', '^') === false);
assert(semver_satisfies('1', '3', '^') === false);
assert(semver_satisfies('1', '1', '^') === true);
assert(semver_satisfies('1.5', '0.8', '^') === false);
assert(semver_satisfies('1.2', '2.2', '^') === false);
assert(semver_satisfies('3.0', '1.5', '^') === false);
assert(semver_satisfies('1.0', '1.0', '^') === true);
assert(semver_satisfies('1.0.9', '1.0.0', '^') === true);
assert(semver_satisfies('1.1.5', '1.1.9', '^') === true);
assert(semver_satisfies('1.3.2', '1.1.9', '^') === true);
assert(semver_satisfies('1.1.2', '1.5.9', '^') === true);
assert(semver_satisfies('0.1.2', '1.5.9', '^') === false);
assert(semver_satisfies('0.1.2', '0.2.9', '^') === false);
assert(semver_satisfies('1.2.2', '1.2.9', '^') === true);
assert(semver_satisfies('1.0.0', '1.0.0', '^') === true);

assert(semver_satisfies('1', '0', '~') === false);
assert(semver_satisfies('1', '3', '~') === false);
assert(semver_satisfies('1', '1', '~') === true);
assert(semver_satisfies('1.5', '0.8', '~') === false);
assert(semver_satisfies('1.2', '2.2', '~') === false);
assert(semver_satisfies('3.0', '1.5', '~') === false);
assert(semver_satisfies('1.0', '1.0', '~') === true);
assert(semver_satisfies('1.0.9', '1.0.0', '~') === true);
assert(semver_satisfies('1.1.5', '1.1.9', '~') === true);
assert(semver_satisfies('1.1.9', '1.1.3', '~') === true);
assert(semver_satisfies('1.2.2', '1.3.9', '~') === false);
assert(semver_satisfies('1.0.0', '1.0.0', '~') === true);
?>
--EXPECT--
