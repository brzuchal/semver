# Semver Extension for PHP

---

## Requirements

* PHP 7.4+

## Installation

**From sources**

```shell
git clone https://github.com/brzuchal/semver.git
cd semver
phpize
./configure --enable-semver
make
make test
make install
```

## Example

```php
$v = "1.5.8-alpha+20200305";

// functional API
if (semver_valid($v)) {
    echo "Version {$v} is valid";
}
if (semver_valid($v) && semver_compare($v, "1.0.0") > 0) {
    echo "Version {$v} is greather than 1.0.0";
}
if (semver_satisfies($v, "1.2", ">=")) {
    echo "Version {$v} satisfies >=1.2.0 constraint";
}
// bumping versions
echo semver_bump_patch(semver_bump_minor(semver_bump("0.0.0"))); // 1.1.1

// OOP API
$version = SemverVersion::createFromString($v);
echo $version->major; // 1
echo $version->minor; // 5
echo $version->patch; // 8
echo $version->prerelease; // alpha
echo $version->metadata; // 20200305

$newVersion = new SemverVersion(1, 5, 8, "beta", date("Ymd"));

```

## API

```php
/**
 * Validates given string if is a valid semver compliant version string
 *
 * @param string $version version string to verify
 * @return true in case of semver compliant version string, false otherwise
 */
function semver_valid(string $version): bool {}

/**
 * Converts given string into int (usefull for ordering)
 *
 * @param string $version version string to convert
 * @return int computed numerical version representation
 * @throws InvalidArgumentException in case of invalid semver non-compliant version string
 */
function semver_numeric(string $version): int {}

/**
 * Compares two semver compliant version strings (usefull for sorting)
 * 
 * @param string $x valid semver version string to compare
 * @param string $y valid semver version string to compare against
 * @return int returns -1 if $x is less than $y, 0 if $x and $y are equal of 1 if $x is greather than $y
 * @throws InvalidArgumentException in case of invalid semver non-compliant version string arguments
 */
function semver_compare(string $x, string $y): int {}

/**
 * Compares if two semver compliant version strings fullfils one of the operators:
 * - `=`  - Equality
 * - `>=` - Higher or equal to
 * - `<=` - Lower or equal to
 * - `<`  - Lower than
 * - `>`  - Higher than
 * - `~`  - Tilde comparison (see https://getcomposer.org/doc/articles/versions.md#tilde-version-range-)
 * - `^`  - Caret comparison (see https://getcomposer.org/doc/articles/versions.md#caret-version-range-)
 *
 * @param string $x valid semver version string to compare
 * @param string $y valid semver version string to compare against
 * @param string $op comparator operator
 * @return int returns -1 if $x is less than $y, 0 if $x and $y are equal of 1 if $x is greather than $y
 * @throws InvalidArgumentException in case of invalid semver non-compliant version string arguments
 */
function semver_satisfies(string $x, string $y, string $op): bool {}

/**
 * Bumps major semver version string
 * 
 * @param string $version valid semver version string
 * @return string returns semver version string with bumped major version
 * @throws InvalidArgumentException in case of invalid semver non-compliant version string argument
 */
function semver_bump(string $version): stirng {}

/**
 * Bumps minor semver version string
 * 
 * @param string $version valid semver version string
 * @return string returns semver version string with bumped minor version
 * @throws InvalidArgumentException in case of invalid semver non-compliant version string argument
 */
function semver_bump_minor(string $version): stirng {}

/**
 * Bumps patch semver version string
 * 
 * @param string $version valid semver version string
 * @return string returns semver version string with bumped patch version
 * @throws InvalidArgumentException in case of invalid semver non-compliant version string argument
 */
function semver_bump_patch(string $version): stirng {}

class SemverVersion
{
    public readonly int $major = 0;
    public readonly int $minor = 0;
    public readonly int $patch = 0;
    public readonly ?string $prerelease = null;
    public readonly ?string $metadata = null;

    /**
     * Parses semver compliant version string and create SemverVersion object
     * 
     * @param string $version valid semver version string
     * @return SemverVersion returns semver version object
     * @throws InvalidArgumentException in case of invalid semver non-compliant version string argument
     */
    public static function createFromString(string $version): self {}

    /**
     * Create SemverVersion object
     * 
     * @param int $major non-negative major version number
     * @param int $minor non-negative minor version number
     * @param int $patch non-negative patch version number
     * @param string|null $prerelease valid semver prerelease string
     * @param string|null $metadata valid semver metadata string
     * @throws InvalidArgumentException in case of invalid semver non-compliant version arguments
     */
    public function __construct(int $major, int $minor, int $patch, ?string $prerelease = null, ?string $metadata = null) {}

    /**
     * Renders SemverVersion object into semver version string
     * 
     * @return string returns rendered semver version string
     */
    public function __toString(): string {}
}
```

## TODO

Things to add to `SemverVersion` class:

* [ ] comparison operator overloading for `SemverVersion` object instances, for eg.: `$versionX > $versionY`
* [ ] casting to string overloading for `SemverVersion` object instances, for eg.: `(int) SemverVersion::createFromString("1.3.6"); // 136`
* [ ] checking operator satisfaction for `SemverVersion` object instance: `SemverVersion::satisfies(SemverVersion $version, string $op): bool`
* [ ] comparing against another `SemverVersion` object instance: `SemverVersion::compare(SemverVersion $version): int`
* [ ] validating version string `SemverVersion::isValid(string $version): bool` in form of static method doing the same as `semver_valid(string $version)`
* [ ] buming version, for eg.: `SemverVersion::bump(): SemverVersion`, `SemverVersion::bumpMinor(): SemverVersion` and `SemverVersion::bumpPatch(): SemverVersion`

Things to do with new `SemverComparator` class:

* [ ] static comparison methods, like for eg.: 
    ```
    SemverComparator::greaterThan(SemverVersion $x, SemverVersion $y): bool;
    SemverComparator::greaterThanOrEqual(SemverVersion $x, SemverVersion $y): bool;
    SemverComparator::lessThan(SemverVersion $x, SemverVersion $y): bool;
    SemverComparator::lessThanOrEqual(SemverVersion $x, SemverVersion $y): bool;
    SemverComparator::equal(SemverVersion $x, SemverVersion $y): bool;
    ```
* [ ] static sorting methods, like for eg.:
    ```
    SemverVersion::sort(array $versions): array;
    SemverVersion::rsort(array $versions): array;
    ```

Things to do with new `SemverConstraint` class:

* [ ] matching `SemverVersion` object instances against complex version constraints, like for eg.: `^5.7 || ^6.5 || ^7.5 || ^8.1`, `1.3.*`, `~1.3.2`, `^1.3.2`, `>=1.3.2` or `1.2.x` (also multi constraint ranges like `>=1.2.3 <=2.3.4`)
    ```
    SemverConstraint::__construct(string $constraint);
    SemverConstraint::match(SemverVersion $version): bool;
    SemverConstraint::minVersion(array $versions): SemverVersion;
    SemverConstraint::maxVersion(array $versions): SemverVersion;
    ```
