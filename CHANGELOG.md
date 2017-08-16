# Change Log

## [Unreleased]
### Updates
- Enable static boost linking by default

## [1.0.0-beta.16] - 13-08-2017
### Updates
- Updated to match same configuration name

### Changes
- Changed header text for files

## [1.0.0-beta.15] - 07-08-2017
### Changes
- Minor speed improvement with bulk loader when connecting

## [1.0.0-beta.14] - 03-08-2017
### Fixes
- Fixed issue with logging DEBUG when built with Release build (use Easylogging++ v9.95.0+)

### Added
- Use of `CHECK_TOKENS` server flag to reduce overhead of pulling token when not needed
- Ability to re-estabilish connection if disconnected from remote

### Changed
- Increased `TOUCH_THRESHOLD` to 2 minutes

## [1.0.0-beta.13] - 02-08-2017
### Fixed
- Fixed compression flag

### Added
- Internal logging level helper enum class

## [1.0.0-beta.12] - 27-07-2017
### Added
- Ability to set internal logging level via configuration using `internal_logging_level`

### Fixed
- Fixed issue with pinging client when client_age < 60

## [1.0.0-beta.11] - 22-07-2017
### Added
- Provide RSA key secret with `secret_key`

## [1.0.0-beta.4] - 07-07-2017
### Fixed
- Fixed dead lock on `reset()`

## [1.0.0-beta.3] - 09-05-2017
### Fixed
- Fixed issue with failing to connect to token and/or logging server. Now throws exception
- Error text on failure
- Fixed exception throwing in `connect()`
- Fixed issue with re-connecting broken socket

## [1.0.0-beta.2] - 20-04-2017
### Added
- Ability to specify server public key
- Added `Residue::setThreadName` (wrapper for `el::Helpers::setThreadName`)
- Added `Residue::setInternalLoggingLevel` for internal logging
- Added `Residue::setApplicationArgs` (wrapper for `START_EASYLOGGINGPP`)
- Added `Residue::reconnect()`
- Added `Residue::moveAccessCodeMap`
- Added `Residue::connect(host, port)` without access code map to be able to connect to different host using existing map
- Added `Residue::enableCrashHandler`
- Added JSON configuration helper `Residue::loadConfiguration`

### Changes
- By default `AutoBulkParams` is now enabled

## [1.0.0-beta] - 31-03-2017
### Added
- Support sending plain log requests in lib

### Fixed
- Issue with dead client and resetting connection caused issue with dispatcher thread in client lib

## [1.0.0-alpha] - 19-03-2017
### Added
- Initial alpha release
