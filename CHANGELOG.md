# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## 1.0.3 - 2018-10-20

### Added
- Support to Zip files
- Ignored files/folders can be specified per add-on
- More than 50 add-ons to the database

### Fixed
- Backup files are saved in the main program folder

### Changed
- Remove button in database editor removes the selected file instead of the add-on
- New entries in add ignored file/folder are added to old entries, not override
- Remap the project structure

## 1.0.2 - 2018-10-05

### Added
- Ability to manage multiple Orbiter installation
- Ability to detect base add-ons
- Settings window
- Snapshots
- Database editor
- 4 add-ons to the default database
- Ignored files/folders

### Fixed
- Several language mistakes
- Faulty config writer

### Changed
- Install/Add dialog renamed to install dialog, and can be used only to install. To add an add-on, add it in the database editor.


## 1.0.1 - 2018-09-21

### Added
- Enable and disable feature
- OverriderMap section in config file
- GitHub repository releases URL in about dialog
- Rescan add-ons button

### Fixed
- Empty directory leaved when uninstall an add-on
- Readme file and program language mistakes
- Faulty orbiter.exe detector
- Include config.cfg file

### Changed
- Config file AddonsData section renamed to DatabaseMap
- Rename the add-on name in the database with the entered name if it contains the same files
- Re-format the manual

### Removed
- Remove GPL3 License from about dialog
- Remove (User) suffix when add a new add-on