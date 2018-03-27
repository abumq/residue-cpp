# Change Log

## [2.1.1] - 27-03-2018
### Updates
- Moved exceptions out of include for native bindings

## [2.1.0] - 25-03-2018
### API Updates
- Added `loadConfigurationFromJson` to load from JSON parameter
- Added `loadConnection` and `saveConnection`

### Updates
- Updated internal networking library (asio) to 1.12.0
- Client private key secret must be hex encoded now
- Configurations now support `RESIDUE_HOME` environment variable

## [2.0.1] - 21-03-2018
- Fix disconnect

## [2.0.0] - 01-03-2018
### Fixes
- Compatibility for server 2.0.0
- Updated Easylogging++ to 9.96.2

## [1.2.3] - 23-02-2018
### Updates
- Removed plain log request to match server 1.5+
- Updated Easylogging++ to 9.96.1

## [1.2.2]
### Updates
- Upgraded Easylogging++ from 9.95.4 to 9.96.0

## [1.2.1]
### Updates
- Separated translation units for development

## [1.1.0]
### Updates
- Removed dependency on linked boost
- Include easylogging++ with packages to avoid conflicts
- Residue headers are now installed in `residue/` directory since it contains it's own version of Easylogging++

## [1.0.2]
### Updates
- License information update
- Ripe upgraded to 4.1.1

### Fixes
- Fix licensee crash issue

## [1.0.1] - 06-10-2017
### Changes
- Compatibility with residue v1.2.0
- Added `serverVersion` under `Residue::instance()`

## [1.0.0] - 28-09-2017
### Fixes
- Static linking of crypto libs

## [1.0.0-beta.17] - 25-09-2017
### Updates
- A lot of minor internal updates with data types and regression testing

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
