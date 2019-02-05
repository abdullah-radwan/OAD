# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## 1.0.5 Beta - 2019-02-05
First release of beta.

### Added
- Failed message if adding add-on to the database failed.
- Show a dialog during operations.
- Show all add-ons in the database option.
- Remove the installed file if the installation failed.

## Changed
- Using QArchive instead of Quazip, better support to Qt event loop and threading.

### Fixed
- Files in the main directory are backed up as folders.